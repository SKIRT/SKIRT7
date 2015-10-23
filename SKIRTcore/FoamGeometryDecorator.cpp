/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include <cmath>
#include "FatalError.hpp"
#include "Foam.hpp"
#include "FoamGeometryDecorator.hpp"
#include "Log.hpp"
#include "Random.hpp"

using namespace std;

///////////////////////////////////////////////////////////////////////////////

FoamGeometryDecorator::FoamGeometryDecorator()
    : _geometry(0), _Ncells(0), _jacobian(0), _foam(0)
{
}

///////////////////////////////////////////////////////////////////////////////

FoamGeometryDecorator::~FoamGeometryDecorator()
{
    delete _foam;
}

//////////////////////////////////////////////////////////////////////

void FoamGeometryDecorator::setupSelfBefore()
{
    BoxGeometry::setupSelfBefore();
    if (_Ncells < 1000) throw FATALERROR("The number of foam cells should be at least 1000");
    if (_Ncells > 1000000) throw FATALERROR("The number of foam cells should be at most 1000000");
    _jacobian = volume();
}

//////////////////////////////////////////////////////////////////////

void FoamGeometryDecorator::setupSelfAfter()
{
    BoxGeometry::setupSelfAfter();
    _foam = Foam::createFoam(find<Log>(), _random, this, 3, _Ncells);
}

////////////////////////////////////////////////////////////////////

void FoamGeometryDecorator::setGeometry(Geometry* value)
{
    if (_geometry) delete _geometry;
    _geometry = value;
    if (_geometry) _geometry->setParent(this);
}

////////////////////////////////////////////////////////////////////

Geometry* FoamGeometryDecorator::geometry() const
{
    return _geometry;
}

//////////////////////////////////////////////////////////////////////

void FoamGeometryDecorator::setNumCells(int value)
{
    _Ncells = value;
}

//////////////////////////////////////////////////////////////////////

int FoamGeometryDecorator::numCells() const
{
    return _Ncells;
}

///////////////////////////////////////////////////////////////////////////////

double FoamGeometryDecorator::density(Position bfr) const
{
    return _geometry->density(bfr);
}

///////////////////////////////////////////////////////////////////////////////

double FoamGeometryDecorator::SigmaX() const
{
    return _geometry->SigmaX();
}

///////////////////////////////////////////////////////////////////////////////

double FoamGeometryDecorator::SigmaY() const
{
    return _geometry->SigmaY();
}

///////////////////////////////////////////////////////////////////////////////

double FoamGeometryDecorator::SigmaZ() const
{
    return _geometry->SigmaZ();
}

///////////////////////////////////////////////////////////////////////////////

Position FoamGeometryDecorator::generatePosition() const
{
    double par[3];
    _foam->MCgenerate(par);
    return Position(fracpos(par[0], par[1], par[2]));
}

/////////////////////////////////////////////////////////////////////

double FoamGeometryDecorator::foamdensity(int ndim, double* par) const
{
    if (ndim != 3) throw FATALERROR("Incorrect dimension (ndim = " + QString::number(ndim) + ")");
    return _geometry->density(Position(fracpos(par[0], par[1], par[2]))) * _jacobian;
}

///////////////////////////////////////////////////////////////////////////////
