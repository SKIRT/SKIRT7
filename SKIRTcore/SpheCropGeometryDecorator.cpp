/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "FatalError.hpp"
#include "Random.hpp"
#include "SpheCropGeometryDecorator.hpp"

using namespace std;

////////////////////////////////////////////////////////////////////

SpheCropGeometryDecorator::SpheCropGeometryDecorator()
    : _geometry(0), _radius(0), _centerX(0), _centerY(0), _centerZ(0)
{
}

////////////////////////////////////////////////////////////////////

void SpheCropGeometryDecorator::setupSelfBefore()
{
    Geometry::setupSelfBefore();

    // calculate some frequently used values
    _center = Position(_centerX,_centerY,_centerZ);
    _radius2 = _radius*_radius;
}

////////////////////////////////////////////////////////////////////

void SpheCropGeometryDecorator::setupSelfAfter()
{
    Geometry::setupSelfAfter();

    // estimate the original geometry's average density inside the sphere
    double rho = 0;
    int Nsamples = 1000;
    int k = Nsamples;
    while (k--)
    {
        double X = _random->uniform();
        Position bfr(_center + pow(X,1./3.)*_radius*_random->direction());
        rho += _geometry->density(bfr);
    }
    rho /= Nsamples;

    // determine the normalization factor
    double spheremass = rho * 4./3.*M_PI*_radius2*_radius;
    if (spheremass < 0.01) throw FATALERROR("Sphere crops more than 99% of the original mass");
    _norm = 1. / (spheremass);
}

////////////////////////////////////////////////////////////////////

void SpheCropGeometryDecorator::setGeometry(Geometry* value)
{
    if (_geometry) delete _geometry;
    _geometry = value;
    if (_geometry) _geometry->setParent(this);
}

////////////////////////////////////////////////////////////////////

Geometry* SpheCropGeometryDecorator::geometry() const
{
    return _geometry;
}

////////////////////////////////////////////////////////////////////

void SpheCropGeometryDecorator::setRadius(double value)
{
    _radius = value;
}

////////////////////////////////////////////////////////////////////

double SpheCropGeometryDecorator::radius() const
{
    return _radius;
}

////////////////////////////////////////////////////////////////////

void SpheCropGeometryDecorator::setCenterX(double value)
{
    _centerX = value;
}

////////////////////////////////////////////////////////////////////

double SpheCropGeometryDecorator::centerX() const
{
    return _centerX;
}

////////////////////////////////////////////////////////////////////

void SpheCropGeometryDecorator::setCenterY(double value)
{
    _centerY = value;
}

////////////////////////////////////////////////////////////////////

double SpheCropGeometryDecorator::centerY() const
{
    return _centerY;
}

////////////////////////////////////////////////////////////////////

void SpheCropGeometryDecorator::setCenterZ(double value)
{
    _centerZ = value;
}

////////////////////////////////////////////////////////////////////

double SpheCropGeometryDecorator::centerZ() const
{
    return _centerZ;
}

////////////////////////////////////////////////////////////////////

int SpheCropGeometryDecorator::dimension() const
{
    int sphereDimension = 1;
    if (_centerZ) sphereDimension = 2;
    if (_centerX || _centerY) sphereDimension = 3;
    return qMax(_geometry->dimension(), sphereDimension);
}

////////////////////////////////////////////////////////////////////

double SpheCropGeometryDecorator::density(Position bfr) const
{
    if ( (bfr-_center).norm2() > _radius2 ) return 0;
    return _geometry->density(bfr) * _norm;
}

////////////////////////////////////////////////////////////////////

Position SpheCropGeometryDecorator::generatePosition() const
{
    while (true)
    {
        Position bfr = _geometry->generatePosition();
        if ( (bfr-_center).norm2() <= _radius2 ) return bfr;
    }
}

////////////////////////////////////////////////////////////////////

double SpheCropGeometryDecorator::SigmaX() const
{
    return _geometry->SigmaX();
}

////////////////////////////////////////////////////////////////////

double SpheCropGeometryDecorator::SigmaY() const
{
    return _geometry->SigmaY();
}

////////////////////////////////////////////////////////////////////

double SpheCropGeometryDecorator::SigmaZ() const
{
    return _geometry->SigmaZ();
}

////////////////////////////////////////////////////////////////////

double SpheCropGeometryDecorator::probabilityForDirection(int ell, Position bfr, Direction bfk) const
{
    return _geometry->probabilityForDirection(ell, bfr, bfk);
}

////////////////////////////////////////////////////////////////////

Direction SpheCropGeometryDecorator::generateDirection(int ell, Position bfr) const
{
    return _geometry->generateDirection(ell, bfr);
}

////////////////////////////////////////////////////////////////////
