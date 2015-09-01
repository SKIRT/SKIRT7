/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "OffsetGeometryDecorator.hpp"

////////////////////////////////////////////////////////////////////

OffsetGeometryDecorator::OffsetGeometryDecorator()
    : _geometry(0), _offsetX(0), _offsetY(0), _offsetZ(0)
{
}

////////////////////////////////////////////////////////////////////

void OffsetGeometryDecorator::setGeometry(Geometry* value)
{
    if (_geometry) delete _geometry;
    _geometry = value;
    if (_geometry) _geometry->setParent(this);
}

////////////////////////////////////////////////////////////////////

Geometry* OffsetGeometryDecorator::geometry() const
{
    return _geometry;
}

////////////////////////////////////////////////////////////////////

void OffsetGeometryDecorator::setOffsetX(double value)
{
    _offsetX = value;
}

////////////////////////////////////////////////////////////////////

double OffsetGeometryDecorator::offsetX() const
{
    return _offsetX;
}

////////////////////////////////////////////////////////////////////

void OffsetGeometryDecorator::setOffsetY(double value)
{
    _offsetY = value;
}

////////////////////////////////////////////////////////////////////

double OffsetGeometryDecorator::offsetY() const
{
    return _offsetY;
}

////////////////////////////////////////////////////////////////////

void OffsetGeometryDecorator::setOffsetZ(double value)
{
    _offsetZ = value;
}

////////////////////////////////////////////////////////////////////

double OffsetGeometryDecorator::offsetZ() const
{
    return _offsetZ;
}

////////////////////////////////////////////////////////////////////

int OffsetGeometryDecorator::dimension() const
{
    return (_offsetX || _offsetY || _geometry->dimension()==3) ? 3 : 2;
}

////////////////////////////////////////////////////////////////////

double
OffsetGeometryDecorator::density(Position bfr)
const
{
    double x,y,z;
    bfr.cartesian(x,y,z);
    return _geometry->density(Position(x-_offsetX, y-_offsetY, z-_offsetZ));
}

////////////////////////////////////////////////////////////////////

Position
OffsetGeometryDecorator::generatePosition()
const
{
    Position bfr = _geometry->generatePosition();
    double x,y,z;
    bfr.cartesian(x,y,z);
    return Position(x+_offsetX, y+_offsetY, z+_offsetZ);
}

////////////////////////////////////////////////////////////////////

double
OffsetGeometryDecorator::SigmaX()
const
{
    return _geometry->SigmaX();
}

////////////////////////////////////////////////////////////////////

double
OffsetGeometryDecorator::SigmaY()
const
{
    return _geometry->SigmaY();
}

////////////////////////////////////////////////////////////////////

double
OffsetGeometryDecorator::SigmaZ()
const
{
    return _geometry->SigmaZ();
}

////////////////////////////////////////////////////////////////////

double
OffsetGeometryDecorator::probabilityForDirection(int ell, Position bfr, Direction bfk)
const
{
    double x,y,z;
    bfr.cartesian(x,y,z);
    return _geometry->probabilityForDirection(ell, Position(x-_offsetX, y-_offsetY, z-_offsetZ),bfk);
}

////////////////////////////////////////////////////////////////////

Direction
OffsetGeometryDecorator::generateDirection(int ell, Position bfr)
const
{
    double x,y,z;
    bfr.cartesian(x,y,z);
    return _geometry->generateDirection(ell, Position(x-_offsetX, y-_offsetY, z-_offsetZ));
}

////////////////////////////////////////////////////////////////////
