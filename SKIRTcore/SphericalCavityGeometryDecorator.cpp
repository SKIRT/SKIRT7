/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "SphericalCavityGeometryDecorator.hpp"

////////////////////////////////////////////////////////////////////

SphericalCavityGeometryDecorator::SphericalCavityGeometryDecorator()
    : _geometry(0), _radius(0), _centerX(0), _centerY(0), _centerZ(0)
{
}

////////////////////////////////////////////////////////////////////

void SphericalCavityGeometryDecorator::setupSelfBefore()
{
    Geometry::setupSelfBefore();

    _center = Position(_centerX,_centerY,_centerZ);
    _radius2 = _radius*_radius;
}

////////////////////////////////////////////////////////////////////

void SphericalCavityGeometryDecorator::setGeometry(Geometry* value)
{
    if (_geometry) delete _geometry;
    _geometry = value;
    if (_geometry) _geometry->setParent(this);
}

////////////////////////////////////////////////////////////////////

Geometry* SphericalCavityGeometryDecorator::geometry() const
{
    return _geometry;
}

////////////////////////////////////////////////////////////////////

void SphericalCavityGeometryDecorator::setRadius(double value)
{
    _radius = value;
}

////////////////////////////////////////////////////////////////////

double SphericalCavityGeometryDecorator::radius() const
{
    return _radius;
}

////////////////////////////////////////////////////////////////////

void SphericalCavityGeometryDecorator::setCenterX(double value)
{
    _centerX = value;
}

////////////////////////////////////////////////////////////////////

double SphericalCavityGeometryDecorator::centerX() const
{
    return _centerX;
}

////////////////////////////////////////////////////////////////////

void SphericalCavityGeometryDecorator::setCenterY(double value)
{
    _centerY = value;
}

////////////////////////////////////////////////////////////////////

double SphericalCavityGeometryDecorator::centerY() const
{
    return _centerY;
}

////////////////////////////////////////////////////////////////////

void SphericalCavityGeometryDecorator::setCenterZ(double value)
{
    _centerZ = value;
}

////////////////////////////////////////////////////////////////////

double SphericalCavityGeometryDecorator::centerZ() const
{
    return _centerZ;
}

////////////////////////////////////////////////////////////////////

int SphericalCavityGeometryDecorator::dimension() const
{
    int holeDimension = 1;
    if (_centerZ) holeDimension = 2;
    if (_centerX || _centerY) holeDimension = 3;
    return qMax(_geometry->dimension(), holeDimension);
}

////////////////////////////////////////////////////////////////////

double SphericalCavityGeometryDecorator::density(Position bfr) const
{
    if ( (bfr-_center).norm2() <= _radius2 ) return 0;
    return _geometry->density(bfr);
}

////////////////////////////////////////////////////////////////////

Position SphericalCavityGeometryDecorator::generatePosition() const
{
    while (true)
    {
        Position bfr = _geometry->generatePosition();
        if ( (bfr-_center).norm2() > _radius2 ) return bfr;
    }
}

////////////////////////////////////////////////////////////////////

double SphericalCavityGeometryDecorator::SigmaX() const
{
    return _geometry->SigmaX();
}

////////////////////////////////////////////////////////////////////

double SphericalCavityGeometryDecorator::SigmaY() const
{
    return _geometry->SigmaY();
}

////////////////////////////////////////////////////////////////////

double SphericalCavityGeometryDecorator::SigmaZ() const
{
    return _geometry->SigmaZ();
}

////////////////////////////////////////////////////////////////////
