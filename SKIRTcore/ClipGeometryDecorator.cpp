/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "ClipGeometryDecorator.hpp"
#include "FatalError.hpp"

using namespace std;

////////////////////////////////////////////////////////////////////

ClipGeometryDecorator::ClipGeometryDecorator()
    : _geometry(0), _remove(Inside)
{
}

////////////////////////////////////////////////////////////////////

void ClipGeometryDecorator::setupSelfAfter()
{
    Geometry::setupSelfAfter();

    // estimate the original geometry's mass in the removed region
    int Nsamples = 10000;
    int Ninside = 0;
    for (int k=0; k<Nsamples; k++)
    {
        Position bfr = _geometry->generatePosition();
        if (inside(bfr)) Ninside++;
    }
    double chi = Ninside/(1.0*Nsamples);
    if (_remove==Outside) chi = 1.0-chi;
    if (chi > 0.9) throw FATALERROR("Clip decorator removes more than 90% of the original mass");
    _norm = 1.0/(1.0-chi);
}

////////////////////////////////////////////////////////////////////

void ClipGeometryDecorator::setGeometry(Geometry* value)
{
    if (_geometry) delete _geometry;
    _geometry = value;
    if (_geometry) _geometry->setParent(this);
}

////////////////////////////////////////////////////////////////////

Geometry* ClipGeometryDecorator::geometry() const
{
    return _geometry;
}

////////////////////////////////////////////////////////////////////

void ClipGeometryDecorator::setRemove(ClipGeometryDecorator::Remove value)
{
    _remove = value;
}

////////////////////////////////////////////////////////////////////

ClipGeometryDecorator::Remove ClipGeometryDecorator::remove() const
{
    return _remove;
}

//////////////////////////////////////////////////////////////////////

double ClipGeometryDecorator::density(Position bfr) const
{
    bool removed = inside(bfr);
    if (_remove==Outside) removed = !removed;
    return removed ? 0.0 : _geometry->density(bfr) * _norm;
}

////////////////////////////////////////////////////////////////////

Position ClipGeometryDecorator::generatePosition() const
{
    while (true)
    {
        Position bfr = _geometry->generatePosition();
        bool removed = inside(bfr);
        if (_remove==Outside) removed = !removed;
        if (!removed) return bfr;
    }
}

////////////////////////////////////////////////////////////////////

double ClipGeometryDecorator::SigmaX() const
{
    return _geometry->SigmaX();
}

////////////////////////////////////////////////////////////////////

double ClipGeometryDecorator::SigmaY() const
{
    return _geometry->SigmaY();
}

////////////////////////////////////////////////////////////////////

double ClipGeometryDecorator::SigmaZ() const
{
    return _geometry->SigmaZ();
}

////////////////////////////////////////////////////////////////////

double ClipGeometryDecorator::probabilityForDirection(int ell, Position bfr, Direction bfk) const
{
    return _geometry->probabilityForDirection(ell, bfr, bfk);
}

////////////////////////////////////////////////////////////////////

Direction ClipGeometryDecorator::generateDirection(int ell, Position bfr) const
{
    return _geometry->generateDirection(ell, bfr);
}

////////////////////////////////////////////////////////////////////
