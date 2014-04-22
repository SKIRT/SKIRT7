/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include <cmath>
#include "NR.hpp"
#include "PowAxDustGridStructure.hpp"
#include "FatalError.hpp"

using namespace std;

//////////////////////////////////////////////////////////////////////

PowAxDustGridStructure::PowAxDustGridStructure()
    : _Rratio(0), _zratio(0)
{
}

//////////////////////////////////////////////////////////////////////

void PowAxDustGridStructure::setupSelfBefore()
{
    AxDustGridStructure::setupSelfBefore();

    // verify property values
    if (_NR <= 0) throw FATALERROR("NR should be positive");
    if (_Rmax <= 0) throw FATALERROR("Rmax should be positive");
    if (_Rratio <= 0) throw FATALERROR("Rratio should be positive");
    if (_Nz <= 0) throw FATALERROR("Nz should be positive");
    if (_zmax <= 0) throw FATALERROR("zmax should be positive");
    if (_zratio <= 0) throw FATALERROR("zratio should be positive");

    // setup grid distribution in R
    NR::powgrid(_Rv, -0., _Rmax, _NR, _Rratio);

    // setup grid distribution in z
    NR::sympowgrid(_zv, _zmax, _Nz, _zratio);

    // the total number of cells
    _Ncells = _NR*_Nz;
}

//////////////////////////////////////////////////////////////////////

void PowAxDustGridStructure::setRadialExtent(double value)
{
    _Rmax = value;
}

//////////////////////////////////////////////////////////////////////

double PowAxDustGridStructure::radialExtent() const
{
    return _Rmax;
}

//////////////////////////////////////////////////////////////////////

void PowAxDustGridStructure::setRadialRatio(double value)
{
    _Rratio = value;
}

//////////////////////////////////////////////////////////////////////

double PowAxDustGridStructure::radialRatio() const
{
    return _Rratio;
}

//////////////////////////////////////////////////////////////////////

void PowAxDustGridStructure::setRadialPoints(int value)
{
    _NR = value;
}

//////////////////////////////////////////////////////////////////////

int PowAxDustGridStructure::radialPoints() const
{
    return _NR;
}

//////////////////////////////////////////////////////////////////////

void PowAxDustGridStructure::setAxialExtent(double value)
{
    _zmax = value;
    _zmin = - value;
}

//////////////////////////////////////////////////////////////////////

double PowAxDustGridStructure::axialExtent() const
{
    return _zmax;
}

//////////////////////////////////////////////////////////////////////

void PowAxDustGridStructure::setAxialRatio(double value)
{
    _zratio = value;
}

//////////////////////////////////////////////////////////////////////

double PowAxDustGridStructure::axialRatio() const
{
    return _zratio;
}

//////////////////////////////////////////////////////////////////////

void PowAxDustGridStructure::setAxialPoints(int value)
{
    _Nz = value;
}

//////////////////////////////////////////////////////////////////////

int PowAxDustGridStructure::axialPoints() const
{
    return _Nz;
}

//////////////////////////////////////////////////////////////////////
