/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include <cmath>
#include "FatalError.hpp"
#include "Random.hpp"
#include "SpecialFunctions.hpp"
#include "ConicalShellGeometry.hpp"

using namespace std;

//////////////////////////////////////////////////////////////////////

ConicalShellGeometry::ConicalShellGeometry()
    : _p(0), _q(0), _DeltaIn(0), _DeltaOut(0), _rmin(0), _rani(false), _rcut(0),
      _rmax(0), _sinDeltaIn(0), _sinDeltaOut(0), _cosDelta(0), _smin(0), _sdiff(0), _A(0)
{
}

//////////////////////////////////////////////////////////////////////

void ConicalShellGeometry::setupSelfBefore()
{
    AxGeometry::setupSelfBefore();

    // verify property values
    if (_p < 0) throw FATALERROR("The radial power law exponent p of the conical shell should be positive");
    if (_q < 0) throw FATALERROR("The polar index q of the conical shell should be positive");
    if (_DeltaIn < 0) throw FATALERROR("The inner angle of the conical shell should be positive");
    if (_DeltaOut < 0) throw FATALERROR("The outer angle of the conical shell should be positive");
    if (_DeltaOut <= _DeltaIn) throw FATALERROR("the outer angle of the shell should be larger than the inner angle");
    if (_rmin <= 0) throw FATALERROR("The minimum radius of the conical shell should be positive");
    if (_rmax <= _rmin) throw FATALERROR("The maximum radius of the conical shell should be larger than the "
                                         "minimum radius");
    if (_rani && _rcut <= 0) throw FATALERROR("The inner cutoff radius of the torus should be positive");

    // cache frequently used values
    _sinDeltaIn = sin(_DeltaIn);
    _sinDeltaOut = sin(_DeltaOut);
    _cosDelta = cos((_DeltaOut+_DeltaIn)/2.);
    _smin = SpecialFunctions::gln(_p-2.0,_rmin);
    _sdiff = SpecialFunctions::gln2(_p-2.0,_rmax,_rmin);

    // determine the normalization factor
    if (_q>1e-3)
        _A = _q * 0.25/M_PI / _sdiff / (exp(-_q*_sinDeltaIn)-exp(-_q*_sinDeltaOut));
    else
        _A = 0.25/M_PI / _sdiff / (_sinDeltaOut-_sinDeltaIn);
}

////////////////////////////////////////////////////////////////////

void ConicalShellGeometry::setExpon(double value)
{
    _p = value;
}

////////////////////////////////////////////////////////////////////

double ConicalShellGeometry::expon() const
{
    return _p;
}

////////////////////////////////////////////////////////////////////

void ConicalShellGeometry::setIndex(double value)
{
    _q = value;
}

////////////////////////////////////////////////////////////////////

double ConicalShellGeometry::index() const
{
    return _q;
}

////////////////////////////////////////////////////////////////////

void ConicalShellGeometry::setInAngle(double value)
{
    _DeltaIn = value;
}

////////////////////////////////////////////////////////////////////

double ConicalShellGeometry::inAngle() const
{
    return _DeltaIn;
}

////////////////////////////////////////////////////////////////////

void ConicalShellGeometry::setOutAngle(double value)
{
    _DeltaOut = value;
}

////////////////////////////////////////////////////////////////////

double ConicalShellGeometry::outAngle() const
{
    return _DeltaOut;
}

////////////////////////////////////////////////////////////////////

void ConicalShellGeometry::setMinRadius(double value)
{
    _rmin = value;
}

////////////////////////////////////////////////////////////////////

double ConicalShellGeometry::minRadius() const
{
    return _rmin;
}

////////////////////////////////////////////////////////////////////

void ConicalShellGeometry::setAnisoRadius(bool value)
{
    _rani = value;
}

////////////////////////////////////////////////////////////////////

bool ConicalShellGeometry::anisoRadius() const
{
    return _rani;
}

////////////////////////////////////////////////////////////////////

void ConicalShellGeometry::setCutRadius(double value)
{
    _rcut = value;
}

////////////////////////////////////////////////////////////////////

double ConicalShellGeometry::cutRadius() const
{
    return _rcut;
}

////////////////////////////////////////////////////////////////////

void ConicalShellGeometry::setMaxRadius(double value)
{
    _rmax = value;
}

////////////////////////////////////////////////////////////////////

double ConicalShellGeometry::maxRadius() const
{
    return _rmax;
}

//////////////////////////////////////////////////////////////////////

double ConicalShellGeometry::density(double R, double z) const
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

    if (fabs(costheta) >=_sinDeltaOut) return 0.0;
    if (fabs(costheta) <=_sinDeltaIn) return 0.0;
    return _A * pow(r,-_p) * exp(-_q*fabs(costheta));
}

//////////////////////////////////////////////////////////////////////

Position ConicalShellGeometry::generatePosition() const
{
    while (true)
    {
        double X = _random->uniform();
        double s = _smin + X*_sdiff;
        double r = SpecialFunctions::gexp(_p-2.0,s);
        X = _random->uniform();
        double costheta = 0.0;
        if (_q<1e-3)
            costheta = (1.0-2.0*X)*_sinDeltaOut;
        else
        {
            double B = 1.0-exp(-_q*_sinDeltaOut);
            costheta = (X<0.5) ? -log(1.0-B*(1.0-2.0*X))/_q : log(1.0-B*(2.0*X-1.0))/_q;
        }
        double theta = acos(costheta);
        X = _random->uniform();
        double phi = 2.0 * M_PI * X;
        Position bfr(r,theta,phi,Position::SPHERICAL);
        if (density(bfr.cylradius(),bfr.height())) return bfr;
    }
}

//////////////////////////////////////////////////////////////////////

double ConicalShellGeometry::SigmaR() const
{
    return _A * exp(-_q*_cosDelta) * SpecialFunctions::gln2(_p,_rmax,_rmin);
}

//////////////////////////////////////////////////////////////////////

double ConicalShellGeometry::SigmaZ() const
{
    return 0.0;
}

//////////////////////////////////////////////////////////////////////
