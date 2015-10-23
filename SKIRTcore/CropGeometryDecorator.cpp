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
    : _geometry(0)
{
}

////////////////////////////////////////////////////////////////////

void
CropGeometryDecorator::setupSelfAfter()
{
    BoxGeometry::setupSelfAfter();

    // estimate the original geometry's mass outside the bounding box
    int Nsamples = 10000;
    int Ncrop = 0;
    for (int k=0; k<Nsamples; k++)
    {
        Position bfr = _geometry->generatePosition();
        if (!contains(bfr)) Ncrop++;
    }
    double chi = Ncrop/(1.0*Nsamples);
    _norm = 1.0/(1.0-chi);
}

////////////////////////////////////////////////////////////////////

void CropGeometryDecorator::setGeometry(Geometry* value)
{
    if (_geometry) delete _geometry;
    _geometry = value;
    if (_geometry) _geometry->setParent(this);
}

////////////////////////////////////////////////////////////////////

Geometry* CropGeometryDecorator::geometry() const
{
    return _geometry;
}

//////////////////////////////////////////////////////////////////////

double CropGeometryDecorator::density(Position bfr) const
{
    return contains(bfr) ? _geometry->density(bfr) * _norm : 0.0;
}

////////////////////////////////////////////////////////////////////

Position CropGeometryDecorator::generatePosition() const
{
    while (true)
    {
        Position bfr = _geometry->generatePosition();
        if (contains(bfr)) return bfr;
    }
}

////////////////////////////////////////////////////////////////////

double CropGeometryDecorator::SigmaX() const
{
    return _geometry->SigmaX();
}

////////////////////////////////////////////////////////////////////

double CropGeometryDecorator::SigmaY() const
{
    return _geometry->SigmaY();
}

////////////////////////////////////////////////////////////////////

double CropGeometryDecorator::SigmaZ() const
{
    return _geometry->SigmaZ();
}

////////////////////////////////////////////////////////////////////

double CropGeometryDecorator::probabilityForDirection(int ell, Position bfr, Direction bfk) const
{
    return _geometry->probabilityForDirection(ell, bfr, bfk);
}

////////////////////////////////////////////////////////////////////

Direction CropGeometryDecorator::generateDirection(int ell, Position bfr) const
{
    return _geometry->generateDirection(ell, bfr);
}

////////////////////////////////////////////////////////////////////
