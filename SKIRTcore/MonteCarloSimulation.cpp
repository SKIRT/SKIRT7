/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include "DustGridStructure.hpp"
#include "DustMix.hpp"
#include "DustSystem.hpp"
#include "DustSystemPath.hpp"
#include "FatalError.hpp"
#include "Instrument.hpp"
#include "InstrumentSystem.hpp"
#include "Log.hpp"
#include "MonteCarloSimulation.hpp"
#include "NR.hpp"
#include "Parallel.hpp"
#include "ParallelFactory.hpp"
#include "PhotonPackage.hpp"
#include "Random.hpp"
#include "StellarSystem.hpp"
#include "TimeLogger.hpp"
#include "WavelengthGrid.hpp"

using namespace std;

////////////////////////////////////////////////////////////////////

MonteCarloSimulation::MonteCarloSimulation()
    : _lambdagrid(0), _ss(0), _ds(0), _is(0), _packages(0)
{
}

////////////////////////////////////////////////////////////////////

void MonteCarloSimulation::setupSelfBefore()
{
    Simulation::setupSelfBefore();

    if (!_lambdagrid) throw FATALERROR("Wavelength grid was not set");
    if (!_ss) throw FATALERROR("Stellar system was not set");
    if (!_is) throw FATALERROR("Instrument system was not set");
    // dust system is optional; nr of packages has a valid default
}

////////////////////////////////////////////////////////////////////

void MonteCarloSimulation::setupSelfAfter()
{
    Simulation::setupSelfAfter();

    // cache frequently used parameters
    _Nlambda = _lambdagrid->Nlambda();

    // determine the number of chunks and the corresponding chunk size
    if (_packages <= 0)
    {
        _Nchunks = 0;
        _chunksize = 0;
        _Npp = 0;
    }
    else
    {
        int Nthreads = _parfac->maxThreadCount();
        if (Nthreads == 1) _Nchunks = 1;
        else _Nchunks = ceil( qMin(_packages/2e4, qMax(_packages/1e7, 10.*Nthreads/_Nlambda)) );
        _chunksize = ceil(_packages/_Nchunks);
        _Npp = _Nchunks*_chunksize;
    }
}

////////////////////////////////////////////////////////////////////

void MonteCarloSimulation::setInstrumentSystem(InstrumentSystem* value)
{
    if (_is) delete _is;
    _is = value;
    if (_is) _is->setParent(this);
}

////////////////////////////////////////////////////////////////////

InstrumentSystem* MonteCarloSimulation::instrumentSystem() const
{
    return _is;
}

////////////////////////////////////////////////////////////////////

void MonteCarloSimulation::setPackages(double value)
{
    // protect implementation limit
    if (value > 1e15)
        throw FATALERROR("Number of photon packages is larger than implementation limit of 1e15");
    if (value < 0)
        throw FATALERROR("Number of photon packages is negative");

    _packages = value;
}

////////////////////////////////////////////////////////////////////

double MonteCarloSimulation::packages() const
{
    return _packages;
}

////////////////////////////////////////////////////////////////////

int MonteCarloSimulation::dimension() const
{
    return qMax(_ss->dimension(), _ds ? _ds->dimension() : 1);
}

////////////////////////////////////////////////////////////////////

void MonteCarloSimulation::initprogress(QString phase)
{
    _phase = phase;
    _Ndone = 0;

    _log->info("(" + QString::number(_packages,'g') + " photon packages for "
               + (_Nlambda==1 ? QString("a single wavelength") : QString("each of %1 wavelengths").arg(_Nlambda))
               + ")");
    _timer.start();
}

////////////////////////////////////////////////////////////////////

void MonteCarloSimulation::logprogress(quint64 extraDone)
{
    // accumulate the work already done
    _Ndone.fetch_add(extraDone);

    // space the messages at least 3 seconds apart; in the interest of speed,
    // we do this without locking, so once in a while two consecutive messages may slip through
    if (_timer.elapsed() > 3000)
    {
        _timer.restart();
        double completed = _Ndone * 100. / (_Npp*_Nlambda);
        _log->info("Launched " + _phase + " photon packages: " + QString::number(completed,'f',1) + "%");
    }
}

////////////////////////////////////////////////////////////////////

