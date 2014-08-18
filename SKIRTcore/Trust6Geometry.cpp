/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include <cmath>
#include "FatalError.hpp"
#include "Random.hpp"
#include "SpecialFunctions.hpp"
#include "Trust6Geometry.hpp"
#include "Units.hpp"

using namespace std;

//////////////////////////////////////////////////////////////////////

Trust6Geometry::Trust6Geometry()
    : _thetaop(0), _rhoambient(0), _rhoshell(0), _rmin(0),
      _rmax(0), _rshell(0), _sigmashell(0)
{
}

//////////////////////////////////////////////////////////////////////

void Trust6Geometry::setupSelfBefore()
{
    AxGeometry::setupSelfBefore();

    // set the values for the main parameters
    _rmin = 1.0 * Units::AU();
    _rmax = 300.0 * Units::AU();
    _rshell = 50.0 * Units::AU();
    _sigmashell = 1.0 * Units::AU();
    double f = 1e-6; // f = rhoambient/rhoshell

    // determine the density normalizations factor

    double t1 = 4.0*M_PI/3.0 * f * (_rmax-_rmin) * (_rmax*_rmax + _rmax*_rmin + _rmin*_rmin);
    double t2a = 2.0*M_PI * pow(_sigmashell,3) * (1.0+cos(_thetaop));
    double smin = _rmin/_sigmashell;
    double smax = _rmax/_sigmashell;
    double sshell = _rshell/_sigmashell;
    double sqrt2 = sqrt(2.0);
    double t2b = (sshell+smin)*exp(-0.5*pow(sshell-smin,2))
                    -(sshell+smax)*exp(-0.5*pow(sshell-smax,2))
                    +sqrt(0.5*M_PI)*(1.0+sshell*sshell)*
                        (erf((smax-sshell)/sqrt2)-erf((smin-sshell)/sqrt2));
    _rhoshell = 1.0/(t1+t2a*t2b);
    _rhoambient = _rhoshell * f;
}

////////////////////////////////////////////////////////////////////

void Trust6Geometry::setOpeningAngle(double value)
{
    _thetaop = value;
}

////////////////////////////////////////////////////////////////////

double Trust6Geometry::openingAngle() const
{
    return _thetaop;
}

//////////////////////////////////////////////////////////////////////

double
Trust6Geometry::density(double R, double z)
const
{
    double r = sqrt(R*R+z*z);
    if (r<_rmin || r>_rmax) return 0.0;
    double theta = 0.0;
    if (R==0.0)
        theta = (z>=0.0) ? 0.0 : M_PI;
    else
        theta = M_PI/2.0 - atan(z/R);
    double rho = _rhoambient;
    if (theta > _thetaop)
    {
        double t = (r-_rshell)/M_SQRT2/_sigmashell;
        rho += _rhoshell*exp(-t*t);
    }
    return rho;
}

//////////////////////////////////////////////////////////////////////

Position
Trust6Geometry::generatePosition()
const
{
    // not implemented...
    return Position();
}

//////////////////////////////////////////////////////////////////////

double
Trust6Geometry::SigmaR()
const
{
    double SigmaRambient = _rhoambient * (_rmax-_rmin);
    double SigmaRshell = 0.0;
    if (_thetaop < M_PI/2.0)
    {
        double umax = (_rmax-_rshell)/_sigmashell/sqrt(2.0);
        double umin = (_rmin-_rshell)/_sigmashell/sqrt(2.0);
        SigmaRshell = sqrt(M_PI/2.0) * _rhoshell * _sigmashell *
                      (erf(umax)-erf(umin));
    }
    return SigmaRambient + SigmaRshell;
}

//////////////////////////////////////////////////////////////////////

double
Trust6Geometry::SigmaZ()
const
{
    double SigmaZambient = 2.0 * _rhoambient * (_rmax-_rmin);
    double SigmaZshell = 0.0;
    if (_thetaop < M_PI)
    {
        double umax = (_rmax-_rshell)/_sigmashell/sqrt(2.0);
        double umin = (_rmin-_rshell)/_sigmashell/sqrt(2.0);
        SigmaZshell = sqrt(M_PI/2.0) * _rhoshell * _sigmashell *
                   (erf(umax)-erf(umin));
        if (_thetaop == 0.0) SigmaZshell *= 2.0;
    }
    return SigmaZambient + SigmaZshell;
}

//////////////////////////////////////////////////////////////////////
