/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "FatalError.hpp"
#include "NR.hpp"
#include "GenLinCubDustGridStructure.hpp"

using namespace std;

//////////////////////////////////////////////////////////////////////

GenLinCubDustGridStructure::GenLinCubDustGridStructure()
{
}

////////////////////////////////////////////////////////////////////

void GenLinCubDustGridStructure::setupSelfBefore()
{
    CubDustGridStructure::setupSelfBefore();

    // verify property values
    if (_xmax <= _xmin) throw FATALERROR("the maximum radius in the x direction should be larger than the minimum radius");
    if (_Nx <= 0) throw FATALERROR("the number of grid points in the x direction should be positive");
    if (_ymax <= _ymin) throw FATALERROR("the maximum radius in the y direction should be larger than the minimum radius");
    if (_Ny <= 0) throw FATALERROR("the number of grid points in the y direction should be positive");
    if (_zmax <= _zmin) throw FATALERROR("the maximum radius in the z direction should be larger than the minimum radius");
    if (_Nz <= 0) throw FATALERROR("the number of grid points in the z direction should be positive");

    // grid distributions
    NR::lingrid(_xv, _xmin, _xmax, _Nx);
    NR::lingrid(_yv, _ymin, _ymax, _Ny);
    NR::lingrid(_zv, _zmin, _zmax, _Nz);

    // the total number of cells
    _Ncells = _Nx*_Ny*_Nz;
}

////////////////////////////////////////////////////////////////////

void GenLinCubDustGridStructure::setMinX(double value)
{
    _xmin = value;
}

////////////////////////////////////////////////////////////////////

double GenLinCubDustGridStructure::minX() const
{
    return _xmin;
}

////////////////////////////////////////////////////////////////////

void GenLinCubDustGridStructure::setMaxX(double value)
{
    _xmax = value;
}

////////////////////////////////////////////////////////////////////

double GenLinCubDustGridStructure::maxX() const
{
    return _xmax;
}

////////////////////////////////////////////////////////////////////

void GenLinCubDustGridStructure::setPointsX(int value)
{
    _Nx = value;
}

////////////////////////////////////////////////////////////////////

int GenLinCubDustGridStructure::pointsX() const
{
    return _Nx;
}

////////////////////////////////////////////////////////////////////

void GenLinCubDustGridStructure::setMinY(double value)
{
    _ymin = value;
}

////////////////////////////////////////////////////////////////////

double GenLinCubDustGridStructure::minY() const
{
    return _ymin;
}

////////////////////////////////////////////////////////////////////

void GenLinCubDustGridStructure::setMaxY(double value)
{
    _ymax = value;
}

////////////////////////////////////////////////////////////////////

double GenLinCubDustGridStructure::maxY() const
{
    return _ymax;
}

////////////////////////////////////////////////////////////////////

void GenLinCubDustGridStructure::setPointsY(int value)
{
    _Ny = value;
}

////////////////////////////////////////////////////////////////////

int GenLinCubDustGridStructure::pointsY() const
{
    return _Ny;
}

////////////////////////////////////////////////////////////////////

void GenLinCubDustGridStructure::setMinZ(double value)
{
    _zmin = value;
}

////////////////////////////////////////////////////////////////////

double GenLinCubDustGridStructure::minZ() const
{
    return _zmin;
}

////////////////////////////////////////////////////////////////////

void GenLinCubDustGridStructure::setMaxZ(double value)
{
    _zmax = value;
}

////////////////////////////////////////////////////////////////////

double GenLinCubDustGridStructure::maxZ() const
{
    return _zmax;
}

////////////////////////////////////////////////////////////////////

void GenLinCubDustGridStructure::setPointsZ(int value)
{
    _Nz = value;
}

////////////////////////////////////////////////////////////////////

int GenLinCubDustGridStructure::pointsZ() const
{
    return _Nz;
}

//////////////////////////////////////////////////////////////////////