void MonteCarloSimulation::runstellaremission()
{
    TimeLogger logger(_log, "the stellar emission phase");
    initprogress("stellar emission");
    Parallel* parallel = find<ParallelFactory>()->parallel();
    parallel->call(this, &MonteCarloSimulation::dostellaremissionchunk, _Nchunks*_Nlambda);
}

////////////////////////////////////////////////////////////////////

void MonteCarloSimulation::dostellaremissionchunk(size_t index)
{
    int ell = index % _Nlambda;
    double L = _ss->luminosity(ell)/_Npp;
    if (L > 0)
    {
        double Lmin = 1e-4 * L;
        PhotonPackage pp,ppp;
        DustSystemPath dsp;

        quint64 remaining = _chunksize;
        while (remaining > 0)
        {
            quint64 count = qMin(remaining, LOG_CHUNK_SIZE);
            for (quint64 i=0; i<count; i++)
            {
                _ss->launch(&pp,ell,L);
                peeloffemission(&pp,&ppp);
                if (_ds) while (true)
                {
                    fillDustSystemPath(&pp,&dsp);
                    simulateescapeandabsorption(&pp,&dsp,_ds->dustemission());
                    if (pp.luminosity() <= Lmin) break;
                    simulatepropagation(&pp,&dsp);
                    peeloffscattering(&pp,&ppp);
                    simulatescattering(&pp);
                }
            }
            logprogress(count);
            remaining -= count;
        }
    }
    else logprogress(_chunksize);
}

////////////////////////////////////////////////////////////////////

void MonteCarloSimulation::fillDustSystemPath(PhotonPackage* pp, DustSystemPath* dsp) const
{
    double tau = 0.0;
    if (_ds) tau = _ds->opticaldepth(pp,dsp);
    if (tau<0.0 || std::isnan(tau) || std::isinf(tau))
        throw FATALERROR("The optical depth along the path is not a positive number: tau = " + QString::number(tau));
}

////////////////////////////////////////////////////////////////////

void MonteCarloSimulation::peeloffemission(PhotonPackage* pp, PhotonPackage* ppp)
{
    Position bfr = pp->position();

    foreach (Instrument* instr, _is->instruments())
    {
        Direction bfknew = instr->bfkobs(bfr);
        ppp->launchEmissionPeelOff(pp, bfknew);
        instr->detect(ppp);
    }
}

////////////////////////////////////////////////////////////////////

void MonteCarloSimulation::peeloffscattering(PhotonPackage* pp, PhotonPackage* ppp)
{
    int Ncomp = _ds->Ncomp();
    int ell = pp->ell();
    Position bfr = pp->position();

    // Determination of the weighting factors of the phase functions corresponding to
    // the different dust components: each component h is weighted by kappasca(h)*rho(m,h).
    Array wv(Ncomp);
    if (Ncomp==1)
        wv[0] = 1.0;
    else
    {
        int m = _ds->whichcell(bfr);
        if (m==-1) throw FATALERROR("The scattering event seems to take place outside the dust grid");
        for (int h=0; h<Ncomp; h++) wv[h] = _ds->mix(h)->kappasca(ell) * _ds->density(m,h);
        wv /= wv.sum();
    }

    // Now do the actual peel-off
    Direction bfkold = pp->direction();
    foreach (Instrument* instr, _is->instruments())
    {
        Direction bfknew = instr->bfkobs(bfr);
        double w = 0.0;
        for (int h=0; h<Ncomp; h++)
            w += wv[h] * _ds->mix(h)->phasefunction(ell,bfkold,bfknew);
        ppp->launchScatteringPeelOff(pp, bfknew, w);
        instr->detect(ppp);
    }
}

////////////////////////////////////////////////////////////////////

