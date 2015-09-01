/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "FatalError.hpp"
#include "Random.hpp"
#include "CylindricalCavityGeometryDecorator.hpp"

using namespace std;

////////////////////////////////////////////////////////////////////

CylindricalCavityGeometryDecorator::CylindricalCavityGeometryDecorator()
    : _geometry(0), _radius(0)
{
}

////////////////////////////////////////////////////////////////////

void
CylindricalCavityGeometryDecorator::setupSelfAfter()
{
    Geometry::setupSelfAfter();

    // estimate the original geometry's mass inside the cylinder

    double volume = M_PI*_radius*_radius*_radius;
    int Nsamples = 200;
    double M = 0;

    // first estimate the mass inside a cylinder with radius _radius and total height _radius

    int k = Nsamples;
    while (k--)
    {
        double R = _radius*sqrt(_random->uniform());
        double phi = 2.0*M_PI*_random->uniform();
        double z = (_random->uniform()-0.5)*_radius;
        Position bfr(R,phi,z,Position::CYLINDRICAL);
        M += _geometry->density(bfr) / Nsamples * volume;
    }

    // add the mass in cylinders above and below the previous ones, until a convergence of 2% is
    // achieved.

    int Nmax = 200;
    for (int i=1; i<=Nmax; i++)
    {
        double dM = 0.0;
        k = Nsamples;
        while (k--)
        {
            double R = _radius*sqrt(_random->uniform());
            double phi = 2.0*M_PI*_random->uniform();
            double z = (i+_random->uniform()-0.5)*_radius;
            Position bfr(R,phi,z,Position::CYLINDRICAL);
            dM += _geometry->density(bfr) / Nsamples * volume;
        }
        k = Nsamples;
        while (k--)
        {
            double R = _radius*sqrt(_random->uniform());
            double phi = 2.0*M_PI*_random->uniform();
            double z = (-i+_random->uniform()-0.5)*_radius;
            Position bfr(R,phi,z,Position::CYLINDRICAL);
            dM += _geometry->density(bfr) / Nsamples * volume;
        }
        double Mprev = M;
        M = Mprev + dM;
        if (dM/Mprev < 0.02) break;
    }

    // determine the normalization factor
    _norm = 1.0 / (1.0 - M);
}

////////////////////////////////////////////////////////////////////

void
CylindricalCavityGeometryDecorator::setGeometry(Geometry* value)
{
    if (_geometry) delete _geometry;
    _geometry = value;
    if (_geometry) _geometry->setParent(this);
}

////////////////////////////////////////////////////////////////////

Geometry*
CylindricalCavityGeometryDecorator::geometry()
const
{
    return _geometry;
}

////////////////////////////////////////////////////////////////////

void
CylindricalCavityGeometryDecorator::setRadius(double value)
{
    _radius = value;
}

////////////////////////////////////////////////////////////////////

double
CylindricalCavityGeometryDecorator::radius()
const
{
    return _radius;
}

////////////////////////////////////////////////////////////////////

int
CylindricalCavityGeometryDecorator::dimension()
const
{
    return qMax(_geometry->dimension(), 2);
}

////////////////////////////////////////////////////////////////////

double
CylindricalCavityGeometryDecorator::density(Position bfr)
const
{
    if (bfr.cylradius()<_radius) return 0;
    return _geometry->density(bfr) * _norm;
}

////////////////////////////////////////////////////////////////////

Position
CylindricalCavityGeometryDecorator::generatePosition()
const
{
    while (true)
    {
        Position bfr = _geometry->generatePosition();
        if (bfr.cylradius()>_radius) return bfr;
    }
}

////////////////////////////////////////////////////////////////////

double
CylindricalCavityGeometryDecorator::SigmaX()
const
{
    return _geometry->SigmaX() * _norm;
}

////////////////////////////////////////////////////////////////////

double
CylindricalCavityGeometryDecorator::SigmaY()
const
{
    return _geometry->SigmaY() * _norm;
}

////////////////////////////////////////////////////////////////////

double
CylindricalCavityGeometryDecorator::SigmaZ()
const
{
    return 0.0;
}

////////////////////////////////////////////////////////////////////

double
CylindricalCavityGeometryDecorator::probabilityForDirection(int ell, Position bfr, Direction bfk)
const
{
    return _geometry->probabilityForDirection(ell, bfr, bfk);
}

////////////////////////////////////////////////////////////////////

Direction
CylindricalCavityGeometryDecorator::generateDirection(int ell, Position bfr)
const
{
    return _geometry->generateDirection(ell, bfr);
}

////////////////////////////////////////////////////////////////////
