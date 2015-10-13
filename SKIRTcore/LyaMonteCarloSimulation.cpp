/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "LyaDustSystem.hpp"
#include "LyaMonteCarloSimulation.hpp"
#include "LyaWavelengthGrid.hpp"
#include "StellarSystem.hpp"

using namespace std;

////////////////////////////////////////////////////////////////////

LyaMonteCarloSimulation::LyaMonteCarloSimulation()
{
}

////////////////////////////////////////////////////////////////////

void LyaMonteCarloSimulation::setWavelengthGrid(LyaWavelengthGrid* value)
{
    if (_lambdagrid) delete _lambdagrid;
    _lambdagrid = value;
    if (_lambdagrid) _lambdagrid->setParent(this);
}

////////////////////////////////////////////////////////////////////

LyaWavelengthGrid* LyaMonteCarloSimulation::wavelengthGrid() const
{
    return dynamic_cast<LyaWavelengthGrid*>(_lambdagrid);
}

////////////////////////////////////////////////////////////////////

void LyaMonteCarloSimulation::setStellarSystem(StellarSystem* value)
{
    if (_ss) delete _ss;
    _ss = value;
    if (_ss) _ss->setParent(this);
}

////////////////////////////////////////////////////////////////////

StellarSystem* LyaMonteCarloSimulation::stellarSystem() const
{
    return _ss;
}

////////////////////////////////////////////////////////////////////

void LyaMonteCarloSimulation::setDustSystem(LyaDustSystem* value)
{
    if (_ds) delete _ds;
    _ds = value;
    if (_ds) _ds->setParent(this);
}

////////////////////////////////////////////////////////////////////

LyaDustSystem* LyaMonteCarloSimulation::dustSystem() const
{
    return dynamic_cast<LyaDustSystem*>(_ds);
}

////////////////////////////////////////////////////////////////////

void LyaMonteCarloSimulation::runSelf()
{
    runstellaremission();
    write();
}

////////////////////////////////////////////////////////////////////