void MonteCarloSimulation::simulateescapeandabsorption(PhotonPackage* pp, DustSystemPath* dsp, bool dustemission)
{
    const double taumin = 1e-3;
    double taupath = dsp->opticaldepth();
    int ell = pp->ell();
    double L = pp->luminosity();
    bool ynstellar = pp->isStellar();
    int Ncomp = _ds->Ncomp();

    // Easy case: there is only one dust component
    if (Ncomp==1)
    {
        double albedo = _ds->mix(0)->albedo(ell);
        double expfactor = (taupath>taumin) ? 1.0-exp(-taupath) : taupath*(1.0-0.5*taupath);
        if (dustemission)
        {
            int Ncells = dsp->size();
            for (int n=0; n<Ncells; n++)
            {
                int m = dsp->cellnumber(n);
                if (m!=-1)
                {
                    double taustart = (n==0) ? 0.0 : dsp->tau(n-1);
                    double dtau = dsp->dtau(n);
                    double expfactorm = (dtau>taumin) ? 1.0-exp(-dtau) : dtau*(1.0-0.5*dtau);
                    double Lintm = L * exp(-taustart) * expfactorm;
                    double Labsm = (1.0-albedo) * Lintm;
                    _ds->absorb(m,ell,Labsm,ynstellar);
                }
            }
        }
        double Lsca = L * albedo * expfactor;
        pp->setLuminosity(Lsca);
    }

    // Difficult case: there are different dust components.
    // The absorption/scattering in each cell has to weighted by the density contribution of the component.
    else
    {
        Array kappascav(Ncomp);
        Array kappaextv(Ncomp);
        for (int h=0; h<Ncomp; h++)
        {
            DustMix* mix = _ds->mix(h);
            kappascav[h] = mix->kappasca(ell);
            kappaextv[h] = mix->kappaext(ell);
        }
        int Ncells = dsp->size();
        double Lsca = 0.0;
        for (int n=0; n<Ncells; n++)
        {
            int m = dsp->cellnumber(n);
            if (m!=-1)
            {
                double ksca = 0.0;
                double kext = 0.0;
                for (int h=0; h<Ncomp; h++)
                {
                    double rho = _ds->density(m,h);
                    ksca += rho*kappascav[h];
                    kext += rho*kappaextv[h];
                }
                double albedo = (kext>0.0) ? ksca/kext : 0.0;
                double taustart = (n==0) ? 0.0 : dsp->tau(n-1);
                double dtau = dsp->dtau(n);
                double expfactorm = (dtau>taumin) ? 1.0-exp(-dtau) : dtau*(1.0-0.5*dtau);
                double Lintm = L * exp(-taustart) * expfactorm;
                double Lscam = albedo * Lintm;
                Lsca += Lscam;
                if (dustemission)
                {
                    double Labsm = (1.0-albedo) * Lintm;
                    _ds->absorb(m,ell,Labsm,ynstellar);
                }
            }
        }
        pp->setLuminosity(Lsca);
    }
}

////////////////////////////////////////////////////////////////////

void MonteCarloSimulation::simulatepropagation(PhotonPackage* pp, DustSystemPath* dsp)
{
    double taupath = dsp->opticaldepth();
    double tau = _random->exponcutoff(taupath);
    double s = dsp->pathlength(tau);
    pp->propagate(s);
}

////////////////////////////////////////////////////////////////////

void MonteCarloSimulation::simulatescattering(PhotonPackage* pp)
{
    int ell = pp->ell();
    int Ncomp = _ds->Ncomp();

    // Select a phase function. If we have just a single dust component, this is simple.
    // If there are multiple dust components, we randomly select the phase function, where the
    // probability of the phase function of each dust component h is weighted by kappasca(h)*rho(m,h).
    int hmix = 0;
    if (Ncomp>1)
    {
        Position bfr = pp->position();
        int m = _ds->whichcell(bfr);
        if (m==-1) throw FATALERROR("The scattering event seems to take place outside the dust grid");
        Array Xv;
        NR::cdf(Xv, Ncomp, [this,ell,m](int h){return _ds->mix(h)->kappasca(ell)*_ds->density(m,h);} );
        hmix = NR::locate_clip(Xv, _random->uniform());
    }
    DustMix* mix = _ds->mix(hmix);

    // Now just perform the scattering
    Direction bfkold = pp->direction();
    Direction bfknew = mix->generatenewdirection(ell,bfkold);
    pp->scatter(bfknew);
}

////////////////////////////////////////////////////////////////////

void MonteCarloSimulation::write()
{
    TimeLogger logger(_log, "writing results");
    if (_is) _is->write();
    if (_ds) _ds->write();
}

////////////////////////////////////////////////////////////////////
