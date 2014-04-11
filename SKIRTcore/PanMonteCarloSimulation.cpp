/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include "DustSystemPath.hpp"
#include "Log.hpp"
#include "NR.hpp"
#include "PanDustSystem.hpp"
#include "PanMonteCarloSimulation.hpp"
#include "PanWavelengthGrid.hpp"
#include "Parallel.hpp"
#include "ParallelFactory.hpp"
#include "PhotonPackage.hpp"
#include "Random.hpp"
#include "SED.hpp"
#include "StellarSystem.hpp"
#include "TimeLogger.hpp"
#include "Units.hpp"

using namespace std;

////////////////////////////////////////////////////////////////////

PanMonteCarloSimulation::PanMonteCarloSimulation()
    : _pds(0)
{
}

////////////////////////////////////////////////////////////////////

void PanMonteCarloSimulation::setupSelfBefore()
{
    MonteCarloSimulation::setupSelfBefore();

    // the number of photon packages per wavelength as a 64-bit integer
    _Npp = static_cast<qint64>(_Nchunks)*CHUNKSIZE;

    // generate 100 messages for each wavelength cycle, but never more than one per chunk
    _Nlog = _lambdagrid->Nlambda() * _Npp / 100;
    if (_Nlog < CHUNKSIZE) _Nlog = CHUNKSIZE;
}

////////////////////////////////////////////////////////////////////

void PanMonteCarloSimulation::setupSelfAfter()
{
    MonteCarloSimulation::setupSelfAfter();

    // correctly size completion counters for progress log
    _Ndone.resize(_lambdagrid->Nlambda());
}

////////////////////////////////////////////////////////////////////

void PanMonteCarloSimulation::setWavelengthGrid(PanWavelengthGrid* value)
{
    if (_lambdagrid) delete _lambdagrid;
    _lambdagrid = value;
    if (_lambdagrid) _lambdagrid->setParent(this);
}

////////////////////////////////////////////////////////////////////

PanWavelengthGrid* PanMonteCarloSimulation::wavelengthGrid() const
{
    return dynamic_cast<PanWavelengthGrid*>(_lambdagrid);
}

////////////////////////////////////////////////////////////////////

void PanMonteCarloSimulation::setStellarSystem(StellarSystem* value)
{
    if (_ss) delete _ss;
    _ss = value;
    if (_ss) _ss->setParent(this);
}

////////////////////////////////////////////////////////////////////

StellarSystem* PanMonteCarloSimulation::stellarSystem() const
{
    return _ss;
}

////////////////////////////////////////////////////////////////////

void PanMonteCarloSimulation::setDustSystem(PanDustSystem* value)
{
    if (_ds) delete _ds;
    _ds = value;
    _pds = value;
    if (_ds) _ds->setParent(this);
}

////////////////////////////////////////////////////////////////////

PanDustSystem* PanMonteCarloSimulation::dustSystem() const
{
    return _pds;
}

////////////////////////////////////////////////////////////////////

bool PanMonteCarloSimulation::parallelPhotonsOfSameWaveLength() const
{
    return false;
}

////////////////////////////////////////////////////////////////////

void PanMonteCarloSimulation::runSelf()
{
    runstellaremission();
    if (_pds && _pds->dustemission())
    {
        if (_pds && _pds->selfAbsorption()) rundustselfabsorption();
        rundustemission();
    }
    write();
}

////////////////////////////////////////////////////////////////////

void PanMonteCarloSimulation::runstellaremission()
{
    TimeLogger logger(_log, "the stellar emission phase");

    initprogress();
    Parallel* parallel = find<ParallelFactory>()->parallel();
    parallel->call(this, &PanMonteCarloSimulation::do_stellaremission_wavelength, _lambdagrid->Nlambda());
}

////////////////////////////////////////////////////////////////////

