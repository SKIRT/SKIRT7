/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "FatalError.hpp"
#include "LinSpheDustGridStructure.hpp"
#include "NR.hpp"

using namespace std;

//////////////////////////////////////////////////////////////////////

LinSpheDustGridStructure::LinSpheDustGridStructure()
{
}

//////////////////////////////////////////////////////////////////////

void LinSpheDustGridStructure::setupSelfBefore()
{
    SpheDustGridStructure::setupSelfBefore();

    // verify property values
    if (_rmax <= 0) throw FATALERROR("the outer radius should be positive");
    if (_Nr <= 0) throw FATALERROR("the number of radial grid points should be positive");

    // grid distribution in r
    NR::lingrid(_rv, 0., _rmax, _Nr);

    // the total number of cells
    _Ncells = _Nr;
}

////////////////////////////////////////////////////////////////////

void LinSpheDustGridStructure::setExtent(double value)
{
    _rmax = value;
}

////////////////////////////////////////////////////////////////////

double LinSpheDustGridStructure::extent() const
{
    return _rmax;
}

////////////////////////////////////////////////////////////////////

void LinSpheDustGridStructure::setPoints(int value)
{
    _Nr = value;
}

////////////////////////////////////////////////////////////////////

int LinSpheDustGridStructure::points() const
{
    return _Nr;
}

//////////////////////////////////////////////////////////////////////
