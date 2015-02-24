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
    : _geometry(0), _xmax(0), _ymax(0), _zmax(0), _Ncells(0), _foam(0)
{
}

///////////////////////////////////////////////////////////////////////////////

FoamGeometryDecorator::~FoamGeometryDecorator()
{
    delete _foam;
}

//////////////////////////////////////////////////////////////////////

void
FoamGeometryDecorator::setupSelfAfter()
{
    GenGeometry::setupSelfAfter();
    _foam = Foam::createFoam(find<Log>(), _random, this, 3, _Ncells);
}

////////////////////////////////////////////////////////////////////

void
FoamGeometryDecorator::setGeometry(Geometry* value)
{
    if (_geometry) delete _geometry;
    _geometry = value;
    if (_geometry) _geometry->setParent(this);
}

////////////////////////////////////////////////////////////////////

Geometry*
FoamGeometryDecorator::geometry()
const
{
    return _geometry;
}

//////////////////////////////////////////////////////////////////////

void
FoamGeometryDecorator::setExtentX(double value)
{
    if (value <= 0.0) throw FATALERROR("The maximum extent (in the X direction) should be positive");
    _xmax = value;
}

//////////////////////////////////////////////////////////////////////

double
FoamGeometryDecorator::extentX()
const
{
    return _xmax;
}

//////////////////////////////////////////////////////////////////////

void
FoamGeometryDecorator::setExtentY(double value)
{
    if (value <= 0.0) throw FATALERROR("The maximum extent (in the Y direction) should be positive");
    _ymax = value;
}

//////////////////////////////////////////////////////////////////////

double
FoamGeometryDecorator::extentY()
const
{
    return _ymax;
}

//////////////////////////////////////////////////////////////////////

void
FoamGeometryDecorator::setExtentZ(double value)
{
    if (value <= 0.0) throw FATALERROR("The maximum extent (in the Z direction) should be positive");
    _zmax = value;
}

//////////////////////////////////////////////////////////////////////

double
FoamGeometryDecorator::extentZ()
const
{
    return _zmax;
}

//////////////////////////////////////////////////////////////////////

void
FoamGeometryDecorator::setNumCells(int value)
{
    if (value < 1000) throw FATALERROR("The number of foam cells should be at least 1000");
    if (value > 1000000) throw FATALERROR("The number of foam cells should be at most 1000000");
    _Ncells = value;
}

//////////////////////////////////////////////////////////////////////

int
FoamGeometryDecorator::numCells()
const
{
    return _Ncells;
}

///////////////////////////////////////////////////////////////////////////////

double
FoamGeometryDecorator::density(Position bfr)
const
{
    return _geometry->density(bfr);
}

///////////////////////////////////////////////////////////////////////////////

double
FoamGeometryDecorator::SigmaX()
const
{
    return _geometry->SigmaX();
}

///////////////////////////////////////////////////////////////////////////////

double
FoamGeometryDecorator::SigmaY()
const
{
    return _geometry->SigmaY();
}

///////////////////////////////////////////////////////////////////////////////

double
FoamGeometryDecorator::SigmaZ()
const
{
    return _geometry->SigmaZ();
}

///////////////////////////////////////////////////////////////////////////////

Position
FoamGeometryDecorator::generatePosition()
const
{
    double par[3];
    _foam->MCgenerate(par);
    double xbar = par[0];
    double ybar = par[1];
    double zbar = par[2];
    double x = (2.0*xbar-1.0)*_xmax;
    double y = (2.0*ybar-1.0)*_ymax;
    double z = (2.0*zbar-1.0)*_zmax;
    return Position(x,y,z);
}

/////////////////////////////////////////////////////////////////////

double
FoamGeometryDecorator::foamdensity(int ndim, double* par)
const
{
    if (ndim != 3)
        throw FATALERROR("Incorrect dimension (ndim = " + QString::number(ndim) + ")");
    double xbar = par[0];
    double ybar = par[1];
    double zbar = par[2];
    double x = (2.0*xbar-1.0)*_xmax;
    double y = (2.0*ybar-1.0)*_ymax;
    double z = (2.0*zbar-1.0)*_zmax;
    double jacobian = 8.0*_xmax*_ymax*_zmax;
    return _geometry->density(Position(x,y,z)) * jacobian;
}

///////////////////////////////////////////////////////////////////////////////
