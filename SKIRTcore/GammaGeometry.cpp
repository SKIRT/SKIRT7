/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include <cmath>
#include <limits>
#include "FatalError.hpp"
#include "GammaGeometry.hpp"
#include "Random.hpp"

using namespace std;

//////////////////////////////////////////////////////////////////////

GammaGeometry::GammaGeometry()
    : _b(0), _gamma(0), _rho0(0)
{
}

//////////////////////////////////////////////////////////////////////

void
GammaGeometry::setupSelfBefore()
{
    SpheGeometry::setupSelfBefore();

    // verify property values
    if (_b <= 0) throw FATALERROR("the scale length b should be positive");
    if (_gamma < 0 || _gamma >= 3.0) throw FATALERROR("the central density slope gamma should be between 0 and 3");

    // calculate cached values
    _rho0 = (3.0-_gamma)/(4.0*M_PI)/pow(_b,3);
}

////////////////////////////////////////////////////////////////////

void
GammaGeometry::setScale(double value)
{
    _b = value;
}

////////////////////////////////////////////////////////////////////

double
GammaGeometry::scale()
const
{
    return _b;
}

////////////////////////////////////////////////////////////////////

void
GammaGeometry::setGamma(double value)
{
    _gamma = value;
}

////////////////////////////////////////////////////////////////////

double
GammaGeometry::gamma()
const
{
    return _gamma;
}

////////////////////////////////////////////////////////////////////

double
GammaGeometry::density(double r)
const
{
    return _rho0 * pow(r/_b,-_gamma) * pow(1.0+r/_b,_gamma-4.0);
}

//////////////////////////////////////////////////////////////////////

double
GammaGeometry::randomradius()
const
{
    double X = _random->uniform();
    double t = pow(X,1.0/(3.0-_gamma));
    return _b * t/(1.0-t);
}

//////////////////////////////////////////////////////////////////////

double
GammaGeometry::Sigmar()
const
{
    if (_gamma<1.0)
        return 1.0/(2.0*M_PI*_b*_b*(1.0-_gamma)*(2.0-_gamma));
    else
        return numeric_limits<double>::infinity();

}

//////////////////////////////////////////////////////////////////////
