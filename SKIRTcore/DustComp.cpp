/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "DustComp.hpp"
#include "DustCompNormalization.hpp"
#include "Geometry.hpp"
#include "DustMix.hpp"
#include "FatalError.hpp"

using namespace std;

//////////////////////////////////////////////////////////////////////

DustComp::DustComp()
    : _geom(0), _mix(0), _norm(0), _nf(0)
{
}

//////////////////////////////////////////////////////////////////////

void DustComp::setupSelfBefore()
{
    SimulationItem::setupSelfBefore();

    // verify that we have all properties
    if (!_geom) throw FATALERROR("Geometry was not set");
    if (!_mix) throw FATALERROR("Dust mix was not set");
    if (!_norm) throw FATALERROR("Normalization was not set");
}

//////////////////////////////////////////////////////////////////////

void DustComp::setupSelfAfter()
{
    SimulationItem::setupSelfAfter();

    // calculate the normalization factor
    _nf = _norm->normalizationFactor(_geom, _mix);
}

//////////////////////////////////////////////////////////////////////

void DustComp::setGeometry(Geometry* value)
{
    if (_geom) delete _geom;
    _geom = value;
    if (_geom) _geom->setParent(this);
}

////////////////////////////////////////////////////////////////////

Geometry* DustComp::geometry() const
{
    return _geom;
}

//////////////////////////////////////////////////////////////////////

void DustComp::setMix(DustMix* value)
{
    if (_mix) delete _mix;
    _mix = value;
    if (_mix) _mix->setParent(this);
}

////////////////////////////////////////////////////////////////////

DustMix* DustComp::mix() const
{
    return _mix;
}

//////////////////////////////////////////////////////////////////////

void DustComp::setNormalization(DustCompNormalization* value)
{
    if (_norm) delete _norm;
    _norm = value;
    if (_norm) _norm->setParent(this);
}

////////////////////////////////////////////////////////////////////

DustCompNormalization* DustComp::normalization() const
{
    return _norm;
}

//////////////////////////////////////////////////////////////////////

int DustComp::dimension()
{
    return _geom->dimension();
}

//////////////////////////////////////////////////////////////////////

double
DustComp::density(Position bfr)
const
{
  return _nf * _geom->density(bfr);
}

//////////////////////////////////////////////////////////////////////

double
DustComp::mass()
const
{
  return _nf; // * _geom->totalmass(); in the new Geometry class, all masses are normalized to one !
}

//////////////////////////////////////////////////////////////////////

double
DustComp::SigmaX()
const
{
  return _nf * _geom->SigmaX();
}

//////////////////////////////////////////////////////////////////////

double
DustComp::SigmaY()
const
{
  return _nf * _geom->SigmaY();
}

//////////////////////////////////////////////////////////////////////

double
DustComp::SigmaZ()
const
{
  return _nf * _geom->SigmaZ();
}

//////////////////////////////////////////////////////////////////// */
