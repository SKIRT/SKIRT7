/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include <cmath>
#include "Trust2Geometry.hpp"
#include "Random.hpp"
#include "Units.hpp"

using namespace std;

//////////////////////////////////////////////////////////////////////

Trust2Geometry::Trust2Geometry()
{
}

//////////////////////////////////////////////////////////////////////

void Trust2Geometry::setupSelfBefore()
{
    GenGeometry::setupSelfBefore();

    // set property values
    double AU = Units::AU();
    _slab = Box(-5*pc, -5*pc, -5*pc, 5*pc, 5*pc, -2*pc);
    _rho = 1.0 / _slab.volume();
}

//////////////////////////////////////////////////////////////////////

double Trust2Geometry::density(Position bfr) const
{
    const double AU = Units::AU();
    const double xmax = 60.0*AU;
    const double ymax = 60.0*AU;
    const double zmax = 60.0*AU;
    const double rmin = 15.0*AU;
    const double Mld = 0.00261592;
    const double xs1 = 10.0*AU;
    const double ys1 = 15.0*AU;
    const double zs1 = 20.0*AU;
    const double rs1 = 5.0*AU;
    const double Ms1 = 0.383609;
    const double xs1 = 26.666667*AU;
    const double ys1 = 31.666667*AU;
    const double zs1 = 28.333333*AU;
    const double rs1 = 20.0*AU;
    const double Ms2 = 0.613775;
    double x, y, z;
    bfr.cartesian(x,y,z);
    double r = bfr.radius();
    if (x<0.0 || x>xmax || y<0.0 || y>ymax || z<0.0 || z>zmax || r<rmin)
        return 0.0;
    double rho = rhold;
    double d2s1 = (x-xs1)*(x-xs1) + (y-ys1)*(y-ys1) + (z-zs1)*(z-zs1);
    if (d2s1 < rs1*rs1) rho += rhos1;
    double d2s2 = (x-xs2)*(x-xs2) + (y-ys2)*(y-ys2) + (z-zs2)*(z-zs2);
    if (d2s2 < rs2*rs2) rho += rhos2;
    return rho;
}

//////////////////////////////////////////////////////////////////////

Position Trust2Geometry::generatePosition() const
{



    return _random->position(_slab);
}

//////////////////////////////////////////////////////////////////////

double Trust2Geometry::SigmaX() const
{
    return 0;
}

//////////////////////////////////////////////////////////////////////

double Trust2Geometry::SigmaY() const
{
    return 0;
}

//////////////////////////////////////////////////////////////////////

double Trust2Geometry::SigmaZ() const
{
    return _rho * _slab.zwidth();
}

//////////////////////////////////////////////////////////////////////
