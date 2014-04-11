/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include <cmath>
#include "FatalError.hpp"
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

    // grid distribution in x
    _xv.resize(_Nx+1);
    for (int i=0; i<=_Nx; ++i)
        _xv[i] = _xmin + i*(_xmax-_xmin)/_Nx;

    // grid distribution in y
    _yv.resize(_Ny+1);
    for (int j=0; j<=_Ny; ++j)
        _yv[j] = _ymin + j*(_ymax-_ymin)/_Ny;

    // grid distribution in z
    _zv.resize(_Nz+1);
    for (int k=0; k<=_Nz; ++k)
        _zv[k] = _zmin + k*(_zmax-_zmin)/_Nz;

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

double GenLinCubDustGridStructure::xmax() const
{
    return max(_xmax,-_xmin);
}

//////////////////////////////////////////////////////////////////////

double GenLinCubDustGridStructure::ymax() const
{
    return max(_ymax,-_ymin);
}

//////////////////////////////////////////////////////////////////////

double GenLinCubDustGridStructure::zmax() const
{
    return max(_zmax,-_zmin);
}

//////////////////////////////////////////////////////////////////////
