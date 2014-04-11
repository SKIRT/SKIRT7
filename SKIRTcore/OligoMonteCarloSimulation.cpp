/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include "OligoDustSystem.hpp"
#include "DustSystemPath.hpp"
#include "Log.hpp"
#include "OligoMonteCarloSimulation.hpp"
#include "Parallel.hpp"
#include "ParallelFactory.hpp"
#include "PhotonPackage.hpp"
#include "StellarSystem.hpp"
#include "TimeLogger.hpp"
#include "Units.hpp"
#include "OligoWavelengthGrid.hpp"

using namespace std;

////////////////////////////////////////////////////////////////////

OligoMonteCarloSimulation::OligoMonteCarloSimulation()
{
}

////////////////////////////////////////////////////////////////////

void OligoMonteCarloSimulation::setupSelfBefore()
{
    MonteCarloSimulation::setupSelfBefore();

    // generate at most 100 messages per wavelength
    _Nlog = _Nchunks/100;
    if (_Nlog < 1) _Nlog = 1;
}

////////////////////////////////////////////////////////////////////

void OligoMonteCarloSimulation::setWavelengthGrid(OligoWavelengthGrid* value)
{
    if (_lambdagrid) delete _lambdagrid;
    _lambdagrid = value;
    if (_lambdagrid) _lambdagrid->setParent(this);
}

////////////////////////////////////////////////////////////////////

OligoWavelengthGrid* OligoMonteCarloSimulation::wavelengthGrid() const
{
    return dynamic_cast<OligoWavelengthGrid*>(_lambdagrid);
}

////////////////////////////////////////////////////////////////////

void OligoMonteCarloSimulation::setStellarSystem(StellarSystem* value)
{
    if (_ss) delete _ss;
    _ss = value;
    if (_ss) _ss->setParent(this);
}

////////////////////////////////////////////////////////////////////

StellarSystem* OligoMonteCarloSimulation::stellarSystem() const
{
    return _ss;
}

////////////////////////////////////////////////////////////////////

void OligoMonteCarloSimulation::setDustSystem(OligoDustSystem* value)
{
    if (_ds) delete _ds;
    _ds = value;
    if (_ds) _ds->setParent(this);
}

////////////////////////////////////////////////////////////////////

OligoDustSystem* OligoMonteCarloSimulation::dustSystem() const
{
    return dynamic_cast<OligoDustSystem*>(_ds);
}

////////////////////////////////////////////////////////////////////

bool OligoMonteCarloSimulation::parallelPhotonsOfSameWaveLength() const
{
    return true;
}

////////////////////////////////////////////////////////////////////

void OligoMonteCarloSimulation::runSelf()
{
    runstellaremission();
    write();
}

////////////////////////////////////////////////////////////////////

void
OligoMonteCarloSimulation::runstellaremission()
{
    TimeLogger logger(_log, "the stellar emission phase");
    Parallel* parallel = find<ParallelFactory>()->parallel();

    int Nlambda = _lambdagrid->Nlambda();
    for (_ell=0; _ell < Nlambda; _ell++)
    {
        TimeLogger logger(_log, "stellar emission for λ = " +
                          QString::number(_units->owavelength(_lambdagrid->lambda(_ell))) + " " +
                          _units->uwavelength());

        _L = _ss->luminosity(_ell)/_Nchunks/CHUNKSIZE;
        _Lmin = 1e-4*_L;
        if (_L>0.0) parallel->call(this, &OligoMonteCarloSimulation::dostellaremissionchunk, _Nchunks);
    }
}

////////////////////////////////////////////////////////////////////

void
OligoMonteCarloSimulation::dostellaremissionchunk(int index)
{
    if (index%_Nlog == 0)
        _log->info("Launching photon package " + QString::number(static_cast<double>(index)*CHUNKSIZE, 'f', 0)
                   + " (" + QString::number(100*index/_Nchunks) + "%)...");

    for (int i=0; i<CHUNKSIZE; i++)
    {
        PhotonPackage pp;
        DustSystemPath dsp;
        _ss->launch(&pp,_ell,_L);
        peeloffemission(&pp);
        if (_ds)
        {
            fillDustSystemPath(&pp,&dsp);
            simulateescapeandabsorption(&pp,&dsp,false);
            while (pp.luminosity() > _Lmin)
            {
                simulatepropagation(&pp,&dsp);
                peeloffscattering(&pp);
                simulatescattering(&pp);
                fillDustSystemPath(&pp,&dsp);
                simulateescapeandabsorption(&pp,&dsp,false);
            }
        }
    }
}

////////////////////////////////////////////////////////////////////
