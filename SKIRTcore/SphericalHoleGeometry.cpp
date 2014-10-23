/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "SphericalHoleGeometry.hpp"

////////////////////////////////////////////////////////////////////

SphericalHoleGeometry::SphericalHoleGeometry()
    : _geometry(0), _radius(0), _centerX(0), _centerY(0), _centerZ(0)
{
}

////////////////////////////////////////////////////////////////////

void SphericalHoleGeometry::setupSelfBefore()
{
    Geometry::setupSelfBefore();

    _center = Position(_centerX,_centerY,_centerZ);
    _radius2 = _radius*_radius;
}

////////////////////////////////////////////////////////////////////

void SphericalHoleGeometry::setGeometry(Geometry* value)
{
    if (_geometry) delete _geometry;
    _geometry = value;
    if (_geometry) _geometry->setParent(this);
}

////////////////////////////////////////////////////////////////////

Geometry* SphericalHoleGeometry::geometry() const
{
    return _geometry;
}

////////////////////////////////////////////////////////////////////

void SphericalHoleGeometry::setRadius(double value)
{
    _radius = value;
}

////////////////////////////////////////////////////////////////////

double SphericalHoleGeometry::radius() const
{
    return _radius;
}

////////////////////////////////////////////////////////////////////

void SphericalHoleGeometry::setCenterX(double value)
{
    _centerX = value;
}

////////////////////////////////////////////////////////////////////

double SphericalHoleGeometry::centerX() const
{
    return _centerX;
}

////////////////////////////////////////////////////////////////////

void SphericalHoleGeometry::setCenterY(double value)
{
    _centerY = value;
}

////////////////////////////////////////////////////////////////////

double SphericalHoleGeometry::centerY() const
{
    return _centerY;
}

////////////////////////////////////////////////////////////////////

void SphericalHoleGeometry::setCenterZ(double value)
{
    _centerZ = value;
}

////////////////////////////////////////////////////////////////////

double SphericalHoleGeometry::centerZ() const
{
    return _centerZ;
}

////////////////////////////////////////////////////////////////////

int SphericalHoleGeometry::dimension() const
{
    int holeDimension = 1;
    if (_centerZ) holeDimension = 2;
    if (_centerX || _centerY) holeDimension = 3;
    return qMax(_geometry->dimension(), holeDimension);
}

////////////////////////////////////////////////////////////////////

double SphericalHoleGeometry::density(Position bfr) const
{
    if ( (bfr-_center).norm2() <= _radius2 ) return 0;
    return _geometry->density(bfr);
}

////////////////////////////////////////////////////////////////////

Position SphericalHoleGeometry::generatePosition() const
{
    while (true)
    {
        Position bfr = _geometry->generatePosition();
        if ( (bfr-_center).norm2() > _radius2 ) return bfr;
    }
}

////////////////////////////////////////////////////////////////////

double SphericalHoleGeometry::SigmaX() const
{
    return _geometry->SigmaX();
}

////////////////////////////////////////////////////////////////////

double SphericalHoleGeometry::SigmaY() const
{
    return _geometry->SigmaY();
}

////////////////////////////////////////////////////////////////////

double SphericalHoleGeometry::SigmaZ() const
{
    return _geometry->SigmaZ();
}

////////////////////////////////////////////////////////////////////
