/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include "FatalError.hpp"
#include "NR.hpp"
#include "PowCubDustGridStructure.hpp"

using namespace std;

//////////////////////////////////////////////////////////////////////

PowCubDustGridStructure::PowCubDustGridStructure()
    : _xratio(0), _yratio(0), _zratio(0)
{
}

////////////////////////////////////////////////////////////////////

void PowCubDustGridStructure::setupSelfBefore()
{
    CubDustGridStructure::setupSelfBefore();

    // verify property values
    if (_xmax <= 0) throw FATALERROR("the outer radius in the x direction should be positive");
    if (_xratio <= 0)
        throw FATALERROR("the ratio of the inner- and outermost bin widths in the x direction should be positive");
    if (_Nx <= 0) throw FATALERROR("the number of grid points in the x direction should be positive");
    if (_ymax <= 0) throw FATALERROR("the outer radius in the y direction should be positive");
    if (_yratio <= 0)
        throw FATALERROR("the ratio of the inner- and outermost bin widths in the y direction should be positive");
    if (_Ny <= 0) throw FATALERROR("the number of grid points in the y direction should be positive");
    if (_zmax <= 0) throw FATALERROR("the outer radius in the z direction should be positive");
    if (_zratio <= 0)
        throw FATALERROR("the ratio of the inner- and outermost bin widths in the z direction should be positive");
    if (_Nz <= 0) throw FATALERROR("the number of grid points in the z direction should be positive");

    // grid distributions
    NR::sympowgrid(_xv, _xmax, _Nx, _xratio);
    NR::sympowgrid(_yv, _ymax, _Ny, _yratio);
    NR::sympowgrid(_zv, _zmax, _Nz, _zratio);

    // the total number of cells
    _Ncells = _Nx*_Ny*_Nz;
}

////////////////////////////////////////////////////////////////////

void PowCubDustGridStructure::setExtentX(double value)
{
    _xmax = value;
    _xmin = -value;
}

////////////////////////////////////////////////////////////////////

double PowCubDustGridStructure::extentX() const
{
    return _xmax;
}

////////////////////////////////////////////////////////////////////

void PowCubDustGridStructure::setRatioX(double value)
{
    _xratio = value;
}

////////////////////////////////////////////////////////////////////

double PowCubDustGridStructure::ratioX() const
{
    return _xratio;
}

////////////////////////////////////////////////////////////////////

void PowCubDustGridStructure::setPointsX(int value)
{
    _Nx = value;
}

////////////////////////////////////////////////////////////////////

int PowCubDustGridStructure::pointsX() const
{
    return _Nx;
}

////////////////////////////////////////////////////////////////////

void PowCubDustGridStructure::setExtentY(double value)
{
    _ymax = value;
    _ymin = -value;
}

////////////////////////////////////////////////////////////////////

double PowCubDustGridStructure::extentY() const
{
    return _ymax;
}

////////////////////////////////////////////////////////////////////

void PowCubDustGridStructure::setRatioY(double value)
{
    _yratio = value;
}

////////////////////////////////////////////////////////////////////

double PowCubDustGridStructure::ratioY() const
{
    return _yratio;
}

////////////////////////////////////////////////////////////////////

void PowCubDustGridStructure::setPointsY(int value)
{
    _Ny = value;
}

////////////////////////////////////////////////////////////////////

int PowCubDustGridStructure::pointsY() const
{
    return _Ny;
}

////////////////////////////////////////////////////////////////////

void PowCubDustGridStructure::setExtentZ(double value)
{
    _zmax = value;
    _zmin = -value;
}

////////////////////////////////////////////////////////////////////

double PowCubDustGridStructure::extentZ() const
{
    return _zmax;
}

////////////////////////////////////////////////////////////////////

void PowCubDustGridStructure::setRatioZ(double value)
{
    _zratio = value;
}

////////////////////////////////////////////////////////////////////

double PowCubDustGridStructure::ratioZ() const
{
    return _zratio;
}

////////////////////////////////////////////////////////////////////

void PowCubDustGridStructure::setPointsZ(int value)
{
    _Nz = value;
}

////////////////////////////////////////////////////////////////////

int PowCubDustGridStructure::pointsZ() const
{
    return _Nz;
}

//////////////////////////////////////////////////////////////////////
