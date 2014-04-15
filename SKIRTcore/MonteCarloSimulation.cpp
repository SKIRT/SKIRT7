/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include "AngularDistribution.hpp"
#include "DustGridStructure.hpp"
#include "DustMix.hpp"
#include "DustSystem.hpp"
#include "DustSystemPath.hpp"
#include "FatalError.hpp"
#include "Instrument.hpp"
#include "InstrumentSystem.hpp"
#include "MonteCarloSimulation.hpp"
#include "NR.hpp"
#include "PeelOffPhotonPackage.hpp"
#include "Random.hpp"
#include "StellarSystem.hpp"
#include "TimeLogger.hpp"

using namespace std;

////////////////////////////////////////////////////////////////////

MonteCarloSimulation::MonteCarloSimulation()
    : _lambdagrid(0), _ss(0), _ds(0), _is(0), _Nchunks(1)
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
    if (value > 2e9*CHUNKSIZE+1)  // add 1 to allow for rounding errors
        throw FATALERROR("Number of photon packages is larger than implementation limit of 2e13");
    if (value < 0)
        throw FATALERROR("Number of photon packages is negative");

    // convert to (rounded) number of chunks, with minimum of 1 chunk unless # packages is identical to zero
    _Nchunks = static_cast<int>((value/CHUNKSIZE)+0.5);
    if (_Nchunks < 1 && value > 0) _Nchunks = 1;
}

////////////////////////////////////////////////////////////////////

double MonteCarloSimulation::packages() const
{
    return static_cast<double>(_Nchunks)*CHUNKSIZE;
}

////////////////////////////////////////////////////////////////////

int MonteCarloSimulation::dimension() const
{
    return qMax(_ss->dimension(), _ds ? _ds->dimension() : 1);
}

////////////////////////////////////////////////////////////////////

void MonteCarloSimulation::write()
{
    TimeLogger logger(_log, "writing results");
    if (_is) _is->write();
    if (_ds) _ds->write();
}

////////////////////////////////////////////////////////////////////

void MonteCarloSimulation::fillDustSystemPath(PhotonPackage* pp, DustSystemPath* dsp) const
{
    double tau = 0.0;
    if (_ds) tau = _ds->opticaldepth(pp->ell(),pp->position(),pp->direction(),dsp);
    if (tau<0.0 || std::isnan(tau) || std::isinf(tau))
        throw FATALERROR("The optical depth along the path is not a positive number: tau = " + QString::number(tau));
}

////////////////////////////////////////////////////////////////////

void MonteCarloSimulation::peeloffemission(PhotonPackage* pp)
{
    int ell = pp->ell();
    bool ynstellar = pp->ynstellar();
    Position bfr = pp->position();
    double L = pp->luminosity();
    const AngularDistribution* ad = pp->angularDistribution();

    foreach (Instrument* instr, _is->instruments())
    {
        Direction bfknew = instr->bfkobs(bfr);
        double directionbias = ad ? ad->probabilityForDirection(bfr, bfknew) : 1.;
        PeelOffPhotonPackage ppp(ynstellar,ell,bfr,bfknew,L*directionbias,0,_ds);
        instr->detect(&ppp);
    }
}

////////////////////////////////////////////////////////////////////

void MonteCarloSimulation::peeloffscattering(PhotonPackage* pp)
{
    int Ncomp = _ds->Ncomp();
    int ell = pp->ell();
    bool ynstellar = pp->ynstellar();
    Position bfr = pp->position();

    // Determination of the weighting factors of the phase functions corresponding to
    // the different dust components: each component h is weighted by kappasca(h)*rho(m,h).
    Array wv(Ncomp);
    if (Ncomp==1)
        wv[0] = 1.0;
    else
    {
        int m = _ds->grid()->whichcell(bfr);
        if (m==-1) throw FATALERROR("The scattering event seems to take place outside the dust grid");
        for (int h=0; h<Ncomp; h++) wv[h] = _ds->mix(h)->kappasca(ell) * _ds->density(m,h);
        wv /= wv.sum();
    }

    // Now do the actual peel-off
    Direction bfkold = pp->direction();
    int nscatt = pp->nscatt()+1;
    foreach (Instrument* instr, _is->instruments())
    {
        Direction bfknew = instr->bfkobs(bfr);
        double w = 0.0;
        for (int h=0; h<Ncomp; h++)
            w += wv[h] * _ds->mix(h)->phasefunction(ell,bfkold,bfknew);
        double L = pp->luminosity() * w;
        PeelOffPhotonPackage ppp(ynstellar,ell,bfr,bfknew,L,nscatt,_ds);
        instr->detect(&ppp);
    }
}

////////////////////////////////////////////////////////////////////

void MonteCarloSimulation::simulateescapeandabsorption(PhotonPackage* pp, DustSystemPath* dsp, bool dustemission)
{
    const double taumin = 1e-3;
    double taupath = dsp->opticaldepth();
    int ell = pp->ell();
    double L = pp->luminosity();
    bool ynstellar = pp->ynstellar();
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
        pp->setluminosity(Lsca);
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
        pp->setluminosity(Lsca);
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
        int m = _ds->grid()->whichcell(bfr);
        if (m==-1) throw FATALERROR("The scattering event seems to take place outside the dust grid");
        Array kapparhov(Ncomp);
        for (int h=0; h<Ncomp; h++) kapparhov[h] = _ds->mix(h)->kappasca(ell) * _ds->density(m,h);
        Array Xv;
        NR::cdf(Xv,kapparhov);
        hmix = NR::locate_clip(Xv, _random->uniform());
   }
    DustMix* mix = _ds->mix(hmix);

    // Now just perform the scattering
    Direction bfkold = pp->direction();
    Direction bfknew = mix->generatenewdirection(ell,bfkold);
    pp->setnscatt(pp->nscatt()+1);
    pp->setdirection(bfknew);
}

////////////////////////////////////////////////////////////////////
