/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include <cmath>
#include <limits>
#include "FatalError.hpp"
#include "NetzerAccretionDiskGeometry.hpp"
#include "Random.hpp"

using namespace std;

//////////////////////////////////////////////////////////////////////

NetzerAccretionDiskGeometry::NetzerAccretionDiskGeometry()
{
}

//////////////////////////////////////////////////////////////////////

void NetzerAccretionDiskGeometry::setupSelfBefore()
{
    Geometry::setupSelfBefore();

    // grid with values of the cumulative luminosity distribution
    int N = 401;
    _thetav.resize(N);
    _Xv.resize(N);
    _thetav[0] = 0;
    _Xv[0] = 0;
    for (int i=1; i<N-1; i++)
    {
        _thetav[i] = M_PI*i/(N-1);
        double ct = cos(_thetav[i]);
        double sign = ct > 0 ? 1. : -1;
        _Xv[i] = (1./2.) - (2./7.)*ct*ct*ct - sign*(3./14.)*ct*ct;
    }
    _thetav[N-1] = M_PI;
    _Xv[N-1] = 1.0;
}

//////////////////////////////////////////////////////////////////////

int NetzerAccretionDiskGeometry::dimension() const
{
    return 2;
}

//////////////////////////////////////////////////////////////////////

double NetzerAccretionDiskGeometry::density(Position bfr) const
{
    return bfr.radius() == 0 ? numeric_limits<double>::infinity() : 0;
}

//////////////////////////////////////////////////////////////////////

Position NetzerAccretionDiskGeometry::generatePosition() const
{
    return Position();
}

//////////////////////////////////////////////////////////////////////

double NetzerAccretionDiskGeometry::SigmaX() const
{
    return numeric_limits<double>::infinity();
}

//////////////////////////////////////////////////////////////////////

double NetzerAccretionDiskGeometry::SigmaY() const
{
    return numeric_limits<double>::infinity();
}

//////////////////////////////////////////////////////////////////////

double NetzerAccretionDiskGeometry::SigmaZ() const
{
    return numeric_limits<double>::infinity();
}

//////////////////////////////////////////////////////////////////////

double
NetzerAccretionDiskGeometry::probabilityForDirection(int /*ell*/, Position bfr, Direction bfk)
const
{
    if (bfr.radius()>0.0)
        throw FATALERROR("the angular probability function is not defined for positions besides the origin");
    double theta, phi;
    bfk.spherical(theta, phi);
    double ct = cos(theta);
    double sign = ct > 0 ? 1. : -1;
    return (6./7.) * ct * (2.*ct + sign);
}

//////////////////////////////////////////////////////////////////////

Direction
NetzerAccretionDiskGeometry::generateDirection(int /*ell*/, Position bfr)
const
{
    if (bfr.radius()>0.0)
        throw FATALERROR("no directions should be generated at positions besides the origin");
    double theta = _random->cdf(_thetav,_Xv);
    double phi = 2.0*M_PI*_random->uniform();
    return Direction(theta,phi);
}

//////////////////////////////////////////////////////////////////////
