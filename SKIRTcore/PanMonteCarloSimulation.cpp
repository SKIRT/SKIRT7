/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

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

void PanMonteCarloSimulation::setupSelfAfter()
{
    MonteCarloSimulation::setupSelfAfter();

    // properly size the array used to communicate between rundustXXX() and the corresponding parallel loop
    _Ncells = _pds ? _pds->Ncells() : 0;
    if (_pds && _pds->dustemission()) _Labsbolv.resize(_Ncells);
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

void PanMonteCarloSimulation::rundustselfabsorption()
{
    TimeLogger logger(_log, "the dust self-absorption phase");

    const int Ncyclesmax = 100;
    const double epsmax = 0.005;
    Array Labsdusttotv(Ncyclesmax+1);

    for (int cycle=1; cycle<=Ncyclesmax; cycle++)
    {
        TimeLogger logger(_log, "the dust self-absorption cycle " + QString::number(cycle));

        // Construct the dust emission spectra
        _log->info("Calculating dust emission spectra...");
        _pds->calculatedustemission();
        _log->info("Dust emission spectra calculated.");

        // Determine the bolometric luminosity that is absorbed in every cell (and that will hence be re-emitted).
        for (int m=0; m<_Ncells; m++)
            _Labsbolv[m] = _pds->Labs(m);

        // Set the absorbed dust luminosity to zero in all cells
        _pds->rebootLabsdust();

        // Run a simulation
        initprogress("dust self-absorption cycle " + QString::number(cycle));
        Parallel* parallel = find<ParallelFactory>()->parallel();
        parallel->call(this, &PanMonteCarloSimulation::dodustselfabsorptionchunk, _Nchunks*_Nlambda);

        // Update the absorbed luminosity in each cell. Save the total absorbed luminosity in the vector Labstotv.
        Labsdusttotv[cycle] = _pds->Labsdusttot();
        _log->info("The total absorbed stellar luminosity is "
                   + QString::number(_units->obolluminosity(_pds->Labsstellartot())) + " "
                   + _units->ubolluminosity() );
        _log->info("The total absorbed dust luminosity is "
                   + QString::number(_units->obolluminosity(Labsdusttotv[cycle])) + " "
                   + _units->ubolluminosity() );

        // Check the criterion to terminate the self-absorption cycle. We use the criterion that the total
        // absorbed dust luminosity should be changed by less than epsmax compared to the previous cycle.
        double eps = fabs((Labsdusttotv[cycle]-Labsdusttotv[cycle-1])/Labsdusttotv[cycle]);
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

void PanMonteCarloSimulation::dodustselfabsorptionchunk(size_t index)
{
    // Determine the wavelength index for this chunk
    int ell = index % _Nlambda;

    // Determine the luminosity to be emitted at this wavelength index
    Array Lv(_Ncells);
    for (int m=0; m<_Ncells; m++)
    {
        double Labsbol = _Labsbolv[m];
        if (Labsbol>0.0) Lv[m] = Labsbol * _pds->dustluminosity(m,ell);
    }
    double Ltot = Lv.sum();

    // Emit photon packages
    if (Ltot > 0)
    {
        Array Xv;
        NR::cdf(Xv, Lv);

        PhotonPackage pp;
        double L = Ltot / _Npp;
        double Lmin = 1e-4*L;

        quint64 remaining = _chunksize;
        while (remaining > 0)
        {
            quint64 count = qMin(remaining, _logchunksize);
            for (quint64 i=0; i<count; i++)
            {
                double X = _random->uniform();
                int m = NR::locate_clip(Xv,X);
                Position bfr = _pds->randomPositionInCell(m);
                Direction bfk = _random->direction();
                pp.launch(L,ell,bfr,bfk);
                while (true)
                {
                    _pds->fillOpticalDepth(&pp);
                    simulateescapeandabsorption(&pp,true);
                    if (pp.luminosity() <= Lmin) break;
                    simulatepropagation(&pp);
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

void PanMonteCarloSimulation::rundustemission()
{
    TimeLogger logger(_log, "the dust emission phase");

    // Construct the dust emission spectra
    _log->info("Calculating dust emission spectra...");
    _pds->calculatedustemission();
    _log->info("Dust emission spectra calculated.");

    // Determine the bolometric luminosity that is absorbed in every cell (and that will hence be re-emitted).
    for (int m=0; m<_Ncells; m++)
        _Labsbolv[m] = _pds->Labs(m);

    // perform the actual dust emission
    initprogress("dust emission");
    Parallel* parallel = find<ParallelFactory>()->parallel();
    parallel->call(this, &PanMonteCarloSimulation::dodustemissionchunk, _Nchunks*_Nlambda);
}

////////////////////////////////////////////////////////////////////

void PanMonteCarloSimulation::dodustemissionchunk(size_t index)
{
    // Determine the wavelength index for this chunk
    int ell = index % _Nlambda;

    // Determine the luminosity to be emitted at this wavelength index
    Array Lv(_Ncells);
    for (int m=0; m<_Ncells; m++)
    {
        double Labsbol = _Labsbolv[m];
        if (Labsbol>0.0) Lv[m] = Labsbol * _pds->dustluminosity(m,ell);
    }
    double Ltot = Lv.sum();

    // Emit photon packages
    if (Ltot > 0)
    {
        Array Xv;
        NR::cdf(Xv, Lv);

        PhotonPackage pp,ppp;
        double L = Ltot / _Npp;
        double Lmin = 1e-4 * L;

        quint64 remaining = _chunksize;
        while (remaining > 0)
        {
            quint64 count = qMin(remaining, _logchunksize);
            for (quint64 i=0; i<count; i++)
            {
                double X = _random->uniform();
                int m = NR::locate_clip(Xv,X);
                Position bfr = _pds->randomPositionInCell(m);
                Direction bfk = _random->direction();
                pp.launch(L,ell,bfr,bfk);
                peeloffemission(&pp,&ppp);
                while (true)
                {
                    _pds->fillOpticalDepth(&pp);
                    if (_continuousScattering) continuouspeeloffscattering(&pp,&ppp);
                    simulateescapeandabsorption(&pp,false);
                    if (pp.luminosity() <= Lmin) break;
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
