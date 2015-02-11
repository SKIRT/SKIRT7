/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "FatalError.hpp"
#include "Random.hpp"
#include "SphericalCavityGeometryDecorator.hpp"

using namespace std;

////////////////////////////////////////////////////////////////////

SphericalCavityGeometryDecorator::SphericalCavityGeometryDecorator()
    : _geometry(0), _radius(0), _centerX(0), _centerY(0), _centerZ(0)
{
}

////////////////////////////////////////////////////////////////////

void SphericalCavityGeometryDecorator::setupSelfBefore()
{
    Geometry::setupSelfBefore();

    // calculate some frequently used values
    _center = Position(_centerX,_centerY,_centerZ);
    _radius2 = _radius*_radius;
}

////////////////////////////////////////////////////////////////////

void SphericalCavityGeometryDecorator::setupSelfAfter()
{
    Geometry::setupSelfAfter();

    // estimate the original geometry's average density inside the cavity
    double rho = 0;
    int Nsamples = 100;
    int k = Nsamples;
    while (k--)
    {
        double X = _random->uniform();
        Position bfr(_center + pow(X,1./3.)*_radius*_random->direction());
        rho += _geometry->density(bfr);
    }
    rho /= Nsamples;

    // determine the normalization factor
    double cavitymass = rho * 4./3.*M_PI*_radius2*_radius;
    if (cavitymass > 0.99) throw FATALERROR("Cavity hides more than 99% of the original mass");
    _norm = 1. / (1. - cavitymass);
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
    return _geometry->density(bfr) * _norm;
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

double SphericalCavityGeometryDecorator::probabilityForDirection(Position bfr, Direction bfk) const
{
    return _geometry->probabilityForDirection(bfr, bfk);
}

////////////////////////////////////////////////////////////////////

Direction SphericalCavityGeometryDecorator::generateDirection(Position bfr) const
{
    return _geometry->generateDirection(bfr);
}

////////////////////////////////////////////////////////////////////
