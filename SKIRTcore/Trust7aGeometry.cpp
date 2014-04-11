/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include <cmath>
#include "Trust7aGeometry.hpp"
#include "FatalError.hpp"
#include "Random.hpp"
#include "Units.hpp"

using namespace std;

//////////////////////////////////////////////////////////////////////

Trust7aGeometry::Trust7aGeometry()
{
}

//////////////////////////////////////////////////////////////////////

void Trust7aGeometry::setupSelfBefore()
{
    SepAxGeometry::setupSelfBefore();

    // set property values
    _Rc = 0.1 * Units::pc();
    _Rout = 3.0 * Units::pc();
    _b = 10.0 * Units::pc();

    // calculate central density (along the centre of the filament)
    _rhoc = 1.0 / M_PI / (_b*_Rc*_Rc*log(1.0+_Rout*_Rout/_Rc/_Rc));
}

//////////////////////////////////////////////////////////////////////

double
Trust7aGeometry::density(double R, double z)
const
{
    if (R>_Rout || fabs(z)>0.5*_b) return 0.0;
    return _rhoc / (1.0+R*R/_Rc/_Rc);
}

//////////////////////////////////////////////////////////////////////

double
Trust7aGeometry::randomR()
const
{
    double X = _random->uniform();
    double t = _Rout/_Rc;
    return _Rc * sqrt( pow(1.0+t*t,X)-1.0 );
}

//////////////////////////////////////////////////////////////////////

double
Trust7aGeometry::randomz()
const
{
    return _b * (_random->uniform()-0.5);
}

//////////////////////////////////////////////////////////////////////

double
Trust7aGeometry::SigmaR()
const
{
    return _rhoc*_Rc * atan(_Rout/_Rc);
}

//////////////////////////////////////////////////////////////////////

double
Trust7aGeometry::SigmaZ()
const
{
    return _rhoc*_b;
}

//////////////////////////////////////////////////////////////////////
