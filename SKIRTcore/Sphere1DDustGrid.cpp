/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include <cmath>
#include <cstdlib>
#include "DustGridPath.hpp"
#include "DustGridPlotFile.hpp"
#include "Log.hpp"
#include "NR.hpp"
#include "Sphere1DDustGrid.hpp"

using namespace std;

//////////////////////////////////////////////////////////////////////

Sphere1DDustGrid::Sphere1DDustGrid()
    : _meshr(0), _random(0), _Nr(0)
{
}

//////////////////////////////////////////////////////////////////////

void Sphere1DDustGrid::setupSelfAfter()
{
    // Cache the random number generator
    _random = find<Random>();

    // Set up the grid properties
    _Nr = _meshr->numBins();
    _rv = _meshr->mesh() * maxR();

    // base class setupSelfAfter() depends on initialization performed above
    SphereDustGrid::setupSelfAfter();
}

//////////////////////////////////////////////////////////////////////

void Sphere1DDustGrid::setMeshR(Mesh* value)
{
    if (_meshr) delete _meshr;
    _meshr = value;
    if (_meshr) _meshr->setParent(this);
}

//////////////////////////////////////////////////////////////////////

Mesh* Sphere1DDustGrid::meshR() const
{
    return _meshr;
}

//////////////////////////////////////////////////////////////////////

int Sphere1DDustGrid::dimension() const
{
    return 1;
}

//////////////////////////////////////////////////////////////////////

int Sphere1DDustGrid::numCells() const
{
    return _Nr;
}

//////////////////////////////////////////////////////////////////////

double Sphere1DDustGrid::volume(int m) const
{
    int i = m;
    if (i<0 || i>=_Nr)
        return 0.0;
    else
    {
        double rL = _rv[i];
        double rR = _rv[i+1];
        return 4.0*M_PI/3.0 * (rR-rL) * (rR*rR + rR*rL + rL*rL);
    }
}

//////////////////////////////////////////////////////////////////////

int Sphere1DDustGrid::whichcell(Position bfr) const
{
    return NR::locate_fail(_rv, bfr.radius());
}

//////////////////////////////////////////////////////////////////////

Position Sphere1DDustGrid::centralPositionInCell(int m) const
{
    int i = m;
    double r = (_rv[i]+_rv[i+1])/2.0;
    return Position(r,0,0);
}

//////////////////////////////////////////////////////////////////////

Position Sphere1DDustGrid::randomPositionInCell(int m) const
{
    int i = m;
    Direction bfk = _random->direction();
    double r = _rv[i] + (_rv[i+1]-_rv[i])*_random->uniform();
    return Position(r,bfk);
}

//////////////////////////////////////////////////////////////////////

void Sphere1DDustGrid::path(DustGridPath* path) const
{
    // Determination of the initial position and direction of the path,
    // and calculation of some initial values

    path->clear();
    double x,y,z;
    path->position().cartesian(x,y,z);
    double kx,ky,kz;
    path->direction().cartesian(kx,ky,kz);
    double rmax = maxR();

    // Move the photon package to the first grid cell that it will pass.
    // If it does not pass any grid cell, return an empty path.

    double r = path->position().radius();
    double q = x*kx + y*ky + z*kz;
    double p = sqrt((r-q)*(r+q));
    if (r>rmax)
    {
        if (q>0.0 || p>rmax) return path->clear();
        else
        {
            r = rmax - 1e-8*(_rv[_Nr]-_rv[_Nr-1]);
            double qmax = sqrt((rmax-p)*(rmax+p));
            double ds = (qmax-q);
            path->addSegment(-1,ds);
            q = qmax;
        }
    }

    // Determination of the initial grid cell

    int i = NR::locate_clip(_rv, r);

    // And here we go...

    double rN, qN;

    // Inward movement (not always...)

    if (q<0.0)
    {
        int imin = NR::locate_clip(_rv,p);
        rN = _rv[i];
        qN = -sqrt((rN-p)*(rN+p));
        while (i>imin)
        {
            int m = i;
            double ds = qN-q;
            path->addSegment(m, ds);
            i--;
            q = qN;
            rN = _rv[i];
            qN = -sqrt((rN-p)*(rN+p));
        }
    }

    // Outward movement

    rN = _rv[i+1];
    qN = sqrt((rN-p)*(rN+p));
    while (true)
    {
        int m = i;
        double ds = qN-q;
        path->addSegment(m, ds);
        i++;
        if (i>=_Nr-1) return;
        else
        {
            q = qN;
            rN = _rv[i+1];
            qN = sqrt((rN-p)*(rN+p));
        }
    }
}

//////////////////////////////////////////////////////////////////////

void Sphere1DDustGrid::write_xy(DustGridPlotFile* outfile) const
{
    for (int i=0; i<=_Nr; i++) outfile->writeCircle(_rv[i]);
}

//////////////////////////////////////////////////////////////////////
