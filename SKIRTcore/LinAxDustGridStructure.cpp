/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include "LinAxDustGridStructure.hpp"
#include "FatalError.hpp"
#include "NR.hpp"

using namespace std;

//////////////////////////////////////////////////////////////////////

LinAxDustGridStructure::LinAxDustGridStructure()
{
}

////////////////////////////////////////////////////////////////////

void LinAxDustGridStructure::setupSelfBefore()
{
    AxDustGridStructure::setupSelfBefore();

    // verify property values
    if (_Rmax <= 0) throw FATALERROR("the outer radius in the radial direction should be positive");
    if (_NR <= 0) throw FATALERROR("the number of radial grid points should be positive");
    if (_zmax <= 0) throw FATALERROR("the outer radius in the axial direction should be positive");
    if (_Nz <= 0) throw FATALERROR("the number of axial grid points should be positive");

    // grid distribution in R
    NR::lingrid(_Rv, 0., _Rmax, _NR);

    // grid distribution in z
    NR::lingrid(_zv, -_zmax, _zmax, _Nz);

    // the total number of cells
    _Ncells = _NR*_Nz;
}

////////////////////////////////////////////////////////////////////

void LinAxDustGridStructure::setRadialExtent(double value)
{
    _Rmax = value;
}

////////////////////////////////////////////////////////////////////

double LinAxDustGridStructure::radialExtent() const
{
    return _Rmax;
}

////////////////////////////////////////////////////////////////////

void LinAxDustGridStructure::setRadialPoints(int value)
{
    _NR = value;
}

////////////////////////////////////////////////////////////////////

int LinAxDustGridStructure::radialPoints() const
{
    return _NR;
}

////////////////////////////////////////////////////////////////////

void LinAxDustGridStructure::setAxialExtent(double value)
{
    _zmax = value;
    _zmin = -value;
}

////////////////////////////////////////////////////////////////////

double LinAxDustGridStructure::axialExtent() const
{
    return _zmax;
}

////////////////////////////////////////////////////////////////////

void LinAxDustGridStructure::setAxialPoints(int value)
{
    _Nz = value;
}

////////////////////////////////////////////////////////////////////

int LinAxDustGridStructure::axialPoints() const
{
    return _Nz;
}

//////////////////////////////////////////////////////////////////////
