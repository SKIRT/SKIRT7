/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "FatalError.hpp"
#include "NR.hpp"
#include "Random.hpp"
#include "SpecialFunctions.hpp"
#include "SpiralStructureGeometryDecorator.hpp"

////////////////////////////////////////////////////////////////////

SpiralStructureGeometryDecorator::SpiralStructureGeometryDecorator()
    : GenGeometry(),
      _geometry(0),
      _m(0), _p(0), _R0(0), _phi0(0), _w(0), _N(0), _tanp(0), _CN(0), _c(0)
{
}

//////////////////////////////////////////////////////////////////////

void
SpiralStructureGeometryDecorator::setupSelfBefore()
{
    GenGeometry::setupSelfBefore();

    // verify property values
    if (_m <= 0) throw FATALERROR("The number of spiral arms should be positive");
    if (_p <= 0 || _p >= M_PI/2.) throw FATALERROR("The pitch angle should be between 0 and 90 degrees");
    if (_R0 <= 0) throw FATALERROR("The radius zero-point should be positive");
    if (_phi0 < 0 || _phi0 > 2.0*M_PI) throw FATALERROR("The phase zero-point should be between 0 and 360 degrees");
    if (_w <= 0 || _w > 1.) throw FATALERROR("The weight of the spiral perturbation should be between 0 and 1");
    if (_N < 0 || _N > 10) throw FATALERROR("The arm-interarm size ratio index should be between 0 and 10");

    // cache frequently used values
    _tanp = tan(_p);
    _CN = sqrt(M_PI) * SpecialFunctions::gamma(_N+1.0) / SpecialFunctions::gamma(_N+0.5);
    _c = 1.0+(_CN-1.0)*_w;
}

////////////////////////////////////////////////////////////////////

void
SpiralStructureGeometryDecorator::setGeometry(AxGeometry* value)
{
    if (_geometry) delete _geometry;
    _geometry = value;
    if (_geometry) _geometry->setParent(this);
}

////////////////////////////////////////////////////////////////////

AxGeometry*
SpiralStructureGeometryDecorator::geometry()
const
{
    return _geometry;
}

////////////////////////////////////////////////////////////////////

void
SpiralStructureGeometryDecorator::setArms(int value)
{
    _m = value;
}

////////////////////////////////////////////////////////////////////

int
SpiralStructureGeometryDecorator::arms()
const
{
    return _m;
}

////////////////////////////////////////////////////////////////////

void
SpiralStructureGeometryDecorator::setPitch(double value)
{
    _p = value;
}

////////////////////////////////////////////////////////////////////

double
SpiralStructureGeometryDecorator::pitch()
const
{
    return _p;
}

////////////////////////////////////////////////////////////////////

void
SpiralStructureGeometryDecorator::setRadius(double value)
{
    _R0 = value;
}

////////////////////////////////////////////////////////////////////

double
SpiralStructureGeometryDecorator::radius()
const
{
    return _R0;
}

////////////////////////////////////////////////////////////////////

void
SpiralStructureGeometryDecorator::setPhase(double value)
{
    _phi0 = value;
}

////////////////////////////////////////////////////////////////////

double
SpiralStructureGeometryDecorator::phase()
const
{
    return _phi0;
}

////////////////////////////////////////////////////////////////////

void
SpiralStructureGeometryDecorator::setPerturbWeight(double value)
{
    _w = value;
}

////////////////////////////////////////////////////////////////////

double
SpiralStructureGeometryDecorator::perturbWeight()
const
{
    return _w;
}

////////////////////////////////////////////////////////////////////

void
SpiralStructureGeometryDecorator::setIndex(int value)
{
    _N = value;
}

////////////////////////////////////////////////////////////////////

int
SpiralStructureGeometryDecorator::index()
const
{
    return _N;
}

////////////////////////////////////////////////////////////////////

double
SpiralStructureGeometryDecorator::density(Position bfr)
const
{
    double R, phi, z;
    bfr.cylindrical(R,phi,z);
    return _geometry->density(R,z) * perturbation(R,phi);
}

////////////////////////////////////////////////////////////////////

Position
SpiralStructureGeometryDecorator::generatePosition()
const
{
    Position bfr = _geometry->generatePosition();
    double R, dummyphi, z;
    bfr.cylindrical(R,dummyphi,z);
    double c = 1.0+(_CN-1.0)*_w;
    double phi, t;
    do
    {
        phi = 2.0*M_PI*_random->uniform();
        t = _random->uniform()*c/perturbation(R,phi);
    }
    while (t>1);
    return Position(R,phi,z,Position::CYLINDRICAL);
}

////////////////////////////////////////////////////////////////////

double
SpiralStructureGeometryDecorator::SigmaX()
const
{
    return _geometry->SigmaX();
}

////////////////////////////////////////////////////////////////////

double
SpiralStructureGeometryDecorator::SigmaY()
const
{
    return _geometry->SigmaY();
}

////////////////////////////////////////////////////////////////////

double
SpiralStructureGeometryDecorator::SigmaZ()
const
{
    return _geometry->SigmaZ();
}

////////////////////////////////////////////////////////////////////

double
SpiralStructureGeometryDecorator::perturbation(double R, double phi)
const
{
    double gamma = log(R/_R0)/_tanp + _phi0 + 0.5*M_PI/_m;
    return (1.0-_w) + _w*_CN*pow(sin(0.5*_m*(gamma-phi)),2*_N);
}

////////////////////////////////////////////////////////////////////
