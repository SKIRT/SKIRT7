/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include <cmath>
#include "Trust1Geometry.hpp"
#include "FatalError.hpp"
#include "Random.hpp"
#include "Units.hpp"

using namespace std;

//////////////////////////////////////////////////////////////////////

Trust1Geometry::Trust1Geometry()
{
}

//////////////////////////////////////////////////////////////////////

void Trust1Geometry::setupSelfBefore()
{
    GenGeometry::setupSelfBefore();

    // set property values
    _xmin = -5.0 * Units::pc();
    _xmax = 5.0 * Units::pc();
    _ymin = -5.0 * Units::pc();
    _ymax = 5.0 * Units::pc();
    _zmin = -5.0 * Units::pc();
    _zmax = -2.0 * Units::pc();

    // calculate central density (along the centre of the helix)
    _rho = 1.0 / (_xmax-_xmin) / (_ymax-_ymin) / (_zmax-_zmin);
}

//////////////////////////////////////////////////////////////////////

double
Trust1Geometry::density(Position bfr)
const
{
    double x, y, z;
    bfr.cartesian(x,y,z);
    if (x<_xmin || x>_xmax || y<_ymin || y>_ymax || z<_zmin ||z>_zmax) return 0.0;
    return _rho;
}

//////////////////////////////////////////////////////////////////////

Position
Trust1Geometry::generatePosition()
const
{
    double x = _xmin + (_xmax-_xmin)*_random->uniform();
    double y = _ymin + (_ymax-_ymin)*_random->uniform();
    double z = _zmin + (_zmax-_zmin)*_random->uniform();
    return Position(x,y,z);
}

//////////////////////////////////////////////////////////////////////

double
Trust1Geometry::SigmaX()
const
{
    return 0.0;
}

//////////////////////////////////////////////////////////////////////

double
Trust1Geometry::SigmaY()
const
{
    return 0.0;
}

//////////////////////////////////////////////////////////////////////

double
Trust1Geometry::SigmaZ()
const
{
    return _rho * (_zmax-_zmin);
}

//////////////////////////////////////////////////////////////////////
