/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include <cmath>
#include <cstdlib>
#include "DustGridPath.hpp"
#include "DustGridPlotFile.hpp"
#include "NR.hpp"
#include "Random.hpp"
#include "SpheDustGridStructure.hpp"

using namespace std;

//////////////////////////////////////////////////////////////////////

SpheDustGridStructure::SpheDustGridStructure()
    : _Nr(0), _rmax(0)
{
}

//////////////////////////////////////////////////////////////////////

int SpheDustGridStructure::dimension() const
{
    return 1;
}

//////////////////////////////////////////////////////////////////////

double SpheDustGridStructure::xmax() const
{
    return _rmax;
}

//////////////////////////////////////////////////////////////////////

double SpheDustGridStructure::ymax() const
{
    return _rmax;
}

//////////////////////////////////////////////////////////////////////

double SpheDustGridStructure::zmax() const
{
    return _rmax;
}

//////////////////////////////////////////////////////////////////////

double SpheDustGridStructure::volume(int m) const
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

int SpheDustGridStructure::whichcell(Position bfr) const
{
    return NR::locate_fail(_rv, bfr.radius());
}

//////////////////////////////////////////////////////////////////////

Position SpheDustGridStructure::centralPositionInCell(int m) const
{
    int i = m;
    double r = (_rv[i]+_rv[i+1])/2.0;
    return Position(r,0.0,0.0);
}

//////////////////////////////////////////////////////////////////////

Position SpheDustGridStructure::randomPositionInCell(int m) const
{
    int i = m;
    Direction bfk = _random->direction();
    double r = _rv[i] + (_rv[i+1]-_rv[i])*_random->uniform();
    return Position(r,bfk);
}

//////////////////////////////////////////////////////////////////////

DustGridPath SpheDustGridStructure::path(Position bfr, Direction bfk) const
{
    // Determination of the initial position and direction of the path,
    // and calculation of some initial values

    DustGridPath path(bfr, bfk, 2*_Nr + 2);
    double x,y,z;
    bfr.cartesian(x,y,z);
    double kx,ky,kz;
    bfk.cartesian(kx,ky,kz);

    // Move the photon package to the first grid cell that it will pass.
    // If it does not pass any grid cell, return an empty path.

    double r = bfr.radius();
    double q = x*kx + y*ky + z*kz;
    double p = sqrt((r-q)*(r+q));
    if (r>_rmax)
    {
        if (q>0.0 || p>_rmax) return path.clear();
        else
        {
            r = _rmax - 1e-8*(_rv[_Nr]-_rv[_Nr-1]);
            double qmax = sqrt((_rmax-p)*(_rmax+p));
            double ds = (qmax-q);
            path.addsegment(-1,ds);
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
            path.addsegment(m, ds);
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
        path.addsegment(m, ds);
        i++;
        if (i>=_Nr-1) return path;
        else
        {
            q = qN;
            rN = _rv[i+1];
            qN = sqrt((rN-p)*(rN+p));
        }
    }
}

//////////////////////////////////////////////////////////////////////

void SpheDustGridStructure::write_xy(DustGridPlotFile* outfile) const
{
    for (int i=0; i<=_Nr; i++) outfile->writeCircle(_rv[i]);
}

//////////////////////////////////////////////////////////////////////
