/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include <cmath>
#include "FatalError.hpp"
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

    // grid distribution in x
    _xv.resize(_Nx+1);
    if (fabs(_xratio-1.0)<1e-3)
    {
        for (int i=0; i<=_Nx; i++)
            _xv[i] = -_xmax + 2.0*i*_xmax/_Nx;
    }
    else if (_Nx%2==0)
    {
        int M = _Nx/2;
        double q = pow(_xratio,1.0/(M-1.0));
        _xv[M] = 0.0;
        for (int i=1; i<=M; ++i)
        {
            _xv[M+i] = (1.0-pow(q,i)) / (1.0-pow(q,M)) * _xmax;
            _xv[M-i] = -_xv[M+i];
        }
    }
    else
    {
        int M = (_Nx+1)/2;
        double q = pow(_xratio,1.0/(M-1.0));
        for (int i=1; i<=M; ++i)
        {
            _xv[M-1+i] = (0.5+0.5*q-pow(q,i)) / (0.5+0.5*q-pow(q,M)) * _xmax;
            _xv[M-i] = -_xv[M-1+i];
        }
    }

    // grid distribution in y
    _yv.resize(_Ny+1);
    if (fabs(_yratio-1.0)<1e-3)
    {
        for (int j=0; j<=_Ny; j++)
            _yv[j] = -_ymax + 2.0*j*_ymax/_Ny;
    }
    else if (_Ny%2==0)
    {
        int M = _Ny/2;
        double q = pow(_yratio,1.0/(M-1.0));
        _yv[M] = 0.0;
        for (int j=1; j<=M; ++j)
        {
            _yv[M+j] = (1.0-pow(q,j)) / (1.0-pow(q,M)) * _ymax;
            _yv[M-j] = -_yv[M+j];
        }
    }
    else
    {
        int M = (_Ny+1)/2;
        double q = pow(_yratio,1.0/(M-1.0));
        for (int j=1; j<=M; ++j)
        {
            _yv[M-1+j] = (0.5+0.5*q-pow(q,j)) / (0.5+0.5*q-pow(q,M)) * _ymax;
            _yv[M-j] = -_yv[M-1+j];
        }
    }

    // grid distribution in z
    _zv.resize(_Nz+1);
    if (fabs(_zratio-1.0)<1e-3)
    {
        for (int k=0; k<=_Nz; k++)
            _zv[k] = -_zmax + 2.0*k*_zmax/_Nz;
    }
    else if (_Nz%2==0)
    {
        int M = _Nz/2;
        double q = pow(_zratio,1.0/(M-1.0));
        _zv[M] = 0.0;
        for (int k=1; k<=M; ++k)
        {
            _zv[M+k] = (1.0-pow(q,k)) / (1.0-pow(q,M)) * _zmax;
            _zv[M-k] = -_zv[M+k];
        }
    }
    else
    {
        int M = (_Nz+1)/2;
        double q = pow(_zratio,1.0/(M-1.0));
        for (int k=1; k<=M; ++k)
        {
            _zv[M-1+k] = (0.5+0.5*q-pow(q,k)) / (0.5+0.5*q-pow(q,M)) * _zmax;
            _zv[M-k] = -_zv[M-1+k];
        }
    }

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
