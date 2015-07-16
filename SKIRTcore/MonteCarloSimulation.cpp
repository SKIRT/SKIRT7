/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "DustGridStructure.hpp"
#include "DustMix.hpp"
#include "DustSystem.hpp"
#include "FatalError.hpp"
#include "IdenticalAssigner.hpp"
#include "Instrument.hpp"
#include "InstrumentSystem.hpp"
#include "Log.hpp"
#include "MonteCarloSimulation.hpp"
#include "NR.hpp"
#include "Parallel.hpp"
#include "ParallelFactory.hpp"
#include "PeerToPeerCommunicator.hpp"
#include "PhotonPackage.hpp"
#include "Random.hpp"
#include "StellarSystem.hpp"
#include "TimeLogger.hpp"
#include "WavelengthGrid.hpp"

using namespace std;

////////////////////////////////////////////////////////////////////

MonteCarloSimulation::MonteCarloSimulation()
    : _is(0), _packages(0), _continuousScattering(false), _lambdagrid(0), _ss(0), _ds(0), _assigner(0)
{
}

////////////////////////////////////////////////////////////////////

void MonteCarloSimulation::setupSelfBefore()
{
    Simulation::setupSelfBefore();

    // protect implementation limit
    if (_packages > 1e15)
        throw FATALERROR("Number of photon packages is larger than implementation limit of 1e15");
    if (_packages < 0)
        throw FATALERROR("Number of photon packages is negative");

    if (!_lambdagrid) throw FATALERROR("Wavelength grid was not set");
    if (!_ss) throw FATALERROR("Stellar system was not set");
    if (!_is) throw FATALERROR("Instrument system was not set");
    // dust system is optional; nr of packages has a valid default

    // If no assigner was set, use an IdenticalAssigner as default
    if (!_assigner) setAssigner(new IdenticalAssigner(this));
}

////////////////////////////////////////////////////////////////////

void MonteCarloSimulation::setChunkParams(double packages)
{
    // Cache the number of wavelengths
    _Nlambda = _lambdagrid->Nlambda();

    // Determine the number of chunks and the corresponding chunk size
    if (packages <= 0)
    {
        _Nchunks = 0;
        _chunksize = 0;
        _Npp = 0;
    }
    else
    {
        // Get the number of processes and threads per process
        int Nprocs = _comm->size();
        int Nthreads = _parfac->maxThreadCount();

        // Set the number of chunks per wavelength, depending on the parallelization mode
        if (Nprocs * Nthreads == 1) _Nchunks = 1;
        else if (Nprocs == 1) _Nchunks = ceil( std::max({packages/1e7, 10.*Nthreads/_Nlambda}));
        else _Nchunks = ceil( std::max({10.*Nprocs, packages/1e7, 10.*Nthreads*Nprocs/_Nlambda}));

        // Calculate the size of the chunks and the definitive number of photon packages per wavelength
        _chunksize = ceil(packages/_Nchunks);
        _Npp = _Nchunks * _chunksize;
    }

    // Determine the log frequency; continuous scattering is much slower!
    _logchunksize = _continuousScattering ? 5000 : 50000;

    // Assign the _Nlambda x _Nchunks different chunks to the different parallel processes
    _assigner->assign(_Nlambda, _Nchunks);
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
    _packages = value;
}

////////////////////////////////////////////////////////////////////

double MonteCarloSimulation::packages() const
{
    return _packages;
}

////////////////////////////////////////////////////////////////////

void MonteCarloSimulation::setContinuousScattering(bool value)
{
    _continuousScattering = value;
}

////////////////////////////////////////////////////////////////////

void MonteCarloSimulation::setAssigner(ProcessAssigner* value)
{
    if (_assigner) delete _assigner;
    _assigner = value;
    if (_assigner) _assigner->setParent(this);
}

////////////////////////////////////////////////////////////////////

ProcessAssigner* MonteCarloSimulation::assigner() const
{
    return _assigner;
}

////////////////////////////////////////////////////////////////////

