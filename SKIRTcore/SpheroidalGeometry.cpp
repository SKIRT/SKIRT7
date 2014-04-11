/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include "FatalError.hpp"
#include "SpheroidalGeometry.hpp"
#include "Random.hpp"

////////////////////////////////////////////////////////////////////

SpheroidalGeometry::SpheroidalGeometry()
    : AxGeometry(),
      _geometry(0), _q(0)
{
}

//////////////////////////////////////////////////////////////////////

void
SpheroidalGeometry::setupSelfBefore()
{
    AxGeometry::setupSelfBefore();

    // verify property values
    if (_q <= 0 || _q > 1.0) throw FATALERROR("the flattening parameter q should be between 0 and 1");
}

////////////////////////////////////////////////////////////////////

void
SpheroidalGeometry::setGeometry(SpheGeometry* value)
{
    if (_geometry) delete _geometry;
    _geometry = value;
    if (_geometry) _geometry->setParent(this);
}

////////////////////////////////////////////////////////////////////

SpheGeometry*
SpheroidalGeometry::geometry()
const
{
    return _geometry;
}

//////////////////////////////////////////////////////////////////////

void
SpheroidalGeometry::setFlattening(double value)
{
    _q = value;
}

////////////////////////////////////////////////////////////////////

double
SpheroidalGeometry::flattening()
const
{
    return _q;
}

////////////////////////////////////////////////////////////////////

double
SpheroidalGeometry::density(double R, double z)
const
{
    double m = sqrt(R*R + z*z/(_q*_q));
    return 1.0/_q * _geometry->density(m);
}

////////////////////////////////////////////////////////////////////

Position
SpheroidalGeometry::generatePosition()
const
{
    // method using spheroidal coordinates
    // x = m cos(phi) sin(tau)
    // y = m sin(phi) sin(tau)
    // z = q m cos(tau)

    double m = _geometry->randomradius();
    double phi = 2.0*M_PI*_random->uniform();
    double cosphi = cos(phi);
    double sinphi = sin(phi);
    double costau = 2.0*_random->uniform()-1.0;
    double sintau = sqrt((1.0-costau)*(1.0+costau));
    double x = m*cosphi*sintau;
    double y = m*sinphi*sintau;
    double z = _q*m*costau;
    return Position(x,y,z);

    // an alternative method using spherical coordinates

    /*
    double X = _random->uniform();
    double t = _q*(1.0-2.0*X);
    double theta = acos(t/sqrt(t*t+4.0*X*(1.0-X)));
    double costheta = cos(theta);
    double m = _geometry->randomradius();
    double r = m/sqrt(1.0+(1.0/_q/_q-1.0)*costheta*costheta);
    double phi = 2.0*M_PI*_random->uniform();
    return Position(r,theta,phi,Position::SPHERICAL);
    */
}

////////////////////////////////////////////////////////////////////

double
SpheroidalGeometry::SigmaR()
const
{
    return 1.0/_q * _geometry->Sigmar();
}

////////////////////////////////////////////////////////////////////

double
SpheroidalGeometry::SigmaZ()
const
{
    return 2.0 * _geometry->Sigmar();
}

////////////////////////////////////////////////////////////////////
