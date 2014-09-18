/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include <cmath>
#include "FatalError.hpp"
#include "Random.hpp"
#include "ShellGeometry.hpp"
#include "SpecialFunctions.hpp"

using namespace std;

//////////////////////////////////////////////////////////////////////

ShellGeometry::ShellGeometry()
    : _rmin(0), _rmax(0), _p(0), _smin(0), _sdiff(0), _A(0)
{
}

////////////////////////////////////////////////////////////////////

void ShellGeometry::setupSelfBefore()
{
    SpheGeometry::setupSelfBefore();

    // verify property values
    if (_rmin <= 0) throw FATALERROR("the inner radius of the shell should be positive");
    if (_rmax <= _rmin) throw FATALERROR("the outer radius of the shell should be larger than the inner radius");
    if (_p < 0) throw FATALERROR("the power law exponent p should be positive");

    // calculate cached values
    _smin = SpecialFunctions::gln(_p-2.0,_rmin);
    _sdiff = SpecialFunctions::gln2(_p-2.0,_rmax,_rmin);
    _A = 0.25/M_PI / _sdiff;
}

////////////////////////////////////////////////////////////////////

void ShellGeometry::setMinRadius(double value)
{
    _rmin = value;
}

////////////////////////////////////////////////////////////////////

double ShellGeometry::minRadius() const
{
    return _rmin;
}

////////////////////////////////////////////////////////////////////

void ShellGeometry::setMaxRadius(double value)
{
    _rmax = value;
}

////////////////////////////////////////////////////////////////////

double ShellGeometry::maxRadius() const
{
    return _rmax;
}

////////////////////////////////////////////////////////////////////

void ShellGeometry::setExpon(double value)
{
    _p = value;
}

////////////////////////////////////////////////////////////////////

double ShellGeometry::expon() const
{
    return _p;
}

//////////////////////////////////////////////////////////////////////

double ShellGeometry::density(double r) const
{
    if (r<_rmin || r>_rmax)
        return 0.0;
    else
        return _A * pow(r,-_p);
}

//////////////////////////////////////////////////////////////////////

double ShellGeometry::randomradius() const
{
    double X = _random->uniform();
    double s = _smin + X*_sdiff;
    return SpecialFunctions::gexp(_p-2.0,s);
}

//////////////////////////////////////////////////////////////////////

double ShellGeometry::Sigmar() const
{
    return _A * SpecialFunctions::gln2(_p,_rmax,_rmin);
}

//////////////////////////////////////////////////////////////////////
