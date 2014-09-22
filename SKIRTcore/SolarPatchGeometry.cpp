/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include <cmath>
#include <limits>
#include <iostream>
#include "FatalError.hpp"
#include "Random.hpp"
#include "SolarPatchGeometry.hpp"

using namespace std;

//////////////////////////////////////////////////////////////////////

SolarPatchGeometry::SolarPatchGeometry()
{
}

//////////////////////////////////////////////////////////////////////

void SolarPatchGeometry::setupSelfBefore()
{
    Geometry::setupSelfBefore();
    // verify property values
    if (_Rmax <= 0) throw FATALERROR("the patch radius Rmax should be positive");
}

//////////////////////////////////////////////////////////////////////

int SolarPatchGeometry::dimension() const
{
    return 2;
}

////////////////////////////////////////////////////////////////////

void SolarPatchGeometry::setRadius(double value)
{
    _Rmax = value;
}

////////////////////////////////////////////////////////////////////

double SolarPatchGeometry::radius() const
{
    return _Rmax;
}

//////////////////////////////////////////////////////////////////////

double SolarPatchGeometry::density(Position bfr) const
{
    if (bfr.height() != 0.0) return 0.0;
    if (bfr.cylradius() < _Rmax) return 0.0;
    return numeric_limits<double>::infinity();
}

//////////////////////////////////////////////////////////////////////

Position SolarPatchGeometry::generatePosition() const
{
    double R = _Rmax * sqrt(_random->uniform());
    double phi = 2.0*M_PI*_random->uniform();
    double z = 0.0;
    return Position(R,phi,z,Position::CYLINDRICAL);
}

//////////////////////////////////////////////////////////////////////

double SolarPatchGeometry::SigmaX() const
{
    return numeric_limits<double>::infinity();
}

//////////////////////////////////////////////////////////////////////

double SolarPatchGeometry::SigmaY() const
{
    return numeric_limits<double>::infinity();
}

//////////////////////////////////////////////////////////////////////

double SolarPatchGeometry::SigmaZ() const
{
    return 1.0/(M_PI*_Rmax*_Rmax);
}

//////////////////////////////////////////////////////////////////////

double SolarPatchGeometry::probabilityForDirection(Position /*bfr*/, Direction bfk) const
{
    double kx, ky, kz;
    bfk.cartesian(kx,ky,kz);
    return (kz > 0.0) ? 4.0*kz : 0.0;
}

//////////////////////////////////////////////////////////////////////

Direction SolarPatchGeometry::generateDirection(Position /*bfr*/) const
{
    double theta = asin(sqrt(_random->uniform()));
    double phi = 2.0*M_PI*_random->uniform();
    return Direction(theta,phi);
}

//////////////////////////////////////////////////////////////////////
