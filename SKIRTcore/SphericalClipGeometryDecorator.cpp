/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "SphericalClipGeometryDecorator.hpp"

using namespace std;

////////////////////////////////////////////////////////////////////

SphericalClipGeometryDecorator::SphericalClipGeometryDecorator()
    : _radius(0), _centerX(0), _centerY(0), _centerZ(0)
{
}

////////////////////////////////////////////////////////////////////

void SphericalClipGeometryDecorator::setupSelfBefore()
{
    ClipGeometryDecorator::setupSelfBefore();

    // calculate some frequently used values
    _center = Position(_centerX,_centerY,_centerZ);
    _radius2 = _radius*_radius;
}

////////////////////////////////////////////////////////////////////

void SphericalClipGeometryDecorator::setRadius(double value)
{
    _radius = value;
}

////////////////////////////////////////////////////////////////////

double SphericalClipGeometryDecorator::radius() const
{
    return _radius;
}

////////////////////////////////////////////////////////////////////

void SphericalClipGeometryDecorator::setCenterX(double value)
{
    _centerX = value;
}

////////////////////////////////////////////////////////////////////

double SphericalClipGeometryDecorator::centerX() const
{
    return _centerX;
}

////////////////////////////////////////////////////////////////////

void SphericalClipGeometryDecorator::setCenterY(double value)
{
    _centerY = value;
}

////////////////////////////////////////////////////////////////////

double SphericalClipGeometryDecorator::centerY() const
{
    return _centerY;
}

////////////////////////////////////////////////////////////////////

void SphericalClipGeometryDecorator::setCenterZ(double value)
{
    _centerZ = value;
}

////////////////////////////////////////////////////////////////////

double SphericalClipGeometryDecorator::centerZ() const
{
    return _centerZ;
}

////////////////////////////////////////////////////////////////////

int SphericalClipGeometryDecorator::dimension() const
{
    int clipDimension = 1;
    if (_centerZ) clipDimension = 2;
    if (_centerX || _centerY) clipDimension = 3;
    return std::max(geometry()->dimension(), clipDimension);
}

////////////////////////////////////////////////////////////////////

bool SphericalClipGeometryDecorator::inside(Position bfr) const
{
    return (bfr-_center).norm2() <= _radius2;
}

////////////////////////////////////////////////////////////////////
