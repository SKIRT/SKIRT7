/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include <cmath>
#include "FatalError.hpp"
#include "LogLinAxDustGridStructure.hpp"
#include "NR.hpp"

using namespace std;

//////////////////////////////////////////////////////////////////////

LogLinAxDustGridStructure::LogLinAxDustGridStructure()
    : _Rmin(0)
{
}

////////////////////////////////////////////////////////////////////

void LogLinAxDustGridStructure::setupSelfBefore()
{
    AxDustGridStructure::setupSelfBefore();

    // verify property values
    if (_Rmin <= 0) throw FATALERROR("the inner radius in the radial direction should be positive");
    if (_Rmax <= _Rmin)
        throw FATALERROR("the outer radius in the radial direction should be larger than the inner radius");
    if (_NR <= 0) throw FATALERROR("the number of radial grid points should be positive");
    if (_zmax <= 0) throw FATALERROR("the outer radius in the axial direction should be positive");
    if (_Nz <= 0) throw FATALERROR("the number of axial grid points should be positive");

    // grid distribution in R
    NR::zerologgrid(_Rv, _Rmin, _Rmax, _NR);

    // grid distribution in z
    NR::lingrid(_zv, -_zmax, _zmax, _Nz);

    // the total number of cells
    _Ncells = _NR*_Nz;
}

////////////////////////////////////////////////////////////////////

void LogLinAxDustGridStructure::setRadialInnerExtent(double value)
{
    _Rmin = value;
}

////////////////////////////////////////////////////////////////////

double LogLinAxDustGridStructure::radialInnerExtent() const
{
    return _Rmin;
}

////////////////////////////////////////////////////////////////////

void LogLinAxDustGridStructure::setRadialOuterExtent(double value)
{
    _Rmax = value;
}

////////////////////////////////////////////////////////////////////

double LogLinAxDustGridStructure::radialOuterExtent() const
{
    return _Rmax;
}

////////////////////////////////////////////////////////////////////

void LogLinAxDustGridStructure::setRadialPoints(int value)
{
    _NR = value;
}

////////////////////////////////////////////////////////////////////

int LogLinAxDustGridStructure::radialPoints() const
{
    return _NR;
}

////////////////////////////////////////////////////////////////////

void LogLinAxDustGridStructure::setAxialExtent(double value)
{
    _zmax = value;
    _zmin = -value;
}

////////////////////////////////////////////////////////////////////

double LogLinAxDustGridStructure::axialExtent() const
{
    return _zmax;
}

////////////////////////////////////////////////////////////////////

void LogLinAxDustGridStructure::setAxialPoints(int value)
{
    _Nz = value;
}

////////////////////////////////////////////////////////////////////

int LogLinAxDustGridStructure::axialPoints() const
{
    return _Nz;
}

//////////////////////////////////////////////////////////////////////
