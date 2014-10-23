/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include <cmath>
#include "FatalError.hpp"
#include "LogPowAxDustGridStructure.hpp"
#include "NR.hpp"

using namespace std;

//////////////////////////////////////////////////////////////////////

LogPowAxDustGridStructure::LogPowAxDustGridStructure()
    : _Rmin(0), _zratio(0)
{
}

////////////////////////////////////////////////////////////////////

void LogPowAxDustGridStructure::setupSelfBefore()
{
    AxDustGridStructure::setupSelfBefore();

    // verify property values
    if (_Rmin <= 0) throw FATALERROR("the inner radius in the radial direction should be positive");
    if (_Rmax <= _Rmin)
        throw FATALERROR("the outer radius in the radial direction should larger than the inner radius");
    if (_NR <= 0) throw FATALERROR("the number of radial grid points NR should be positive");
    if (_zmax <= 0) throw FATALERROR("the outer radius in the axial direction zmax should be positive");
    if (_zratio <= 0)
        throw FATALERROR("the ratio of the inner- and outermost bin widths in the axial direction should be positive");
    if (_Nz <= 0) throw FATALERROR("the number of axial grid points Nz should be positive");

    // grid distribution in R
    NR::zerologgrid(_Rv, _Rmin, _Rmax, _NR);

    // grid distribution in z
    NR::sympowgrid(_zv, _zmax, _Nz, _zratio);

    // the total number of cells
    _Ncells = _NR*_Nz;
}

////////////////////////////////////////////////////////////////////

void LogPowAxDustGridStructure::setRadialInnerExtent(double value)
{
    _Rmin = value;
}

////////////////////////////////////////////////////////////////////

double LogPowAxDustGridStructure::radialInnerExtent() const
{
    return _Rmin;
}

////////////////////////////////////////////////////////////////////

void LogPowAxDustGridStructure::setRadialOuterExtent(double value)
{
    _Rmax = value;
}

////////////////////////////////////////////////////////////////////

double LogPowAxDustGridStructure::radialOuterExtent() const
{
    return _Rmax;
}

////////////////////////////////////////////////////////////////////

void LogPowAxDustGridStructure::setRadialPoints(int value)
{
    _NR = value;
}

////////////////////////////////////////////////////////////////////

int LogPowAxDustGridStructure::radialPoints() const
{
    return _NR;
}

////////////////////////////////////////////////////////////////////

void LogPowAxDustGridStructure::setAxialExtent(double value)
{
    _zmax = value;
    _zmin = -value;
}

////////////////////////////////////////////////////////////////////

double LogPowAxDustGridStructure::axialExtent() const
{
    return _zmax;
}

////////////////////////////////////////////////////////////////////

void LogPowAxDustGridStructure::setAxialRatio(double value)
{
    _zratio = value;
}

////////////////////////////////////////////////////////////////////

double LogPowAxDustGridStructure::axialRatio() const
{
    return _zratio;
}

////////////////////////////////////////////////////////////////////

void LogPowAxDustGridStructure::setAxialPoints(int value)
{
    _Nz = value;
}

////////////////////////////////////////////////////////////////////

int LogPowAxDustGridStructure::axialPoints() const
{
    return _Nz;
}

//////////////////////////////////////////////////////////////////////
