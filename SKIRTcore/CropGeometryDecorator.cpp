/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "FatalError.hpp"
#include "Random.hpp"
#include "CropGeometryDecorator.hpp"

using namespace std;

////////////////////////////////////////////////////////////////////

CropGeometryDecorator::CropGeometryDecorator()
    : _geometry(0), _xmax(0), _ymax(0), _zmax(0)
{
}

////////////////////////////////////////////////////////////////////

void CropGeometryDecorator::setupSelfBefore()
{
    Geometry::setupSelfBefore();
    if (_xmax <= 0.0 || _ymax <= 0.0 || _zmax <= 0.0) throw FATALERROR("The maximum extent should be positive");
}

////////////////////////////////////////////////////////////////////

void
CropGeometryDecorator::setupSelfAfter()
{
    Geometry::setupSelfAfter();

    // estimate the original geometry's mass outside the bounding box
    int Nsamples = 10000;
    int Ncrop = 0;
    for (int k=0; k<Nsamples; k++)
    {
        Position bfr = _geometry->generatePosition();
        double x, y, z;
        bfr.cartesian(x,y,z);
        if (x<-_xmax || x>_xmax || y<-_ymax || y>_ymax || z<-_zmax || z>_zmax) Ncrop++;
    }
    double chi = Ncrop/(1.0*Nsamples);
    _norm = 1.0/(1.0-chi);
}

////////////////////////////////////////////////////////////////////

void
CropGeometryDecorator::setGeometry(Geometry* value)
{
    if (_geometry) delete _geometry;
    _geometry = value;
    if (_geometry) _geometry->setParent(this);
}

////////////////////////////////////////////////////////////////////

Geometry*
CropGeometryDecorator::geometry()
const
{
    return _geometry;
}

//////////////////////////////////////////////////////////////////////

void
CropGeometryDecorator::setExtentX(double value)
{
    _xmax = value;
}

//////////////////////////////////////////////////////////////////////

double
CropGeometryDecorator::extentX()
const
{
    return _xmax;
}

//////////////////////////////////////////////////////////////////////

void
CropGeometryDecorator::setExtentY(double value)
{
    _ymax = value;
}

//////////////////////////////////////////////////////////////////////

double
CropGeometryDecorator::extentY()
const
{
    return _ymax;
}

//////////////////////////////////////////////////////////////////////

void
CropGeometryDecorator::setExtentZ(double value)
{
    _zmax = value;
}

//////////////////////////////////////////////////////////////////////

double
CropGeometryDecorator::extentZ()
const
{
    return _zmax;
}

////////////////////////////////////////////////////////////////////

double
CropGeometryDecorator::density(Position bfr)
const
{
    double x, y, z;
    bfr.cartesian(x,y,z);
    if (x<-_xmax || x>_xmax || y<-_ymax || y>_ymax || z<-_zmax || z>_zmax)
        return 0.0;
    else
        return _geometry->density(bfr) * _norm;
}

////////////////////////////////////////////////////////////////////

Position
CropGeometryDecorator::generatePosition()
const
{
    while (true)
    {
        Position bfr = _geometry->generatePosition();
        double x, y, z;
        bfr.cartesian(x,y,z);
        if (x>-_xmax && x<_xmax && y>-_ymax && y<_ymax && z>-_zmax && z<_zmax)
            return bfr;
    }
}

////////////////////////////////////////////////////////////////////

double
CropGeometryDecorator::SigmaX()
const
{
    return _geometry->SigmaX();
}

////////////////////////////////////////////////////////////////////

double
CropGeometryDecorator::SigmaY()
const
{
    return _geometry->SigmaY();
}

////////////////////////////////////////////////////////////////////

double
CropGeometryDecorator::SigmaZ()
const
{
    return _geometry->SigmaZ();
}

////////////////////////////////////////////////////////////////////

double
CropGeometryDecorator::probabilityForDirection(Position bfr, Direction bfk)
const
{
    return _geometry->probabilityForDirection(bfr, bfk);
}

////////////////////////////////////////////////////////////////////

Direction
CropGeometryDecorator::generateDirection(Position bfr)
const
{
    return _geometry->generateDirection(bfr);
}

////////////////////////////////////////////////////////////////////
