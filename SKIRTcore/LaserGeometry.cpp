/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include <cmath>
#include <limits>
#include "LaserGeometry.hpp"
#include "FatalError.hpp"

using namespace std;

//////////////////////////////////////////////////////////////////////

LaserGeometry::LaserGeometry()
{
}

//////////////////////////////////////////////////////////////////////

int
LaserGeometry::dimension()
const
{
    return 2;
}

//////////////////////////////////////////////////////////////////////

double
LaserGeometry::density(Position bfr)
const
{
    return bfr.radius() == 0 ? numeric_limits<double>::infinity() : 0;
}

//////////////////////////////////////////////////////////////////////

Position
LaserGeometry::generatePosition()
const
{
    return Position();
}

//////////////////////////////////////////////////////////////////////

double
LaserGeometry::SigmaX()
const
{
    return numeric_limits<double>::infinity();
}

//////////////////////////////////////////////////////////////////////

double
LaserGeometry::SigmaY()
const
{
    return numeric_limits<double>::infinity();
}

//////////////////////////////////////////////////////////////////////

double
LaserGeometry::SigmaZ()
const
{
    return numeric_limits<double>::infinity();
}

//////////////////////////////////////////////////////////////////////

double
LaserGeometry::probabilityForDirection(int /*ell*/, Position bfr, Direction bfk)
const
{
    if (bfr.radius()>0.0)
        throw FATALERROR("the angular probability function is not defined for positions besides the origin");
    double theta, phi;
    bfk.spherical(theta, phi);
    return (theta==0.0) ? numeric_limits<double>::infinity() : 0.0;
}

//////////////////////////////////////////////////////////////////////

Direction
LaserGeometry::generateDirection(int /*ell*/, Position bfr)
const
{
    if (bfr.radius()>0.0)
        throw FATALERROR("no directions should be generated at positions besides the origin");
    return Direction(0.0,0.0,1.0);
}

//////////////////////////////////////////////////////////////////////
