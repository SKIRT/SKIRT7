/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include <cmath>
#include "FatalError.hpp"
#include "Foam.hpp"
#include "FoamDecoGeometry.hpp"
#include "Log.hpp"
#include "Random.hpp"

using namespace std;

///////////////////////////////////////////////////////////////////////////////

FoamDecoGeometry::FoamDecoGeometry()
    : _geometry(0), _xmax(0), _ymax(0), _zmax(0), _Ncells(0), _foam(0)
{
}

///////////////////////////////////////////////////////////////////////////////

FoamDecoGeometry::~FoamDecoGeometry()
{
    delete _foam;
}

//////////////////////////////////////////////////////////////////////

void
FoamDecoGeometry::setupSelfBefore()
{
    GenGeometry::setupSelfBefore();
}

//////////////////////////////////////////////////////////////////////

void
FoamDecoGeometry::setupSelfAfter()
{
    GenGeometry::setupSelfAfter();
    _foam = Foam::createFoam(find<Log>(), _random, this, 3, _Ncells);
}

////////////////////////////////////////////////////////////////////

void
FoamDecoGeometry::setGeometry(Geometry* value)
{
    if (_geometry) delete _geometry;
    _geometry = value;
    if (_geometry) _geometry->setParent(this);
}

////////////////////////////////////////////////////////////////////

Geometry*
FoamDecoGeometry::geometry()
const
{
    return _geometry;
}

//////////////////////////////////////////////////////////////////////

void
FoamDecoGeometry::setExtentX(double value)
{
    if (value <= 0.0) throw FATALERROR("The maximum extent (in the X direction) should be positive");
    _xmax = value;
}

//////////////////////////////////////////////////////////////////////

double
FoamDecoGeometry::extentX()
const
{
    return _xmax;
}

//////////////////////////////////////////////////////////////////////

void
FoamDecoGeometry::setExtentY(double value)
{
    if (value <= 0.0) throw FATALERROR("The maximum extent (in the X direction) should be positive");
    _ymax = value;
}

//////////////////////////////////////////////////////////////////////

double
FoamDecoGeometry::extentY()
const
{
    return _ymax;
}

//////////////////////////////////////////////////////////////////////

void
FoamDecoGeometry::setExtentZ(double value)
{
    if (value <= 0.0) throw FATALERROR("The maximum extent (in the X direction) should be positive");
    _zmax = value;
}

//////////////////////////////////////////////////////////////////////

double
FoamDecoGeometry::extentZ()
const
{
    return _zmax;
}

//////////////////////////////////////////////////////////////////////

void
FoamDecoGeometry::setNumCells(int value)
{
    if (value < 100) throw FATALERROR("The minimum tree level should be at least 100");
    if (value > 100000) throw FATALERROR("The minimum tree level should be at most 100000");
    _Ncells = value;
}

//////////////////////////////////////////////////////////////////////

int
FoamDecoGeometry::numCells()
const
{
    return _Ncells;
}

///////////////////////////////////////////////////////////////////////////////

double
FoamDecoGeometry::density(Position bfr)
const
{
    return _geometry->density(bfr);
}

///////////////////////////////////////////////////////////////////////////////

double
FoamDecoGeometry::SigmaX()
const
{
    return _geometry->SigmaX();
}

///////////////////////////////////////////////////////////////////////////////

double
FoamDecoGeometry::SigmaY()
const
{
    return _geometry->SigmaY();
}

///////////////////////////////////////////////////////////////////////////////

double
FoamDecoGeometry::SigmaZ()
const
{
    return _geometry->SigmaZ();
}

///////////////////////////////////////////////////////////////////////////////

Position
FoamDecoGeometry::generatePosition()
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
FoamDecoGeometry::foamdensity(int ndim, double* par)
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