void PanMonteCarloSimulation::do_stellaremission_wavelength(int ell)
{
    double Ltot = _ss->luminosity(ell);
    if (Ltot>0.0)
    {
        PhotonPackage pp;
        DustSystemPath dsp;
        double L = Ltot / _Npp;
        double Lmin = 1e-4*L;
        for (qint64 index=0; index<_Npp; index++)
        {
            if (index%_Nlog == 0) logprogress("stellar emission", ell, index);
            _ss->launch(&pp,ell,L);
            peeloffemission(&pp);
            if (_ds) while (true)
            {
                fillDustSystemPath(&pp,&dsp);
                simulateescapeandabsorption(&pp,&dsp,_pds->dustemission());
                if (pp.luminosity() <= Lmin) break;
                simulatepropagation(&pp,&dsp);
                peeloffscattering(&pp);
                simulatescattering(&pp);
            }
        }
        logprogress("stellar emission", ell, _Npp);
    }
}

////////////////////////////////////////////////////////////////////

void PanMonteCarloSimulation::rundustselfabsorption()
{
    TimeLogger logger(_log, "the dust self-absorption phase");

    const int Ncyclesmax = 100;
    const double epsmax = 0.005;
    const int Nlambda = _lambdagrid->Nlambda();
    const int Ncells = _ds->Ncells();
    vector<double> Labsdusttotv(Ncyclesmax+1);
    Labsdusttotv[0] = 0.0;

    for (_cycle=1; _cycle<=Ncyclesmax; _cycle++)
    {
        TimeLogger logger(_log, "the dust self-absorption cycle " + QString::number(_cycle));

        // Construct the dust emission spectra
        _log->info("Calculating dust emission spectra...");
        _pds->calculatedustemission();
        _log->info("Dust emission spectra calculated.");

        // Determine the bolometric luminosity that is absorbed in every cell (and that will hence be re-emitted).
        _Labsbolv.resize(Ncells);
        for (int m=0; m<Ncells; m++)
            _Labsbolv[m] = _ds->Labstot(m);

        // Set the absorbed dust luminosity to zero in all cells
        _ds->rebootLabsdust();

        // Run a simulation at every wavelength
        initprogress();
        Parallel* parallel = find<ParallelFactory>()->parallel();
        parallel->call(this, &PanMonteCarloSimulation::do_dustselfabsorption_wavelength, Nlambda);

        // Update the absorbed luminosity in each cell. Save the total absorbed luminosity in the vector Labstotv.
        Labsdusttotv[_cycle] = _ds->Labsdusttot();
        _log->info("The total absorbed stellar luminosity is "
                   + QString::number(_units->obolluminosity(_ds->Labsstellartot())) + " "
                   + _units->ubolluminosity() );
        _log->info("The total absorbed dust luminosity is "
                   + QString::number(_units->obolluminosity(_ds->Labsdusttot())) + " "
                   + _units->ubolluminosity() );

        // Check the criterion to terminate the self-absorption cycle. We use the criterion that the total
        // absorbed dust luminosity should be changed by less than epsmax compared to the previous cycle.
        double eps = fabs((Labsdusttotv[_cycle]-Labsdusttotv[_cycle-1])/Labsdusttotv[_cycle]);
        if (eps<epsmax)
        {
            _log->info("Convergence reached; the last increase in the absorbed dust luminosity was "
                       + QString::number(eps*100, 'f', 2) + "%");
            return;
        }
        else
        {
            _log->info("Convergence not yet reached; the increase in the absorbed dust luminosity was "
                       + QString::number(eps*100, 'f', 2) + "%");
        }
    }
    _log->error("Convergence not yet reached after " + QString::number(Ncyclesmax) + " cycles!");
}

////////////////////////////////////////////////////////////////////

