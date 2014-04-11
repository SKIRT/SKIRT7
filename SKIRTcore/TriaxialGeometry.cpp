/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include "FatalError.hpp"
#include "TriaxialGeometry.hpp"
#include "Random.hpp"

////////////////////////////////////////////////////////////////////

TriaxialGeometry::TriaxialGeometry()
    : GenGeometry(),
      _geometry(0), _p(0), _q(0)
{
}

//////////////////////////////////////////////////////////////////////

void
TriaxialGeometry::setupSelfBefore()
{
    GenGeometry::setupSelfBefore();

    // verify property values
    if (_p <= 0 || _p > 1.0) throw FATALERROR("the flattening parameter p should be between 0 and 1");
    if (_q <= 0 || _q > 1.0) throw FATALERROR("the flattening parameter q should be between 0 and 1");
}

////////////////////////////////////////////////////////////////////

void
TriaxialGeometry::setGeometry(SpheGeometry* value)
{
    if (_geometry) delete _geometry;
    _geometry = value;
    if (_geometry) _geometry->setParent(this);
}

////////////////////////////////////////////////////////////////////

SpheGeometry*
TriaxialGeometry::geometry()
const
{
    return _geometry;
}

//////////////////////////////////////////////////////////////////////

void
TriaxialGeometry::setYFlattening(double value)
{
    _p = value;
}

////////////////////////////////////////////////////////////////////

double
TriaxialGeometry::yFlattening()
const
{
    return _p;
}

//////////////////////////////////////////////////////////////////////

void
TriaxialGeometry::setZFlattening(double value)
{
    _q = value;
}

////////////////////////////////////////////////////////////////////

double
TriaxialGeometry::zFlattening()
const
{
    return _q;
}

////////////////////////////////////////////////////////////////////

double
TriaxialGeometry::density(Position bfr)
const
{
    double x, y, z;
    bfr.cartesian(x,y,z);
    double m = sqrt(x*x + y*y/(_p*_p) + z*z/(_q*_q));
    return 1.0/_p/_q * _geometry->density(m);
}

////////////////////////////////////////////////////////////////////

Position
TriaxialGeometry::generatePosition()
const
{
    // method using ellipsoidal coordinates
    // x = m cos(sigma) sin(tau)
    // y = p m sin(sigma) sin(tau)
    // z = q m cos(tau)

    double m = _geometry->randomradius();
    double sigma = 2.0*M_PI*_random->uniform();
    double cossigma = cos(sigma);
    double sinsigma = sin(sigma);
    double costau = 2.0*_random->uniform()-1.0;
    double sintau = sqrt((1.0-costau)*(1.0+costau));
    double x = m*cossigma*sintau;
    double y = _p*m*sinsigma*sintau;
    double z = _q*m*costau;
    return Position(x,y,z);

    // an alternative method using spherical coordinates

    /*
    double X = _random->uniform();
    double phi = atan(_p*tan(2.0*M_PI*X));
    if (X>0.75)
        phi += 2.0*M_PI;
    else if (X>0.25)
        phi += M_PI;
    double cosphi = cos(phi);
    double sinphi = sin(phi);
    double A2 = cosphi*cosphi+sinphi*sinphi/_p/_p;
    double A = sqrt(A2);
    X = _random->uniform();
    double t = _q*A*(1.0-2.0*X);
    double theta = acos(t/sqrt(t*t+4.0*X*(1.0-X)));
    double costheta = cos(theta);
    double sintheta = sin(theta);
    double m = _geometry->randomradius();
    double r = m/sqrt(A2*sintheta*sintheta + costheta*costheta/_q/_q);
    return Position(r,theta,phi,Position::SPHERICAL);
    */
}

////////////////////////////////////////////////////////////////////

double
TriaxialGeometry::SigmaX()
const
{
    return 2.0/(_p*_q) * _geometry->Sigmar();
}

////////////////////////////////////////////////////////////////////

double
TriaxialGeometry::SigmaY()
const
{
    return 2.0/_q * _geometry->Sigmar();
}

////////////////////////////////////////////////////////////////////

double
TriaxialGeometry::SigmaZ()
const
{
    return 2.0/_p * _geometry->Sigmar();
}

////////////////////////////////////////////////////////////////////
