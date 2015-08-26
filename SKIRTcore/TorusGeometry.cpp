/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include <cmath>
#include "FatalError.hpp"
#include "Random.hpp"
#include "SpecialFunctions.hpp"
#include "TorusGeometry.hpp"

using namespace std;

//////////////////////////////////////////////////////////////////////

TorusGeometry::TorusGeometry()
    : _p(0), _q(0), _Delta(0), _rmin(0), _rmax(0), _rani(false), _rcut(0),
      _sinDelta(0), _smin(0), _sdiff(0), _A(0)
{
}

//////////////////////////////////////////////////////////////////////

void TorusGeometry::setupSelfBefore()
{
    AxGeometry::setupSelfBefore();

    // verify property values
    if (_p < 0) throw FATALERROR("The radial power law exponent p of the torus should be positive");
    if (_q < 0) throw FATALERROR("The polar index q of the torus should be positive");
    if (_Delta < 0) throw FATALERROR("The half opening angle of the torus should be positive");
    if (_rmin <= 0) throw FATALERROR("The minimum radius of the torus should be positive");
    if (_rmax <= _rmin) throw FATALERROR("The maximum radius of the torus should be larger than the minimum radius");
    if (_rani && _rcut <= 0) throw FATALERROR("The inner cutoff radius of the torus should be positive");

    // cache frequently used values
    _sinDelta = sin(_Delta);
    _smin = SpecialFunctions::gln(_p-2.0,_rmin);
    _sdiff = SpecialFunctions::gln2(_p-2.0,_rmax,_rmin);

    // determine the normalization factor
    if (_q>1e-3)
        _A = _q * 0.25/M_PI / _sdiff / (1.0-exp(-_q*_sinDelta));
    else
        _A = 0.25/M_PI / _sdiff / _sinDelta;
}

////////////////////////////////////////////////////////////////////

void TorusGeometry::setExpon(double value)
{
    _p = value;
}

////////////////////////////////////////////////////////////////////

double TorusGeometry::expon() const
{
    return _p;
}

////////////////////////////////////////////////////////////////////

void TorusGeometry::setIndex(double value)
{
    _q = value;
}

////////////////////////////////////////////////////////////////////

double TorusGeometry::index() const
{
    return _q;
}

////////////////////////////////////////////////////////////////////

void TorusGeometry::setOpenAngle(double value)
{
    _Delta = value;
}

////////////////////////////////////////////////////////////////////

double TorusGeometry::openAngle() const
{
    return _Delta;
}

////////////////////////////////////////////////////////////////////

void TorusGeometry::setMinRadius(double value)
{
    _rmin = value;
}

////////////////////////////////////////////////////////////////////

double TorusGeometry::minRadius() const
{
    return _rmin;
}

////////////////////////////////////////////////////////////////////

void TorusGeometry::setMaxRadius(double value)
{
    _rmax = value;
}

////////////////////////////////////////////////////////////////////

double TorusGeometry::maxRadius() const
{
    return _rmax;
}

////////////////////////////////////////////////////////////////////

void TorusGeometry::setAnisoRadius(bool value)
{
    _rani = value;
}

////////////////////////////////////////////////////////////////////

bool TorusGeometry::anisoRadius() const
{
    return _rani;
}

////////////////////////////////////////////////////////////////////

void TorusGeometry::setCutRadius(double value)
{
    _rcut = value;
}

////////////////////////////////////////////////////////////////////

double TorusGeometry::cutRadius() const
{
    return _rcut;
}

//////////////////////////////////////////////////////////////////////

double TorusGeometry::density(double R, double z) const
{
    double r = sqrt(R*R+z*z);
    double costheta = z/r;

    if (r>=_rmax) return 0.0;
    if (_rani)
    {
        double rminani = _rmin*sqrt(6./7.*fabs(costheta)*(2.*fabs(costheta)+1));
        if (r<=rminani || r<_rcut) return 0.0;
    }
    else
    {
        if (r<=_rmin) return 0.0;
    }

    if (fabs(costheta) >=_sinDelta) return 0.0;
    return _A * pow(r,-_p) * exp(-_q*fabs(costheta));
}

//////////////////////////////////////////////////////////////////////

Position TorusGeometry::generatePosition() const
{
    double X = _random->uniform();
    double s = _smin + X*_sdiff;
    double r = SpecialFunctions::gexp(_p-2.0,s);
    X = _random->uniform();
    double costheta = 0.0;
    if (_q<1e-3)
        costheta = (1.0-2.0*X)*_sinDelta;
    else
    {
        double B = 1.0-exp(-_q*_sinDelta);
        costheta = (X<0.5) ? -log(1.0-B*(1.0-2.0*X))/_q : log(1.0-B*(2.0*X-1.0))/_q;
    }
    double theta = acos(costheta);
    X = _random->uniform();
    double phi = 2.0 * M_PI * X;
    Position bfr(r,theta,phi,Position::SPHERICAL);
    return bfr;
}

//////////////////////////////////////////////////////////////////////

double TorusGeometry::SigmaR() const
{
    return _A * SpecialFunctions::gln2(_p,_rmax,_rmin);
}

//////////////////////////////////////////////////////////////////////

double TorusGeometry::SigmaZ() const
{
    return 0.0;
}

//////////////////////////////////////////////////////////////////////
