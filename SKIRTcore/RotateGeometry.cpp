/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "RotateGeometry.hpp"
#include "FatalError.hpp"

////////////////////////////////////////////////////////////////////

RotateGeometry::RotateGeometry()
    : _geometry(0),
      _alpha(0), _beta(0), _gamma(0),
      _sinalpha(0), _cosalpha(0),
      _sinbeta(0), _cosbeta(0),
      _singamma(0), _cosgamma(0),
      _R11(0), _R12(0), _R13(0),
      _R21(0), _R22(0), _R23(0),
      _R31(0), _R32(0), _R33(0)
{
}

//////////////////////////////////////////////////////////////////////

void
RotateGeometry::setupSelfBefore()
{
    GenGeometry::setupSelfBefore();

    // verify property values
    if (_alpha < 0 || _alpha > 2.0*M_PI) throw FATALERROR("The angle alpha should be between 0 and 360 degrees");
    if (_beta < 0 || _beta > M_PI) throw FATALERROR("The angle beta should be between 0 and 180 degrees");
    if (_gamma < 0 || _gamma > 2.0*M_PI) throw FATALERROR("The angle gamma should be between 0 and 360 degrees");

    // cache frequently used values
    _sinalpha = sin(_alpha);
    _cosalpha = cos(_alpha);
    _sinbeta = sin(_beta);
    _cosbeta = cos(_beta);
    _singamma = sin(_gamma);
    _cosgamma = cos(_gamma);
    _R11 = _cosalpha*_cosgamma - _sinalpha*_cosbeta*_singamma;
    _R12 = _sinalpha*_cosgamma + _cosalpha*_cosbeta*_singamma;
    _R13 = _cosbeta*_singamma;
    _R21 = -_cosalpha*_singamma - _sinalpha*_cosbeta*_cosgamma;
    _R22 = -_sinalpha*_singamma + _cosalpha*_cosbeta*_cosgamma;
    _R23 = _sinbeta*_cosgamma;
    _R31 = _sinalpha*_sinbeta;
    _R32 = -_cosalpha*_sinbeta;
    _R33 = _cosbeta;
}

////////////////////////////////////////////////////////////////////

void RotateGeometry::setGeometry(Geometry* value)
{
    if (_geometry) delete _geometry;
    _geometry = value;
    if (_geometry) _geometry->setParent(this);
}

////////////////////////////////////////////////////////////////////

Geometry* RotateGeometry::geometry() const
{
    return _geometry;
}

////////////////////////////////////////////////////////////////////

void RotateGeometry::setEuleralpha(double value)
{
    _alpha = value;
}

////////////////////////////////////////////////////////////////////

double RotateGeometry::euleralpha() const
{
    return _alpha;
}

////////////////////////////////////////////////////////////////////

void RotateGeometry::setEulerbeta(double value)
{
    _beta = value;
}

////////////////////////////////////////////////////////////////////

double RotateGeometry::eulerbeta() const
{
    return _beta;
}

////////////////////////////////////////////////////////////////////

void RotateGeometry::setEulergamma(double value)
{
    _gamma = value;
}

////////////////////////////////////////////////////////////////////

double RotateGeometry::eulergamma() const
{
    return _gamma;
}

////////////////////////////////////////////////////////////////////

double
RotateGeometry::density(Position bfr)
const
{
    Position bfrorig = derotate(bfr);
    return _geometry->density(bfrorig);
}

////////////////////////////////////////////////////////////////////

Position
RotateGeometry::generatePosition()
const
{
    Position bfrorig = _geometry->generatePosition();
    Position bfr = rotate(bfrorig);
    return bfr;
}

////////////////////////////////////////////////////////////////////

double
RotateGeometry::SigmaX()
const
{
    return _geometry->SigmaX();
}

////////////////////////////////////////////////////////////////////

double
RotateGeometry::SigmaY()
const
{
    return _geometry->SigmaY();
}

////////////////////////////////////////////////////////////////////

double
RotateGeometry::SigmaZ()
const
{
    return _geometry->SigmaZ();
}

////////////////////////////////////////////////////////////////////

double
RotateGeometry::probabilityForDirection(Position bfr, Direction bfk)
const
{
    Position bfrorig = derotate(bfr);
    Direction bfkorig = derotate(bfk);
    return _geometry->probabilityForDirection(bfrorig,bfkorig);
}

////////////////////////////////////////////////////////////////////

Direction
RotateGeometry::generateDirection(Position bfr)
const
{
    Position bfrorig = derotate(bfr);
    Direction bfkorig = _geometry->generateDirection(bfrorig);
    Direction bfk = rotate(bfkorig);
    return bfk;
}

////////////////////////////////////////////////////////////////////

Position
RotateGeometry::rotate(Position bfrorig)
const
{
    double xorig = bfrorig.x();
    double yorig = bfrorig.y();
    double zorig = bfrorig.z();
    double x = _R11*xorig + _R12*yorig + _R13*zorig;
    double y = _R21*xorig + _R22*yorig + _R23*zorig;
    double z = _R31*xorig + _R32*yorig + _R33*zorig;
    return Position(x,y,z);
}

////////////////////////////////////////////////////////////////////

Position
RotateGeometry::derotate(Position bfr)
const
{
    double x = bfr.x();
    double y = bfr.y();
    double z = bfr.z();
    double xorig = _R11*x + _R21*y + _R31*z;
    double yorig = _R12*x + _R22*y + _R32*z;
    double zorig = _R13*x + _R23*y + _R33*z;
    return Position(xorig,yorig,zorig);
}

////////////////////////////////////////////////////////////////////

Direction
RotateGeometry::rotate(Direction bfkorig)
const
{
    double kxorig = bfkorig.x();
    double kyorig = bfkorig.y();
    double kzorig = bfkorig.z();
    double kx = _R11*kxorig + _R12*kyorig + _R13*kzorig;
    double ky = _R21*kxorig + _R22*kyorig + _R23*kzorig;
    double kz = _R31*kxorig + _R32*kyorig + _R33*kzorig;
    return Direction(kx,ky,kz);
}

////////////////////////////////////////////////////////////////////

Direction
RotateGeometry::derotate(Direction bfk)
const
{
    double kx = bfk.x();
    double ky = bfk.y();
    double kz = bfk.z();
    double kxorig = _R11*kx + _R21*ky + _R31*kz;
    double kyorig = _R12*kx + _R22*ky + _R32*kz;
    double kzorig = _R13*kx + _R23*ky + _R33*kz;
    return Direction(kxorig,kyorig,kzorig);
}

////////////////////////////////////////////////////////////////////
