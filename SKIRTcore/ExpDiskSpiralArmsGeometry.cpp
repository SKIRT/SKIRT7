/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include <cmath>
#include <vector>
#include "ExpDiskSpiralArmsGeometry.hpp"
#include "FatalError.hpp"
#include "NR.hpp"
#include "Random.hpp"
#include "SpecialFunctions.hpp"

using namespace std;

//////////////////////////////////////////////////////////////////////

ExpDiskSpiralArmsGeometry::ExpDiskSpiralArmsGeometry()
    : GenGeometry(),
      _hR(0), _hz(0), _Rmax(0), _zmax(0),
      _m(0), _p(0), _phi0(0), _w(0), _tanp(0), _rho0(0),
      _Nphi(0), _dphi(0), _Ngamma(0), _dgamma(0), _Xvv()
{
}

//////////////////////////////////////////////////////////////////////

void
ExpDiskSpiralArmsGeometry::setupSelfBefore()
{
    GenGeometry::setupSelfBefore();

    // verify property values
    if (_hR <= 0) throw FATALERROR("The radial scale length hR should be positive");
    if (_hz <= 0) throw FATALERROR("The axial scale height hz should be positive");
    if (_Rmax < 0) throw FATALERROR("The radial truncation length Rmax should be zero or positive");
    if (_zmax < 0) throw FATALERROR("The axial truncation length zmax should be zero or positive");
    if (_m <= 0) throw FATALERROR("The number of spiral arms should be positive");
    if (_p <= 0 || _p >= M_PI/2.) throw FATALERROR("The pitch angle should be between 0 and 90 degrees");
    if (_phi0 < 0 || _phi0 > 2.0*M_PI) throw FATALERROR("The phase zero-point should be between 0 and 360 degrees");
    if (_w <= 0 || _w > 1.) throw FATALERROR("The weight of the spiral perturbation should be between 0 and 1");

    // cache frequently used values
    _tanp = tan(_p);
    _rho0 = 1.0/(4.0*M_PI*_hz*_hR*_hR);
    if (_Rmax>0.0)
        _rho0 /= 1.0-(1.0+_Rmax/_hR)*exp(-_Rmax/_hR);
    if (_zmax>0.0)
        _rho0 /= 1.0-exp(-_zmax/_hz);

    // setup the vector of phi values for the cumulative distribution

    _Nphi = 360;
    _dphi = 2.0*M_PI/_Nphi;
    _Ngamma = 720;
    _dgamma = 2.0*M_PI/_Ngamma;
    _Xvv.resize(_Ngamma+1, _Nphi+1);
    for (int k=0; k<=_Ngamma; k++)
    {
        double gamma = k*_dgamma;
        for (int i=0; i<=_Nphi; i++)
        {
            double phi = i*_dphi;
            _Xvv(k,i) = phi/(2.0*M_PI) + _w * sin(0.5*_m*phi) * sin(_m*(gamma-0.5*phi)) / (_m*M_PI);
        }
    }
}

////////////////////////////////////////////////////////////////////

void
ExpDiskSpiralArmsGeometry::setRadialScale(double value)
{
    _hR = value;
}

////////////////////////////////////////////////////////////////////

double
ExpDiskSpiralArmsGeometry::radialScale()
const
{
    return _hR;
}

////////////////////////////////////////////////////////////////////

void
ExpDiskSpiralArmsGeometry::setAxialScale(double value)
{
    _hz = value;
}

////////////////////////////////////////////////////////////////////

double
ExpDiskSpiralArmsGeometry::axialScale()
const
{
    return _hz;
}

////////////////////////////////////////////////////////////////////

void
ExpDiskSpiralArmsGeometry::setRadialTrunc(double value)
{
    _Rmax = value;
}

////////////////////////////////////////////////////////////////////

double
ExpDiskSpiralArmsGeometry::radialTrunc()
const
{
    return _Rmax;
}

////////////////////////////////////////////////////////////////////

void
ExpDiskSpiralArmsGeometry::setAxialTrunc(double value)
{
    _zmax = value;
}

