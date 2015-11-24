/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include <cmath>
#include "BrokenExpDiskGeometry.hpp"
#include "FatalError.hpp"
#include "Random.hpp"
#include "NR.hpp"
#include "SpecialFunctions.hpp"
#include "Units.hpp"
using namespace std;


////////////////////////////////////////////////////////////////////

BrokenExpDiskGeometry::BrokenExpDiskGeometry()
    : _hinn(0), _hout(0), _hz(0), _Rb(0), _s(0),
      _beta(0), _rho0(0), _SigmaR(0), _Rv(0), _Xv(0)
{
}

////////////////////////////////////////////////////////////////////

void BrokenExpDiskGeometry::setupSelfBefore()
{
    SepAxGeometry::setupSelfBefore();

    // verify property values
    if (_hinn <= 0) throw FATALERROR("The inner scale length should be positive");
    if (_hout <= 0) throw FATALERROR("The outer scale length should be positive");
    if (_hz <= 0) throw FATALERROR("The axial scale height should be positive");
    if (_Rb <= 0) throw FATALERROR("The break radius should be positive");
    if (_s <= 0) throw FATALERROR("The sharpness should be positive");

    _beta = 1.0/_s*(_hout/_hinn-1.0);

    // create a radial array with the cumulative mass distribution. Use Ninn points in the
    // range between 0 and _Rb, and Nout points between _Rb and the outermost radius,
    // which we choose to be _Rb + 10*_hout.

    int Ninn = 200;
    int Nout = 400;
    int N = Ninn+Nout;
    _Rv.resize(N+1);
    _Xv.resize(N+1);
    double dRinn = _Rb/Ninn;
    for (int i=0; i<Ninn; i++) _Rv[i] = i*dRinn;
    double dRout = 10*_hout/Nout;
    for (int i=Ninn; i<=N; i++) _Rv[i] = _Rb + (i-Ninn)*dRout;
    _Xv[0] = 0.0;
    double intprev = 0.0;
    for (int i=1; i<=N; i++)
    {
        double RL = _Rv[i-1];
        double RR = _Rv[i];
        double intL = intprev;
        double intR = radialdensity(RR)*RR;
        intprev = intR;
        _Xv[i] = _Xv[i-1] + 0.5*(RR-RL)*(intL+intR);
    }
    double IR = _Xv[N-1];
    _Xv /= IR;

    // calculate _rho0;

    _rho0 = 1.0/4.0/M_PI/_hz/IR;

    // calculate the radial surface density

    intprev = 0.0;
    double sum = 0.0;
    for (int i=1; i<=N; i++)
    {
        double RL = _Rv[i-1];
        double RR = _Rv[i];
        double intL = intprev;
        double intR = radialdensity(RR);
        intprev = intR;
        sum += 0.5*(RR-RL)*(intL+intR);
    }
    _SigmaR = sum*_rho0;
}


////////////////////////////////////////////////////////////////////

void BrokenExpDiskGeometry::setRadialScaleInner(double value)
{
    _hinn = value;
}

////////////////////////////////////////////////////////////////////

double BrokenExpDiskGeometry::radialScaleInner() const
{
    return _hinn;
}

////////////////////////////////////////////////////////////////////

void BrokenExpDiskGeometry::setRadialScaleOuter(double value)
{
    _hout = value;
}

////////////////////////////////////////////////////////////////////

double BrokenExpDiskGeometry::radialScaleOuter() const
{
    return _hout;
}

////////////////////////////////////////////////////////////////////

void BrokenExpDiskGeometry::setAxialScale(double value)
{
    _hz = value;
}

////////////////////////////////////////////////////////////////////

double BrokenExpDiskGeometry::axialScale() const
{
    return _hz;
}

////////////////////////////////////////////////////////////////////

void BrokenExpDiskGeometry::setBreakRadius(double value)
{
    _Rb = value;
}

////////////////////////////////////////////////////////////////////

double BrokenExpDiskGeometry::breakRadius() const
{
    return _Rb;
}

////////////////////////////////////////////////////////////////////

void BrokenExpDiskGeometry::setSharpness(double value)
{
    _s = value;
}

////////////////////////////////////////////////////////////////////

double BrokenExpDiskGeometry::sharpness() const
{
    return _s;
}

////////////////////////////////////////////////////////////////////

double BrokenExpDiskGeometry::radialdensity(double R) const
{
    return exp(-R/_hinn) * pow( 1.0+exp(_s*(R-_Rb)/_hout) , _beta);
}

////////////////////////////////////////////////////////////////////

double BrokenExpDiskGeometry::density(double R, double z) const
{
    return _rho0 * exp(-abs(z)/_hz) * radialdensity(R);
}

////////////////////////////////////////////////////////////////////

double BrokenExpDiskGeometry::randomR() const
{
    return _random->cdf(_Rv,_Xv);
}

////////////////////////////////////////////////////////////////////

double BrokenExpDiskGeometry::randomz() const
{
    double z, XX;
    XX = _random->uniform();
    z = (XX<=0.5) ? _hz*log(2.0*XX) : -_hz*log(2.0*(1.0-XX));
    return z;
}

//////////////////////////////////////////////////////////////////////

double BrokenExpDiskGeometry::SigmaR() const
{
    return _SigmaR;
}

//////////////////////////////////////////////////////////////////////

double BrokenExpDiskGeometry::SigmaZ() const
{
    return 2.0 * _rho0 * _hz * radialdensity(0.0);
}

//////////////////////////////////////////////////////////////////////
