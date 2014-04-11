/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include <cmath>
#include "FatalError.hpp"
#include "LogLinAxSpheDustGridStructure.hpp"

using namespace std;

//////////////////////////////////////////////////////////////////////

LogLinAxSpheDustGridStructure::LogLinAxSpheDustGridStructure()
    : _rmin(0)
{
}

////////////////////////////////////////////////////////////////////

void LogLinAxSpheDustGridStructure::setupSelfBefore()
{
    AxSpheDustGridStructure::setupSelfBefore();

    // verify property values
    if (_rmin <= 0) throw FATALERROR("the inner radius in the radial direction should be positive");
    if (_rmax <= _rmin)
        throw FATALERROR("the outer radius in the radial direction should be larger than the inner radius");
    if (_Nr <= 0) throw FATALERROR("the number of radial grid points should be positive");
    if (_Ntheta <= 0) throw FATALERROR("the number of angular grid points should be positive");
    if (_Ntheta%2 != 0) throw FATALERROR("the number of angular grid points should be even");

    // grid distribution in R
    _rv.resize(_Nr+1);
    double logrmin = log10(_rmin);
    double logrmax = log10(_rmax);
    _rv[0] = 0.0;
    for (int i=0; i<_Nr; i++)
    {
        double logr = logrmin + i*(logrmax-logrmin)/(_Nr-1);
        _rv[i+1] = pow(10.0,logr);
    }

    // grid distribution in theta (an even number of grid cells ensures that the xy-plane is included)
    _thetav.resize(_Ntheta+1);
    for (int k=0; k<=_Ntheta; k++)
        _thetav[k] = k*M_PI/_Ntheta;

    // the total number of cells
    _Ncells = _Nr*_Ntheta;
}

////////////////////////////////////////////////////////////////////

void LogLinAxSpheDustGridStructure::setRadialInnerExtent(double value)
{
    _rmin = value;
}

////////////////////////////////////////////////////////////////////

double LogLinAxSpheDustGridStructure::radialInnerExtent() const
{
    return _rmin;
}

////////////////////////////////////////////////////////////////////

void LogLinAxSpheDustGridStructure::setRadialOuterExtent(double value)
{
    _rmax = value;
}

////////////////////////////////////////////////////////////////////

double LogLinAxSpheDustGridStructure::radialOuterExtent() const
{
    return _rmax;
}

////////////////////////////////////////////////////////////////////

void LogLinAxSpheDustGridStructure::setRadialPoints(int value)
{
    _Nr = value;
}

////////////////////////////////////////////////////////////////////

int LogLinAxSpheDustGridStructure::radialPoints() const
{
    return _Nr;
}

////////////////////////////////////////////////////////////////////

void LogLinAxSpheDustGridStructure::setAngularPoints(int value)
{
    _Ntheta = value;
}

////////////////////////////////////////////////////////////////////

int LogLinAxSpheDustGridStructure::angularPoints() const
{
    return _Ntheta;
}

//////////////////////////////////////////////////////////////////////
