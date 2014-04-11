/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include <cmath>
#include "FatalError.hpp"
#include "NR.hpp"
#include "Random.hpp"
#include "RingGeometry.hpp"

using namespace std;

//////////////////////////////////////////////////////////////////////

RingGeometry::RingGeometry()
    : _R0(0), _w(0), _hz(0), _A(0)
{
}

////////////////////////////////////////////////////////////////////

void RingGeometry::setupSelfBefore()
{
    SepAxGeometry::setupSelfBefore();

    // verify property values
    if (_R0 <= 0) throw FATALERROR("The radius R0 should be positive");
    if (_w <= 0) throw FATALERROR("The radial width w should be positive");
    if (_hz <= 0) throw FATALERROR("The axial scale height hz should be positive");

    // determine normalization parameter
    double t = _R0/_w/M_SQRT2;
    double intz = 2.0*_hz;
    double intR = _w*_w * (exp(-t*t) + sqrt(M_PI)*t*(1.0+erf(t)));
    _A = 1.0 / (2.0*M_PI*intz*intR);

    // setup the cumulative distribution for the radial distribution
    int NR = 330;
    NR::lingrid(_Rv, max(0.,_R0-8*_w), _R0+8*_w, NR);
    _Xv.resize(NR);
    double sqrtpi = sqrt(M_PI);
    for (int i=0; i<NR; i++)
    {
        double R = _Rv[i];
        double u = (_R0-R)/_w/M_SQRT2;
        _Xv[i] = 4.0*M_PI*_A*_hz*_w*_w * ( exp(-t*t) - exp(-u*u) + sqrtpi*t*(erf(t)-erf(u)) );
    }
    _Xv[0] = 0.0;
    _Xv[NR-1] = 1.0;
}

////////////////////////////////////////////////////////////////////

void RingGeometry::setRadius(double value)
{
    _R0 = value;
}

////////////////////////////////////////////////////////////////////

double RingGeometry::radius() const
{
    return _R0;
}

////////////////////////////////////////////////////////////////////

void RingGeometry::setWidth(double value)
{
    _w = value;
}

////////////////////////////////////////////////////////////////////

double RingGeometry::width() const
{
    return _w;
}

////////////////////////////////////////////////////////////////////

void RingGeometry::setHeight(double value)
{
    _hz = value;
}

////////////////////////////////////////////////////////////////////

double RingGeometry::height() const
{
    return _hz;
}

//////////////////////////////////////////////////////////////////////

double
RingGeometry::density(double R, double z)
const
{
    double u = (R-_R0)/(M_SQRT2*_w);
    return _A * exp(-u*u) * exp(-fabs(z)/_hz);
}

//////////////////////////////////////////////////////////////////////

double
RingGeometry::randomR()
const
{
    return _random->cdf(_Rv,_Xv);
}

//////////////////////////////////////////////////////////////////////

double
RingGeometry::randomz()
const
{
    double X = _random->uniform();
    double z = (X<=0.5) ? _hz*log(2.0*X) : -_hz*log(2.0*(1.0-X));
    return z;
}

//////////////////////////////////////////////////////////////////////

double
RingGeometry::SigmaR()
const
{
    double t = _R0/(M_SQRT2*_w);
    return sqrt(M_PI/2.0)*_A*_w*(1.0+erf(t));
}

//////////////////////////////////////////////////////////////////////

double
RingGeometry::SigmaZ()
const
{
    double t = _R0/(M_SQRT2*_w);
    return 2.0*_A*_hz*exp(-t*t);
}

//////////////////////////////////////////////////////////////////////
