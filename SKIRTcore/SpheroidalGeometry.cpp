/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "FatalError.hpp"
#include "SpheroidalGeometry.hpp"
#include "Random.hpp"

////////////////////////////////////////////////////////////////////

SpheroidalGeometry::SpheroidalGeometry()
    : AxGeometry(),
      _geometry(0), _q(0)
{
}

//////////////////////////////////////////////////////////////////////

void
SpheroidalGeometry::setupSelfBefore()
{
    AxGeometry::setupSelfBefore();

    // verify property values
    if (_q <= 0 || _q > 1.0) throw FATALERROR("the flattening parameter q should be between 0 and 1");
}

////////////////////////////////////////////////////////////////////

void
SpheroidalGeometry::setGeometry(SpheGeometry* value)
{
    if (_geometry) delete _geometry;
    _geometry = value;
    if (_geometry) _geometry->setParent(this);
}

////////////////////////////////////////////////////////////////////

SpheGeometry*
SpheroidalGeometry::geometry()
const
{
    return _geometry;
}

//////////////////////////////////////////////////////////////////////

void
SpheroidalGeometry::setFlattening(double value)
{
    _q = value;
}

////////////////////////////////////////////////////////////////////

double
SpheroidalGeometry::flattening()
const
{
    return _q;
}

////////////////////////////////////////////////////////////////////

double
SpheroidalGeometry::density(double R, double z)
const
{
    double m = sqrt(R*R + z*z/(_q*_q));
    return 1.0/_q * _geometry->density(m);
}

////////////////////////////////////////////////////////////////////

Position
SpheroidalGeometry::generatePosition()
const
{
    Position bfrs = _geometry->generatePosition();
    double xs, ys, zs;
    bfrs.cartesian(xs,ys,zs);
    return Position(xs,ys,_q*zs);
}

////////////////////////////////////////////////////////////////////

double
SpheroidalGeometry::SigmaR()
const
{
    return 1.0/_q * _geometry->Sigmar();
}

////////////////////////////////////////////////////////////////////

double
SpheroidalGeometry::SigmaZ()
const
{
    return 2.0 * _geometry->Sigmar();
}

////////////////////////////////////////////////////////////////////
