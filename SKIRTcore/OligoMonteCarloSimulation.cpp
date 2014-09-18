/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "OligoDustSystem.hpp"
#include "OligoMonteCarloSimulation.hpp"
#include "OligoWavelengthGrid.hpp"
#include "StellarSystem.hpp"

using namespace std;

////////////////////////////////////////////////////////////////////

OligoMonteCarloSimulation::OligoMonteCarloSimulation()
{
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

void OligoMonteCarloSimulation::runSelf()
{
    runstellaremission();
    write();
}

////////////////////////////////////////////////////////////////////
