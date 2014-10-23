/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "FatalError.hpp"
#include "Random.hpp"
#include "SersicFunction.hpp"
#include "SersicGeometry.hpp"
#include "SpecialFunctions.hpp"

using namespace std;

//////////////////////////////////////////////////////////////////////

SersicGeometry::SersicGeometry()
    : _n(0), _reff(0), _rho0(0), _b(0), _sersicfunction(0)
{
}

//////////////////////////////////////////////////////////////////////

SersicGeometry::~SersicGeometry()
{
    delete _sersicfunction;
}

//////////////////////////////////////////////////////////////////////

void SersicGeometry::setupSelfBefore()
{
    SpheGeometry::setupSelfBefore();

    // verify property values
    if (_n <= 0.5 || _n > 10) throw FATALERROR("the Sersic index n should be between 0.5 and 10");
    if (_reff <= 0) throw FATALERROR("the effective radius should be positive");

    // calculate cached values
    _rho0 = 1.0/(_reff*_reff*_reff);
    _b = 2.0*_n - 1.0/3.0 + 4.0/405.0/_n + 46.0/25515.0/(_n*_n) + 131.0/1148175.0/(_n*_n*_n);
    _sersicfunction = new SersicFunction(_n);
}

////////////////////////////////////////////////////////////////////

void SersicGeometry::setIndex(double value)
{
    _n = value;
}

////////////////////////////////////////////////////////////////////

double SersicGeometry::index() const
{
    return _n;
}

////////////////////////////////////////////////////////////////////

void SersicGeometry::setRadius(double value)
{
    _reff = value;
}

////////////////////////////////////////////////////////////////////

double SersicGeometry::radius() const
{
    return _reff;
}

//////////////////////////////////////////////////////////////////////

double
SersicGeometry::density(double r)
const
{
    double s = r/_reff;
    return _rho0 * (*_sersicfunction)(s);
}

//////////////////////////////////////////////////////////////////////

double
SersicGeometry::randomradius()
const
{
    double X = _random->uniform();
    return _reff * _sersicfunction->inversemass(X);
}

//////////////////////////////////////////////////////////////////////

double
SersicGeometry::Sigmar()
const
{
    return 1.0/(_reff*_reff) * pow(_b,2.0*_n)
            / (2.0*M_PI*SpecialFunctions::gamma(2.0*_n+1.0));
}

//////////////////////////////////////////////////////////////////////
