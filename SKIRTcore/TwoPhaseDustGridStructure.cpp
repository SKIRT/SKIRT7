/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include "TwoPhaseDustGridStructure.hpp"
#include "FatalError.hpp"
#include "Random.hpp"

using namespace std;

//////////////////////////////////////////////////////////////////////

TwoPhaseDustGridStructure::TwoPhaseDustGridStructure()
    : _contrast(0), _fillingfactor(0)
{
}

////////////////////////////////////////////////////////////////////

void TwoPhaseDustGridStructure::setupSelfBefore()
{
    CubDustGridStructure::setupSelfBefore();

    // verify property values
    if (_xmax <= 0) throw FATALERROR("the outer radius in the x direction should be positive");
    if (_Nx <= 0) throw FATALERROR("the number of grid points in the x direction should be positive");
    if (_ymax <= 0) throw FATALERROR("the outer radius in the y direction should be positive");
    if (_Ny <= 0) throw FATALERROR("the number of grid points in the y direction should be positive");
    if (_zmax <= 0) throw FATALERROR("the outer radius in the z direction should be positive");
    if (_Nz <= 0) throw FATALERROR("the number of grid points in the z direction should be positive");
    if (_fillingfactor <= 0 || _fillingfactor >= 1)
        throw FATALERROR("the volume filling factor of the high-density medium should be between 0 and 1");
    if (_contrast <= 0)
        throw FATALERROR("the density contrast between the high- and low-density medium should be positive");

    // grid distribution in x
    _xv.resize(_Nx+1);
    for (int i=0; i<=_Nx; i++)
        _xv[i] = -_xmax + 2.0*i*_xmax/_Nx;

    // grid distribution in y
    _yv.resize(_Ny+1);
    for (int j=0; j<=_Ny; j++)
        _yv[j] = -_ymax + 2.0*j*_ymax/_Ny;

    // grid distribution in z
    _zv.resize(_Nz+1);
    for (int k=0; k<=_Nz; k++)
        _zv[k] = -_zmax + 2.0*k*_zmax/_Nz;

    // the total number of cells
    _Ncells = _Nx*_Ny*_Nz;

    // construction of the weight matrix
    _weightv.resize(_Ncells);
    double& ff = _fillingfactor;
    double& C = _contrast;
    for (int m=0; m<_Ncells; m++)
    {
        double X = _random->uniform();
        _weightv[m] = (X<ff) ? C/(C*ff+1.0-ff) : 1.0/(C*ff+1.0-ff);
    }
}

////////////////////////////////////////////////////////////////////

void TwoPhaseDustGridStructure::setExtentX(double value)
{
    _xmax = value;
    _xmin = -value;
}

////////////////////////////////////////////////////////////////////

double TwoPhaseDustGridStructure::extentX() const
{
    return _xmax;
}

////////////////////////////////////////////////////////////////////

void TwoPhaseDustGridStructure::setPointsX(int value)
{
    _Nx = value;
}

////////////////////////////////////////////////////////////////////

int TwoPhaseDustGridStructure::pointsX() const
{
    return _Nx;
}

////////////////////////////////////////////////////////////////////

void TwoPhaseDustGridStructure::setExtentY(double value)
{
    _ymax = value;
    _ymin = -value;
}

////////////////////////////////////////////////////////////////////

double TwoPhaseDustGridStructure::extentY() const
{
    return _ymax;
}

////////////////////////////////////////////////////////////////////

void TwoPhaseDustGridStructure::setPointsY(int value)
{
    _Ny = value;
}

////////////////////////////////////////////////////////////////////

int TwoPhaseDustGridStructure::pointsY() const
{
    return _Ny;
}

////////////////////////////////////////////////////////////////////

void TwoPhaseDustGridStructure::setExtentZ(double value)
{
    _zmax = value;
    _zmin = -value;
}

////////////////////////////////////////////////////////////////////

double TwoPhaseDustGridStructure::extentZ() const
{
    return _zmax;
}

////////////////////////////////////////////////////////////////////

void TwoPhaseDustGridStructure::setPointsZ(int value)
{
    _Nz = value;
}

////////////////////////////////////////////////////////////////////

int TwoPhaseDustGridStructure::pointsZ() const
{
    return _Nz;
}

////////////////////////////////////////////////////////////////////

void TwoPhaseDustGridStructure::setFillingFactor(double value)
{
    _fillingfactor = value;
}

////////////////////////////////////////////////////////////////////

double TwoPhaseDustGridStructure::fillingFactor() const
{
    return _fillingfactor;
}

////////////////////////////////////////////////////////////////////

void TwoPhaseDustGridStructure::setContrast(double value)
{
    _contrast = value;
}

////////////////////////////////////////////////////////////////////

double TwoPhaseDustGridStructure::contrast() const
{
    return _contrast;
}

//////////////////////////////////////////////////////////////////////

double
TwoPhaseDustGridStructure::weight(int m)
const
{
    if (m==-1)
        return 0.0;
    else
        return _weightv[m];
}

//////////////////////////////////////////////////////////////////////
