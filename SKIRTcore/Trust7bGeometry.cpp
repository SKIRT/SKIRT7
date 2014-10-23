/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include <cmath>
#include "Trust7bGeometry.hpp"
#include "FatalError.hpp"
#include "Random.hpp"
#include "Units.hpp"

using namespace std;

//////////////////////////////////////////////////////////////////////

Trust7bGeometry::Trust7bGeometry()
{
}

//////////////////////////////////////////////////////////////////////

void Trust7bGeometry::setupSelfBefore()
{
    GenGeometry::setupSelfBefore();

    // set property values
    _Rc = 0.1 * Units::pc();
    _Rout = 3.0 * Units::pc();
    _a = 1.0 * Units::pc();
    _b = 10.0 * Units::pc();

    // calculate central density (along the centre of the helix)
    _rhoc = 1.0 / M_PI / (_b*_Rc*_Rc*log(1.0+_Rout*_Rout/_Rc/_Rc));
}

//////////////////////////////////////////////////////////////////////

double
Trust7bGeometry::density(Position bfr)
const
{
    double x, y, z;
    bfr.cartesian(x,y,z);
    if (fabs(z)>0.5*_b) return 0.0;
    double t = 2.0*M_PI*(z/_b);
    double xf = _a*sin(t);
    double yf = _a*cos(t);
    double Rf2 = (x-xf)*(x-xf) + (y-yf)*(y-yf);
    if (Rf2<0.0 || Rf2>_Rout*_Rout) return 0.0;
    return _rhoc / (1.0+Rf2/_Rc/_Rc);
}

//////////////////////////////////////////////////////////////////////

Position
Trust7bGeometry::generatePosition()
const
{
    double z = _b * (_random->uniform()-0.5);
    double t = _Rout/_Rc;
    double Rf = _Rc * sqrt( pow(1.0+t*t,_random->uniform())-1.0 );
    double phi = 2.0*M_PI*_random->uniform();
    double x = _a*sin(2.0*M_PI*z/_b) + Rf*cos(phi);
    double y = -_a*cos(2.0*M_PI*z/_b) + Rf*sin(phi);
    return Position(x,y,z);
}

//////////////////////////////////////////////////////////////////////

double
Trust7bGeometry::SigmaX()
const
{
    double xmax = sqrt(_Rout*_Rout-_a*_a);
    double p = sqrt(_a*_a+_Rc*_Rc);
    return 2.0*_rhoc*_Rc*_Rc/p * atan(xmax/p);
}

//////////////////////////////////////////////////////////////////////

double
Trust7bGeometry::SigmaY()
const
{
    return 2.0*_Rc*_rhoc*atan(_Rout/_Rc);
}

//////////////////////////////////////////////////////////////////////

double
Trust7bGeometry::SigmaZ()
const
{
    return (_rhoc*_b) / (1.0+_a*_a/_Rc/_Rc);
}

//////////////////////////////////////////////////////////////////////
