/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include <cmath>
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

    _Rv.resize(_NR+1);
    if (fabs(_Rratio-1.0)<1e-3)
        for (int i=0; i<=_NR; i++)
            _Rv[i] = i*_Rmax/_NR;
    else
    {
        double q = pow(_Rratio,1.0/(_NR-1.0));
        for (int i=0; i<=_NR; ++i)
            _Rv[i] = (1.0-pow(q,i))/(1.0-pow(q,_NR)) * _Rmax;
    }

    // setup grid distribution in z

    _zv.resize(_Nz+1);
    if (fabs(_zratio-1.0)<1e-3)
        for (int k=0; k<=_Nz; k++)
            _zv[k] = -_zmax + 2.0*k*_zmax/_Nz;
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
