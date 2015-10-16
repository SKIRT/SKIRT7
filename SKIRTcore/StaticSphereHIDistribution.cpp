/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "FatalError.hpp"
#include "Random.hpp"
#include "StaticSphereHIDistribution.hpp"

using namespace std;

//////////////////////////////////////////////////////////////////////

StaticSphereHIDistribution::StaticSphereHIDistribution()
    : _R(0), _T(0), _M(0), _rho(0)
{
}

//////////////////////////////////////////////////////////////////////

void StaticSphereHIDistribution::setupSelfBefore()
{
    AnalyticalHIDistribution::setupSelfBefore();

    // verify property values
    if (_R <= 0) throw FATALERROR("the radius should be positive");
    if (_T <= 0) throw FATALERROR("the gas temperature should be positive");
    if (_M <= 0) throw FATALERROR("the neutral hydrogen gas mass should be positive");

    // calculate cached values
    _rho = 0.75/M_PI*_M/(_R*_R*_R);
    _random = find<Random>();
}

////////////////////////////////////////////////////////////////////

void StaticSphereHIDistribution::setRadius(double value)
{
    _R = value;
}

////////////////////////////////////////////////////////////////////

double StaticSphereHIDistribution::radius() const
{
    return _R;
}

////////////////////////////////////////////////////////////////////

void StaticSphereHIDistribution::setTemperature(double value)
{
    _T = value;
}

////////////////////////////////////////////////////////////////////

double StaticSphereHIDistribution::temperature() const
{
    return _T;
}

////////////////////////////////////////////////////////////////////

void StaticSphereHIDistribution::setMass(double value)
{
    _M = value;
}

////////////////////////////////////////////////////////////////////

double StaticSphereHIDistribution::mass() const
{
    return _M;
}

////////////////////////////////////////////////////////////////////

int StaticSphereHIDistribution::dimension() const
{
    return 1;
}

////////////////////////////////////////////////////////////////////

double StaticSphereHIDistribution::density(Position bfr) const
{
    return (bfr.radius()<_R) ? _rho : 0.0;
}

////////////////////////////////////////////////////////////////////

double StaticSphereHIDistribution::gastemperature(Position bfr) const
{
    return (bfr.radius()<_R) ? _T : 0.0;
}

////////////////////////////////////////////////////////////////////

Vec StaticSphereHIDistribution::bulkvelocity(Position /*bfr*/) const
{
    return Vec();
}

////////////////////////////////////////////////////////////////////

Position StaticSphereHIDistribution::generatePosition() const
{
    double r = cbrt(_random->uniform());
    Direction bfk = _random->direction();
    return Position(r,bfk);
}

//////////////////////////////////////////////////////////////////////

