/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "FatalError.hpp"
#include "SpheroidalGeometryDecorator.hpp"
#include "Random.hpp"

////////////////////////////////////////////////////////////////////

SpheroidalGeometryDecorator::SpheroidalGeometryDecorator()
    : AxGeometry(),
      _geometry(0), _q(0)
{
}

//////////////////////////////////////////////////////////////////////

void
SpheroidalGeometryDecorator::setupSelfBefore()
{
    AxGeometry::setupSelfBefore();

    // verify property values
    if (_q <= 0 || _q > 1.0) throw FATALERROR("the flattening parameter q should be between 0 and 1");
}

////////////////////////////////////////////////////////////////////

void
SpheroidalGeometryDecorator::setGeometry(SpheGeometry* value)
{
    if (_geometry) delete _geometry;
    _geometry = value;
    if (_geometry) _geometry->setParent(this);
}

////////////////////////////////////////////////////////////////////

SpheGeometry*
SpheroidalGeometryDecorator::geometry()
const
{
    return _geometry;
}

//////////////////////////////////////////////////////////////////////

void
SpheroidalGeometryDecorator::setFlattening(double value)
{
    _q = value;
}

////////////////////////////////////////////////////////////////////

double
SpheroidalGeometryDecorator::flattening()
const
{
    return _q;
}

////////////////////////////////////////////////////////////////////

double
SpheroidalGeometryDecorator::density(double R, double z)
const
{
    double m = sqrt(R*R + z*z/(_q*_q));
    return 1.0/_q * _geometry->density(m);
}

////////////////////////////////////////////////////////////////////

Position
SpheroidalGeometryDecorator::generatePosition()
const
{
    Position bfrs = _geometry->generatePosition();
    double xs, ys, zs;
    bfrs.cartesian(xs,ys,zs);
    return Position(xs,ys,_q*zs);
}

////////////////////////////////////////////////////////////////////

double
SpheroidalGeometryDecorator::SigmaR()
const
{
    return 1.0/_q * _geometry->Sigmar();
}

////////////////////////////////////////////////////////////////////

double
SpheroidalGeometryDecorator::SigmaZ()
const
{
    return 2.0 * _geometry->Sigmar();
}

////////////////////////////////////////////////////////////////////