////////////////////////////////////////////////////////////////////

double
ExpDiskSpiralArmsGeometry::axialTrunc()
const
{
    return _zmax;
}

////////////////////////////////////////////////////////////////////

void
ExpDiskSpiralArmsGeometry::setArms(int value)
{
    _m = value;
}

////////////////////////////////////////////////////////////////////

int
ExpDiskSpiralArmsGeometry::arms()
const
{
    return _m;
}

////////////////////////////////////////////////////////////////////

void
ExpDiskSpiralArmsGeometry::setPitch(double value)
{
    _p = value;
}

////////////////////////////////////////////////////////////////////

double
ExpDiskSpiralArmsGeometry::pitch()
const
{
    return _p;
}

////////////////////////////////////////////////////////////////////

void
ExpDiskSpiralArmsGeometry::setPhase(double value)
{
    _phi0 = value;
}

////////////////////////////////////////////////////////////////////

double
ExpDiskSpiralArmsGeometry::phase()
const
{
    return _phi0;
}

////////////////////////////////////////////////////////////////////

void
ExpDiskSpiralArmsGeometry::setPerturbWeight(double value)
{
    _w = value;
}

////////////////////////////////////////////////////////////////////

double
ExpDiskSpiralArmsGeometry::perturbWeight()
const
{
    return _w;
}

//////////////////////////////////////////////////////////////////////

double
ExpDiskSpiralArmsGeometry::density(Position bfr)
const
{
    double R, phi, z;
    bfr.cylindrical(R,phi,z);
    double absz = fabs(z);
    if (_Rmax>0.0 && R>_Rmax)
        return 0.0;
    else if (_zmax>0.0 && absz>_zmax)
        return 0.0;
    double gamma = log(R/_hR)/_tanp + _phi0;
    double perturbation = 1.0 + _w*sin(_m*(gamma-phi));
    return _rho0 * perturbation * exp(-R/_hR) * exp(-absz/_hz);
}

////////////////////////////////////////////////////////////////////

Position
ExpDiskSpiralArmsGeometry::generatePosition()
const
{
    double R, z, X;
    do
    {
        X = _random->uniform();
        R =_hR*(-1.0-SpecialFunctions::LambertW1((X-1.0)/M_E));
    }
    while (_Rmax>0.0 && R>=_Rmax);
    do
    {
        X = _random->uniform();
        z = (X<=0.5) ? _hz*log(2.0*X) : -_hz*log(2.0*(1.0-X));
    }
    while (_zmax>0.0 && fabs(z)>=_zmax);
    double gamma = log(R/_hR)/_tanp + _phi0;
    gamma -= floor(gamma/(2.0*M_PI))*2.0*M_PI; // ensure that gamma is between 0 and 2*pi
    int k = static_cast<int>(gamma/_dgamma);
    const Array& Xv = _Xvv[k];
    X = _random->uniform();
    int i = NR::locate_clip(Xv,X);
    double p = (X-Xv[i])/(Xv[i+1]-Xv[i]);
    double phi = (i+p)*_dphi;
    return Position(R,phi,z,Position::CYLINDRICAL);
}

//////////////////////////////////////////////////////////////////////

double
ExpDiskSpiralArmsGeometry::SigmaX()
const
{
    if (_Rmax>0.0)
        return 2.0 * _rho0 * _hR * (1.0-exp(-_Rmax/_hR));
    else
        return 2.0 * _rho0 * _hR;
}

//////////////////////////////////////////////////////////////////////

double
ExpDiskSpiralArmsGeometry::SigmaY()
const
{
    return SigmaX();
}

//////////////////////////////////////////////////////////////////////

double
ExpDiskSpiralArmsGeometry::SigmaZ()
const
{
    if (_zmax>0.0)
        return 2.0 * _rho0 * _hz * (1.0-exp(-_zmax/_hz));
    else
        return 2.0 * _rho0 * _hz;
}

///////////////////////////////////////////////////////////////////////////////
