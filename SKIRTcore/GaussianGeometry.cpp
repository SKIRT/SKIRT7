/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include <cmath>
#include "FatalError.hpp"
#include "GaussianGeometry.hpp"
#include "Random.hpp"

using namespace std;

////////////////////////////////////////////////////////////////////

GaussianGeometry::GaussianGeometry()
    : _sigma(0), _q(0), _rho0(0)
{
}

//////////////////////////////////////////////////////////////////////

void GaussianGeometry::setupSelfBefore()
{
    SepAxGeometry::setupSelfBefore();

    // verify property values
    if (_sigma <= 0) throw FATALERROR("the dispersion parameter sigma should be positive");
    if (_q <= 0 || _q > 1.0) throw FATALERROR("the flattening parameter q should be between 0 and 1");

    // calculate cached values
    _rho0 = 1.0 / pow(sqrt(2.0*M_PI)*_sigma,3) / _q;
}

////////////////////////////////////////////////////////////////////

void GaussianGeometry::setDispersion(double value)
{
    _sigma = value;
}

////////////////////////////////////////////////////////////////////

double GaussianGeometry::dispersion() const
{
    return _sigma;
}

////////////////////////////////////////////////////////////////////

void GaussianGeometry::setFlattening(double value)
{
    _q = value;
}

////////////////////////////////////////////////////////////////////

double GaussianGeometry::flattening() const
{
    return _q;
}

////////////////////////////////////////////////////////////////////

double
GaussianGeometry::density(double R, double z)
const
{
    double m2 = R*R + z*z/(_q*_q);
    double sigma2 = _sigma*_sigma;
    return _rho0 * exp(-0.5*m2/sigma2);
}

////////////////////////////////////////////////////////////////////

double
GaussianGeometry::randomR()
const
{
    double X = _random->uniform();
    return _sigma * sqrt(-2.0*log(X));
}

////////////////////////////////////////////////////////////////////

double
GaussianGeometry::randomz()
const
{
    return (_q*_sigma) * _random->gauss();
}

////////////////////////////////////////////////////////////////////

double
GaussianGeometry::SigmaR()
const
{
    return 1.0/(4.0*M_PI*_q*_sigma*_sigma);
}

////////////////////////////////////////////////////////////////////

double
GaussianGeometry::SigmaZ()
const
{
    return 1.0/(2.0*M_PI*_sigma*_sigma);
}

////////////////////////////////////////////////////////////////////
