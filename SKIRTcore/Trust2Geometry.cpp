/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include <cmath>
#include "Trust2Geometry.hpp"
#include "Random.hpp"
#include "Units.hpp"

using namespace std;

//////////////////////////////////////////////////////////////////////

Trust2Geometry::Trust2Geometry()
    : _M0(0), _L0(0), _R0(0), _rho0(0),
      _M1(0), _R1(0), _rho1(0), _bfr1(),
      _M2(0), _R2(0), _rho2(0), _bfr2()
{
}

//////////////////////////////////////////////////////////////////////

void Trust2Geometry::setupSelfBefore()
{
    GenGeometry::setupSelfBefore();

    // set property values
    const double AU = Units::AU();

    _M0 = 0.00261592496620725;
    _L0 = 30.0 * AU;
    _R0 = 15.0 * AU;
    _rho0 = _M0 / (8.0*_L0*_L0*_L0-M_PI/6.0*_R0*_R0*_R0);

    _M1 = 0.383609259628382;
    _bfr1 = Position(-20.0*AU,-15.0*AU,-10.0*AU);
    _R1 = 5.0*AU;
    _rho1 = _M1 / (4.0*M_PI/3.0*_R1*_R1*_R1);

    _M2 = 0.613774815405411;
    _bfr2 = Position(-10.0/3.0*AU,5.0/3.0*AU,-5.0/3.0*AU);
    _R2 = 20.0 * AU;
    _rho2 = _M2 / (4.0*M_PI/3.0*_R2*_R2*_R2);
}

//////////////////////////////////////////////////////////////////////

double Trust2Geometry::density(Position bfr) const
{
    double x, y, z;
    bfr.cartesian(x,y,z);
    if (x<-_L0 || x>_L0 || y<-_L0 || y>_L0 || z<-_L0 || z>_L0) return 0.0;
    double dd0 = (x+_L0)*(x+_L0) + (y+_L0)*(y+_L0) + (z+_L0)*(z+_L0);
    if (dd0<_R0*_R0) return 0.0;
    double rho = _rho0;
    double x1, y1, z1;
    _bfr1.cartesian(x1,y1,z1);
    double dd1 = (x-x1)*(x-x1) + (y-y1)*(y-y1) + (z-z1)*(z-z1);
    if (dd1<_R1*_R1) rho += _rho1;
    double x2, y2, z2;
    _bfr2.cartesian(x2,y2,z2);
    double dd2 = (x-x2)*(x-x2) + (y-y2)*(y-y2) + (z-z2)*(z-z2);
    if (dd2<_R2*_R2) rho += _rho2;
    return rho;
}

//////////////////////////////////////////////////////////////////////

Position Trust2Geometry::generatePosition() const
{
    double X = _random->uniform();
    if (X<_M0)
    {
        while (true)
        {
            double x = (2.0*_random->uniform()-1.0) * _L0;
            double y = (2.0*_random->uniform()-1.0) * _L0;
            double z = (2.0*_random->uniform()-1.0) * _L0;
            double dd0 = (x+_L0)*(x+_L0) + (y+_L0)*(y+_L0) + (z+_L0)*(z+_L0);
            if (dd0>=_R0*_R0) return Position(x,y,z);
        }
    }
    else if (X<_M0+_M1)
    {
        Direction bfk = _random->direction();
        double r = _R1*pow(_random->uniform(),1.0/3.0);
        return Position(_bfr1+r*bfk);
    }
    else
    {
        Direction bfk = _random->direction();
        double r = _R2*pow(_random->uniform(),1.0/3.0);
        return Position(_bfr2+r*bfk);
    }
}

//////////////////////////////////////////////////////////////////////

double Trust2Geometry::SigmaX() const
{
    double x2, y2, z2;
    _bfr2.cartesian(x2,y2,z2);
    double Sigma0 = 2.0*_L0*_rho0;
    double Sigma1 = 0.0;
    double Sigma2 = 2.0*sqrt(_R2*_R2-y2*y2-z2*z2)*_rho2;
    return Sigma0 + Sigma1 + Sigma2;
}

//////////////////////////////////////////////////////////////////////

double Trust2Geometry::SigmaY() const
{
    double x2, y2, z2;
    _bfr2.cartesian(x2,y2,z2);
    double Sigma0 = 2.0*_L0*_rho0;
    double Sigma1 = 0.0;
    double Sigma2 = 2.0*sqrt(_R2*_R2-x2*x2-z2*z2)*_rho2;
    return Sigma0 + Sigma1 + Sigma2;
}

//////////////////////////////////////////////////////////////////////

double Trust2Geometry::SigmaZ() const
{
    double x2, y2, z2;
    _bfr2.cartesian(x2,y2,z2);
    double Sigma0 = 2.0*_L0*_rho0;
    double Sigma1 = 0.0;
    double Sigma2 = 2.0*sqrt(_R2*_R2-x2*x2-y2*y2)*_rho2;
    return Sigma0 + Sigma1 + Sigma2;
}

//////////////////////////////////////////////////////////////////////
