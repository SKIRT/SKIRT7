/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include <cmath>
#include "FatalError.hpp"
#include "LogLinAxSpheDustGridStructure.hpp"
#include "NR.hpp"

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
    NR::zerologgrid(_rv, _rmin, _rmax, _Nr);

    // grid distribution in theta (an even number of bins ensures that the xy-plane is included)
    NR::lingrid(_thetav, 0., M_PI, _Ntheta);

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