bool MonteCarloSimulation::continuousScattering() const
{
    return _continuousScattering;
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

    _log->info("(" + QString::number(_Npp) + " photon packages for "
               + (_Nlambda==1 ? QString("a single wavelength") : QString("each of %1 wavelengths").arg(_Nlambda))
               + ")");

    if (_comm->isMultiProc()) _log->info("(" + QString::number(_assigner->nvalues()*_chunksize/_Nlambda)
                                         + " photon packages per wavelength per process)");

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
        double completed = _Ndone * 100. / (_assigner->nvalues()*_chunksize);
        _log->info("Launched " + _phase + " photon packages: " + QString::number(completed,'f',1) + "%");
    }
}

////////////////////////////////////////////////////////////////////

void MonteCarloSimulation::runstellaremission()
{
    TimeLogger logger(_log, "the stellar emission phase");
    setChunkParams(_packages);
    initprogress("stellar emission");
    Parallel* parallel = find<ParallelFactory>()->parallel();
    parallel->call(this, &MonteCarloSimulation::dostellaremissionchunk, _assigner);

    // Wait for the other processes to reach this point
    _comm->wait("the stellar emission phase");
}

////////////////////////////////////////////////////////////////////

void MonteCarloSimulation::dostellaremissionchunk(size_t index)
{
    int ell = index % _Nlambda;
    double L = _ss->luminosity(ell)/_Npp;
    if (L > 0)
    {
        double Lmin = 1e-6 * L;
        PhotonPackage pp,ppp;

        quint64 remaining = _chunksize;
        while (remaining > 0)
        {
            quint64 count = qMin(remaining, _logchunksize);
            for (quint64 i=0; i<count; i++)
            {
                _ss->launch(&pp,ell,L);
                peeloffemission(&pp,&ppp);
                if (_ds) while (true)
                {
                    _ds->fillOpticalDepth(&pp);
                    if (_continuousScattering) continuouspeeloffscattering(&pp,&ppp);
                    simulateescapeandabsorption(&pp,_ds->dustemission());
                    if (pp.luminosity() <= Lmin && pp.nScatt() > 2) break;
                    simulatepropagation(&pp);
                    if (!_continuousScattering) peeloffscattering(&pp,&ppp);
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

    // Determine the weighting factors of the phase functions corresponding to
    // the different dust components: each component h is weighted by kappasca(h)*rho(m,h)
    QVarLengthArray<double,4> wv(Ncomp);
    if (Ncomp==1)
        wv[0] = 1.0;
    else
    {
        int m = _ds->whichcell(bfr);
        if (m==-1) return; // abort peel-off
        for (int h=0; h<Ncomp; h++) wv[h] = _ds->mix(h)->kappasca(ell) * _ds->density(m,h);
        double sum = 0;
        for (int h=0; h<Ncomp; h++) sum += wv[h];
        if (sum<=0) return; // abort peel-off
        for (int h=0; h<Ncomp; h++) wv[h] /= sum;
    }

    // Now do the actual peel-off
    foreach (Instrument* instr, _is->instruments())
    {
        Direction bfkobs = instr->bfkobs(bfr);
        Direction bfkx = instr->bfkx();
        Direction bfky = instr->bfky();
        double I = 0, Q = 0, U = 0, V = 0;
        for (int h=0; h<Ncomp; h++)
        {
            DustMix* mix = _ds->mix(h);
            double w = wv[h] * mix->phaseFunctionValue(pp, bfkobs);
            StokesVector sv;
            mix->scatteringPeelOffPolarization(&sv, pp, bfkobs, bfkx, bfky);
            I += w * sv.stokesI();
            Q += w * sv.stokesQ();
            U += w * sv.stokesU();
            V += w * sv.stokesV();
        }
        ppp->launchScatteringPeelOff(pp, bfkobs, I);
        ppp->setStokes(I, Q, U, V);
        instr->detect(ppp);
    }
}

////////////////////////////////////////////////////////////////////

void MonteCarloSimulation::continuouspeeloffscattering(PhotonPackage *pp, PhotonPackage *ppp)
{
    int ell = pp->ell();
    Position bfr = pp->position();
    Direction bfk = pp->direction();

    int Ncomp = _ds->Ncomp();
    QVarLengthArray<double,4> kappascav(Ncomp);
    QVarLengthArray<double,4> kappaextv(Ncomp);
    for (int h=0; h<Ncomp; h++)
    {
        DustMix* mix = _ds->mix(h);
        kappascav[h] = mix->kappasca(ell);
        kappaextv[h] = mix->kappaext(ell);
    }

    int Ncells = pp->size();
    for (int n=0; n<Ncells; n++)
    {
        int m = pp->m(n);
        if (m!=-1)
        {
            QVarLengthArray<double,4> wv(Ncomp);
            double ksca = 0.0;
            double kext = 0.0;
            for (int h=0; h<Ncomp; h++)
            {
                double rho = _ds->density(m,h);
                wv[h] = rho*kappascav[h];
                ksca += rho*kappascav[h];
                kext += rho*kappaextv[h];
            }
            if (ksca>0.0)
            {
                for (int h=0; h<Ncomp; h++) wv[h] /= ksca;
                double albedo = ksca/kext;
                double tau0 = (n==0) ? 0.0 : pp->tau(n-1);
                double dtau = pp->dtau(n);
                double s0 = (n==0) ? 0.0 : pp->s(n-1);
                double ds = pp->ds(n);
                double factorm = albedo * exp(-tau0) * (-expm1(-dtau));
                double s = s0 + _random->uniform()*ds;
                Position bfrnew(bfr+s*bfk);
                foreach (Instrument* instr, _is->instruments())
                {
                    Direction bfkobs = instr->bfkobs(bfrnew);
                    Direction bfkx = instr->bfkx();
                    Direction bfky = instr->bfky();
                    double I = 0, Q = 0, U = 0, V = 0;
                    for (int h=0; h<Ncomp; h++)
                    {
                        DustMix* mix = _ds->mix(h);
                        double w = wv[h] * mix->phaseFunctionValue(pp, bfkobs);
                        StokesVector sv;
                        mix->scatteringPeelOffPolarization(&sv, pp, bfkobs, bfkx, bfky);
                        I += w * sv.stokesI();
                        Q += w * sv.stokesQ();
                        U += w * sv.stokesU();
                        V += w * sv.stokesV();
                    }
                    ppp->launchScatteringPeelOff(pp, bfrnew, bfkobs, factorm*I);
                    ppp->setStokes(I, Q, U, V);
                    instr->detect(ppp);
                }
            }
        }
    }
}

////////////////////////////////////////////////////////////////////

void MonteCarloSimulation::simulateescapeandabsorption(PhotonPackage* pp, bool dustemission)
{
    double taupath = pp->tau();
    int ell = pp->ell();
    double L = pp->luminosity();
    bool ynstellar = pp->isStellar();
    int Ncomp = _ds->Ncomp();

    // Easy case: there is only one dust component
    if (Ncomp==1)
    {
        double albedo = _ds->mix(0)->albedo(ell);
        double expfactor = -expm1(-taupath);
        if (dustemission)
        {
            int Ncells = pp->size();
            for (int n=0; n<Ncells; n++)
            {
                int m = pp->m(n);
                if (m!=-1)
                {
                    double taustart = (n==0) ? 0.0 : pp->tau(n-1);
                    double dtau = pp->dtau(n);
                    double expfactorm = -expm1(-dtau);
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
    // The absorption/scattering in each cell is weighted by the density contribution of the component.
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
        int Ncells = pp->size();
        double Lsca = 0.0;
        for (int n=0; n<Ncells; n++)
        {
            int m = pp->m(n);
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
                double taustart = (n==0) ? 0.0 : pp->tau(n-1);
                double dtau = pp->dtau(n);
                double expfactorm = -expm1(-dtau);
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

void MonteCarloSimulation::simulatepropagation(PhotonPackage* pp)
{
    double taupath = pp->tau();
    double tau = _random->exponcutoff(taupath);
    double s = pp->pathlength(tau);
    pp->propagate(s);
}

////////////////////////////////////////////////////////////////////

void MonteCarloSimulation::simulatescattering(PhotonPackage* pp)
{
    // Randomly select a dust mix; the probability of each dust component h is weighted by kappasca(h)*rho(m,h)
    DustMix* mix = _ds->randomMixForPosition(pp->position(), pp->ell());

    // Now perform the scattering using this dust mix
    Direction bfknew = mix->scatteringDirectionAndPolarization(pp, pp);
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
