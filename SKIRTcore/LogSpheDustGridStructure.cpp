/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include <cmath>
#include "FatalError.hpp"
#include "LogSpheDustGridStructure.hpp"
#include "NR.hpp"

using namespace std;

//////////////////////////////////////////////////////////////////////

LogSpheDustGridStructure::LogSpheDustGridStructure()
    : _rmin(0)
{
}

//////////////////////////////////////////////////////////////////////

void LogSpheDustGridStructure::setupSelfBefore()
{
    SpheDustGridStructure::setupSelfBefore();

    // verify property values
    if (_rmin <= 0) throw FATALERROR("the inner radius should be positive");
    if (_rmax <= _rmin) throw FATALERROR("the outer radius should be larger than the inner radius");
    if (_Nr <= 0) throw FATALERROR("the number of radial grid points should be positive");

    // grid distribution in r
    NR::zerologgrid(_rv, _rmin, _rmax, _Nr);

    // the total number of cells
    _Ncells = _Nr;
}

////////////////////////////////////////////////////////////////////

void LogSpheDustGridStructure::setInnerExtent(double value)
{
    _rmin = value;
}

////////////////////////////////////////////////////////////////////

double LogSpheDustGridStructure::innerExtent() const
{
    return _rmin;
}

////////////////////////////////////////////////////////////////////

void LogSpheDustGridStructure::setOuterExtent(double value)
{
    _rmax = value;
}

////////////////////////////////////////////////////////////////////

double LogSpheDustGridStructure::outerExtent() const
{
    return _rmax;
}

////////////////////////////////////////////////////////////////////

void LogSpheDustGridStructure::setPoints(int value)
{
    _Nr = value;
}

////////////////////////////////////////////////////////////////////

int LogSpheDustGridStructure::points() const
{
    return _Nr;
}

//////////////////////////////////////////////////////////////////////
