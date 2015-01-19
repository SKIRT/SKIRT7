/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "FatalError.hpp"
#include "TriaxialGeometryDecorator.hpp"
#include "Random.hpp"

////////////////////////////////////////////////////////////////////

TriaxialGeometryDecorator::TriaxialGeometryDecorator()
    : GenGeometry(),
      _geometry(0), _p(0), _q(0)
{
}

//////////////////////////////////////////////////////////////////////

void
TriaxialGeometryDecorator::setupSelfBefore()
{
    GenGeometry::setupSelfBefore();

    // verify property values
    if (_p <= 0 || _p > 1.0) throw FATALERROR("the flattening parameter p should be between 0 and 1");
    if (_q <= 0 || _q > 1.0) throw FATALERROR("the flattening parameter q should be between 0 and 1");
}

////////////////////////////////////////////////////////////////////

void
TriaxialGeometryDecorator::setGeometry(SpheGeometry* value)
{
    if (_geometry) delete _geometry;
    _geometry = value;
    if (_geometry) _geometry->setParent(this);
}

////////////////////////////////////////////////////////////////////

SpheGeometry*
TriaxialGeometryDecorator::geometry()
const
{
    return _geometry;
}

//////////////////////////////////////////////////////////////////////

void
TriaxialGeometryDecorator::setYFlattening(double value)
{
    _p = value;
}

////////////////////////////////////////////////////////////////////

double
TriaxialGeometryDecorator::yFlattening()
const
{
    return _p;
}

//////////////////////////////////////////////////////////////////////

void
TriaxialGeometryDecorator::setZFlattening(double value)
{
    _q = value;
}

////////////////////////////////////////////////////////////////////

double
TriaxialGeometryDecorator::zFlattening()
const
{
    return _q;
}

////////////////////////////////////////////////////////////////////

double
TriaxialGeometryDecorator::density(Position bfr)
const
{
    double x, y, z;
    bfr.cartesian(x,y,z);
    Position bfrs(x,y/_p,z/_q);
    return 1.0/_p/_q * _geometry->density(bfrs);
}

////////////////////////////////////////////////////////////////////

Position
TriaxialGeometryDecorator::generatePosition()
const
{
    Position bfrs = _geometry->generatePosition();
    double xs, ys, zs;
    bfrs.cartesian(xs,ys,zs);
    return Position(xs,_p*ys,_q*zs);
}

////////////////////////////////////////////////////////////////////

double
TriaxialGeometryDecorator::SigmaX()
const
{
    return 2.0/(_p*_q) * _geometry->Sigmar();
}

////////////////////////////////////////////////////////////////////

double
TriaxialGeometryDecorator::SigmaY()
const
{
    return 2.0/_q * _geometry->Sigmar();
}

////////////////////////////////////////////////////////////////////

double
TriaxialGeometryDecorator::SigmaZ()
const
{
    return 2.0/_p * _geometry->Sigmar();
}

////////////////////////////////////////////////////////////////////
