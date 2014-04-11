/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include <cmath>
#include "FatalError.hpp"
#include "LogPowAxDustGridStructure.hpp"

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
    _Rv.resize(_NR+1);
    double logRmin = log10(_Rmin);
    double logRmax = log10(_Rmax);
    _Rv[0] = 0.0;
    for (int i=0; i<_NR; i++)
    {
        double logR = logRmin + i*(logRmax-logRmin)/(_NR-1);
        _Rv[i+1] = pow(10.0,logR);
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
