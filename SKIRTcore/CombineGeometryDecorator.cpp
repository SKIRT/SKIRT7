////////////////////////////////////////////////////////////////////

#include "FatalError.hpp"
#include "NR.hpp"
#include "Random.hpp"
#include "CombineGeometryDecorator.hpp"

using namespace std;

//////////////////////////////////////////////////////////////////////

CombineGeometryDecorator::CombineGeometryDecorator()
    : _geometry1(0), _geometry2(0), _w1(0), _w2(0), _random(0)
{
}

////////////////////////////////////////////////////////////////////

void
CombineGeometryDecorator::setFirstGeometry(Geometry* value)
{
    if (_geometry1) delete _geometry1;
    _geometry1 = value;
    if (_geometry1) _geometry1->setParent(this);
}

////////////////////////////////////////////////////////////////////

Geometry*
CombineGeometryDecorator::firstGeometry()
const
{
    return _geometry1;
}

////////////////////////////////////////////////////////////////////

void
CombineGeometryDecorator::setFirstWeight(double value)
{
    _w1 = value;
}

////////////////////////////////////////////////////////////////////

double
CombineGeometryDecorator::firstWeight()
const
{
    return _w1;
}

////////////////////////////////////////////////////////////////////

void
CombineGeometryDecorator::setSecondGeometry(Geometry* value)
{
    if (_geometry2) delete _geometry2;
    _geometry2 = value;
    if (_geometry2) _geometry2->setParent(this);
}

////////////////////////////////////////////////////////////////////

Geometry*
CombineGeometryDecorator::secondGeometry()
const
{
    return _geometry2;
}

////////////////////////////////////////////////////////////////////

void
CombineGeometryDecorator::setSecondWeight(double value)
{
    _w2 = value;
}

////////////////////////////////////////////////////////////////////

double
CombineGeometryDecorator::secondWeight()
const
{
    return _w2;
}

//////////////////////////////////////////////////////////////////////

void
CombineGeometryDecorator::setupSelfBefore()
{
    Geometry::setupSelfBefore();
    _random = find<Random>();
}

//////////////////////////////////////////////////////////////////////

void
CombineGeometryDecorator::setupSelfAfter()
{
    Geometry::setupSelfAfter();
    double sum = _w1+_w2;
    _w1 /= sum;
    _w2 /= sum;
}

//////////////////////////////////////////////////////////////////////

int
CombineGeometryDecorator::dimension()
const
{
    return max(_geometry1->dimension(),_geometry2->dimension());
}

////////////////////////////////////////////////////////////////////

double
CombineGeometryDecorator::density(Position bfr)
const
{
    return _w1*_geometry1->density(bfr) + _w2*_geometry2->density(bfr);
}

////////////////////////////////////////////////////////////////////

Position
CombineGeometryDecorator::generatePosition()
const
{
    double X = _random->uniform();
    if (X<_w1)
        return _geometry1->generatePosition();
    else
        return _geometry2->generatePosition();
}

////////////////////////////////////////////////////////////////////

double
CombineGeometryDecorator::SigmaX()
const
{
    return _w1*_geometry1->SigmaX() + _w2*_geometry2->SigmaX();
}

////////////////////////////////////////////////////////////////////

double
CombineGeometryDecorator::SigmaY()
const
{
    return _w1*_geometry1->SigmaY() + _w2*_geometry2->SigmaY();
}

////////////////////////////////////////////////////////////////////

double
CombineGeometryDecorator::SigmaZ()
const
{
    return _w1*_geometry1->SigmaZ() + _w2*_geometry2->SigmaZ();
}

////////////////////////////////////////////////////////////////////

double
CombineGeometryDecorator::probabilityForDirection(int ell, Position bfr, Direction bfk)
const
{
    return _w1*_geometry1->probabilityForDirection(ell,bfr,bfk)
            + _w2*_geometry2->probabilityForDirection(ell,bfr,bfk);
}

////////////////////////////////////////////////////////////////////

Direction
CombineGeometryDecorator::generateDirection(int ell, Position bfr)
const
{
    double X = _random->uniform();
    if (X<_w1)
        return _geometry1->generateDirection(ell,bfr);
    else
        return _geometry2->generateDirection(ell,bfr);
}

////////////////////////////////////////////////////////////////////