void PanMonteCarloSimulation::do_dustselfabsorption_wavelength(int ell)
{
    const int Ncells = _ds->Ncells();

    // Determine the luminosity to be emitted at this wavelength index
    Array Lv(Ncells);
    for (int m=0; m<Ncells; m++)
    {
        double Labsbol = _Labsbolv[m];
        if (Labsbol>0.0) Lv[m] = Labsbol * _pds->dustluminosity(m,ell);
    }
    double Ltot = Lv.sum();

    // Emit photon packages
    if (Ltot>0.0)
    {
        Array Xv;
        NR::cdf(Xv, Lv);

        PhotonPackage pp;
        DustSystemPath dsp;
        double L = Ltot / _Npp;
        double Lmin = 1e-4*L;
        for (qint64 index=0; index<_Npp; index++)
        {
            if (index%_Nlog == 0) logprogress("self-absorption cycle " + QString::number(_cycle), ell, index);
            double X = _random->uniform();
            int m = NR::locate_clip(Xv,X);
            Position bfr = _ds->randomPositionInCell(m);
            Direction bfk = _random->direction();
            pp.set(false,ell,bfr,bfk,L,0);
            while (true)
            {
                fillDustSystemPath(&pp,&dsp);
                simulateescapeandabsorption(&pp,&dsp,true);
                if (pp.luminosity() <= Lmin) break;
                simulatepropagation(&pp,&dsp);
                simulatescattering(&pp);
            }
        }
        logprogress("self-absorption cycle " + QString::number(_cycle), ell, _Npp);
    }
}

////////////////////////////////////////////////////////////////////

void PanMonteCarloSimulation::rundustemission()
{
    TimeLogger logger(_log, "the dust emission phase");

    // Construct the dust emission spectra
    _log->info("Calculating dust emission spectra...");
    _pds->calculatedustemission();
    _log->info("Dust emission spectra calculated.");

    // perform the actual dust emission
    initprogress();
    Parallel* parallel = find<ParallelFactory>()->parallel();
    parallel->call(this, &PanMonteCarloSimulation::do_dustemission_wavelength, _lambdagrid->Nlambda());
}

////////////////////////////////////////////////////////////////////

void PanMonteCarloSimulation::do_dustemission_wavelength(int ell)
{
    const int Ncells = _ds->Ncells();

    // Determine the luminosity to be emitted at this wavelength index
    Array Lv(Ncells);
    for (int m=0; m<Ncells; m++)
    {
        double Labsbol = _ds->Labstot(m);
        if (Labsbol>0.0) Lv[m] = Labsbol * _pds->dustluminosity(m,ell);
    }
    double Ltot = Lv.sum();

    // Emit photon packages
    if (Ltot>0.0)
    {
        Array Xv;
        NR::cdf(Xv, Lv);

        PhotonPackage pp;
        DustSystemPath dsp;
        double L = Ltot / _Npp;
        double Lmin = 1e-4*L;
        for (qint64 index=0; index<_Npp; index++)
        {
            if (index%_Nlog == 0) logprogress("dust emission", ell, index);
            double X = _random->uniform();
            int m = NR::locate_clip(Xv,X);
            Position bfr = _ds->randomPositionInCell(m);
            Direction bfk = _random->direction();
            pp.set(false,ell,bfr,bfk,L,0);
            peeloffemission(&pp);
            while (true)
            {
                fillDustSystemPath(&pp,&dsp);
                simulateescapeandabsorption(&pp,&dsp,false);
                if (pp.luminosity() <= Lmin) break;
                simulatepropagation(&pp,&dsp);
                peeloffscattering(&pp);
                simulatescattering(&pp);
            }
        }
        logprogress("dust emission", ell, _Npp);
    }
}

////////////////////////////////////////////////////////////////////

void PanMonteCarloSimulation::initprogress()
{
    _Ndone = 0;
}

////////////////////////////////////////////////////////////////////

void PanMonteCarloSimulation::logprogress(QString phase, int ell, qint64 index)
{
    // update progress counter (conversion to double may loose some precision)
    _Ndone[ell] = index;

    // only log when the total number of photon packages is nonzero (to avoid messages with NaN's)
    if (_Npp > 0)
    {
        // calculate completion percentage
        // note: since we don't do this in a critical section, the result may be slightly incorrect
        double completed = _Ndone.sum() * 100. / _Npp / _Ndone.size();

        // log message
        QString prefix = (index == _Npp)
                         ? "Completed " + QString::number(_Npp) + " " + phase + " photon packages"
                         : "Launching " + phase + " photon package " + QString::number(index);
        _log->info(prefix + " for λ = " + QString::number(_units->owavelength(_lambdagrid->lambda(ell)), 'g', 4) + " "
                   + _units->uwavelength() + " (" + QString::number(completed,'f',0) + "%)...");
    }
}

////////////////////////////////////////////////////////////////////
