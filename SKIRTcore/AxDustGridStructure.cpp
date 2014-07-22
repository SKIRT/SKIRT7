/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include <cmath>
#include <cstdlib>
#include "AxDustGridStructure.hpp"
#include "DustGridPath.hpp"
#include "DustGridPlotFile.hpp"
#include "NR.hpp"
#include "Random.hpp"

using namespace std;

//////////////////////////////////////////////////////////////////////

AxDustGridStructure::AxDustGridStructure()
    : _NR(0), _Nz(0), _Rmax(0), _zmin(0), _zmax(0)
{
}

//////////////////////////////////////////////////////////////////////

int AxDustGridStructure::dimension() const
{
    return 2;
}

//////////////////////////////////////////////////////////////////////

double
AxDustGridStructure::xmax()
const
{
    return _Rmax;
}

//////////////////////////////////////////////////////////////////////

double
AxDustGridStructure::ymax()
const
{
    return _Rmax;
}

//////////////////////////////////////////////////////////////////////

double
AxDustGridStructure::zmax()
const
{
    return _zmax;
}

//////////////////////////////////////////////////////////////////////

double
AxDustGridStructure::volume(int m)
const
{
    int i, k;
    invertindex(m,i,k);
    if (i<0 || i>=_NR || k<0 || k>=_Nz)
        return 0.0;
    else
        return M_PI*(_zv[k+1]-_zv[k])*(_Rv[i+1]-_Rv[i])*(_Rv[i+1]+_Rv[i]);
}

//////////////////////////////////////////////////////////////////////

int
AxDustGridStructure::whichcell(Position bfr)
const
{
    int i = NR::locate_fail(_Rv, bfr.cylradius());
    int k = NR::locate_fail(_zv, bfr.height());
    if (i<0 || k<0) return -1;
    return index(i,k);
}

//////////////////////////////////////////////////////////////////////

Position
AxDustGridStructure::centralPositionInCell(int m)
const
{
    int i, k;
    invertindex(m,i,k);
    double R = (_Rv[i]+_Rv[i+1])/2.0;
    double phi = 0.0;
    double z = (_zv[k]+_zv[k+1])/2.0;
    return Position(R,phi,z,Position::CYLINDRICAL);
}

//////////////////////////////////////////////////////////////////////

Position
AxDustGridStructure::randomPositionInCell(int m)
const
{
    int i, k;
    invertindex(m,i,k);
    double R = _Rv[i] + (_Rv[i+1]-_Rv[i])*_random->uniform();
    double phi = 2.0*M_PI*_random->uniform();
    double z = _zv[k] + (_zv[k+1]-_zv[k])*_random->uniform();
    return Position(R,phi,z,Position::CYLINDRICAL);
}

//////////////////////////////////////////////////////////////////////

void AxDustGridStructure::path(DustGridPath* path) const
{
    // Determination of the initial position and direction of the path,
    // and calculation of some initial values
    path->clear();
    double kx,ky,kz;
    path->direction().cartesian(kx,ky,kz);
    double kq = sqrt(kx*kx+ky*ky);
    if (kz==0.0) kz = 1e-20;   // avoid moving exactly parallel to the equatorial plane
    if (kq==0.0) kq = 1e-20;   // avoid moving exactly parallel to the z-axis
    double x,y,z;
    path->position().cartesian(x,y,z);
    double R = path->position().cylradius();
    double q = (x*kx+y*ky)/kq;
    double p2 = (R-q)*(R+q);
    double p = sqrt(max(0.0,p2));  // make sure that p>=0 here; necessary sometimes due to rounding errors

    // Move the photon package to the first grid cell that it will pass.
    // If it does not pass any grid cell, return an empty path.
    // Otherwise calculate the distance covered and add a segment to the path.

    if (R>=_Rmax)
    {
        if (q>0.0 || p>_Rmax) return path->clear();
        else
        {
            R = _Rmax - 1e-8*(_Rv[_NR]-_Rv[_NR-1]);
            double qmax = sqrt((_Rmax-p)*(_Rmax+p));
            double ds = (qmax-q)/kq;
            path->addSegment(-1,ds);
            q = qmax;
            z += kz*ds;
        }
    }
    if (z<_zmin)
    {
        if (kz<=0.0) return path->clear();
        else
        {
            double ds = (_zmin-z)/kz;
            path->addSegment(-1,ds);
            q += kq*ds;
            R = sqrt(p*p+q*q);
            z = _zmin + 1e-8*(_zv[1]-_zv[0]);
        }
    }
    else if (z>_zmax)
    {
        if (kz>=0.0) return path->clear();
        else
        {
            double ds = (_zmax-z)/kz;
            path->addSegment(-1,ds);
            q += kq*ds;
            R = sqrt(p*p+q*q);
            z = _zmax - 1e-8*(_zv[_Nz]-_zv[_Nz-1]);
        }
    }
    if (std::isinf(R) || std::isnan(R) || std::isinf(z) || std::isnan(z) ||
        R>=_Rmax || z<=_zmin || z>=_zmax) return path->clear();

    // Determination of the initial grid cell

    int i = NR::locate_clip(_Rv, R);
    int k = NR::locate_clip(_zv, z);

    // And here we go...

    double ds, dsq, dsz;
    double RN, qN, zN;

    // SCENARIO 1: UPWARD MOVEMENT

    if (kz>=0.0)
    {
        if (q<0.0)
        {
            int imin = NR::locate_clip(_Rv, p);
            RN = _Rv[i];
            qN = -sqrt((RN-p)*(RN+p));
            zN = _zv[k+1];
            while (i>imin)
            {
                int m = index(i,k);
                dsq = (qN-q)/kq;
                dsz = (zN-z)/kz;
                if (dsq<dsz)
                {
                    ds = dsq;
                    path->addSegment(m, ds);
                    i--;
                    q = qN;
                    z += kz*ds;
                    RN = _Rv[i];
                    qN = -sqrt((RN-p)*(RN+p));
                }
                else
                {
                    ds = dsz;
                    path->addSegment(m, ds);
                    k++;
                    if (k>=_Nz) return;
                    else
                    {
                        q += kq*ds;
                        z = zN;
                        zN = _zv[k+1];
                    }
                }
            }
        }
        RN = _Rv[i+1];
        qN = sqrt((RN-p)*(RN+p));
        zN = _zv[k+1];
        while (true) {
            int m = index(i,k);
            dsq = (qN-q)/kq;
            dsz = (zN-z)/kz;
            if (dsq<dsz)
            {
                ds = dsq;
                path->addSegment(m, ds);
                i++;
                if (i>=_NR) return;
                else
                {
                    q = qN;
                    z += kz*ds;
                    RN = _Rv[i+1];
                    qN = sqrt((RN-p)*(RN+p));
                }
            }
            else
            {
                ds = dsz;
                path->addSegment(m, ds);
                k++;
                if (k>=_Nz) return;
                else
                {
                    q += kq*ds;
                    z = zN;
                    zN = _zv[k+1];
                }
            }
        }
    }

    // SCENARIO 2: DOWNWARD MOVEMENT

    else
    {
        if (q<0.0)
        {
            int imin = NR::locate_clip(_Rv, p);
            RN = _Rv[i];
            qN = -sqrt((RN-p)*(RN+p));
            zN = _zv[k];
            while (i>imin)
            {
                int m = index(i,k);
                dsq = (qN-q)/kq;
                dsz = (zN-z)/kz;
                if (dsq<dsz)
                {
                    ds = dsq;
                    path->addSegment(m, ds);
                    i--;
                    q = qN;
                    z += kz*ds;
                    RN = _Rv[i];
                    qN = -sqrt((RN-p)*(RN+p));
                }
                else
                {
                    ds = dsz;
                    path->addSegment(m, ds);
                    k--;
                    if (k<0) return;
                    else
                    {
                        q += kq*ds;
                        z = zN;
                        zN = _zv[k];
                    }
                }
            }
        }
        RN = _Rv[i+1];
        qN = sqrt((RN-p)*(RN+p));
        zN = _zv[k];
        while (true) {
            int m = index(i,k);
            dsq = (qN-q)/kq;
            dsz = (zN-z)/kz;
            if (dsq<dsz)
            {
                ds = dsq;
                path->addSegment(m, ds);
                i++;
                if (i>=_NR-1) return;
                else
                {
                    q = qN;
                    z += kz*ds;
                    RN = _Rv[i+1];
                    qN = sqrt((RN-p)*(RN+p));
                }
            }
            else
            {
                ds = dsz;
                path->addSegment(m, ds);
                k--;
                if (k<0) return;
                else
                {
                    q += kq*ds;
                    z = zN;
                    zN = _zv[k];
                }
            }
        }
    }
}

//////////////////////////////////////////////////////////////////////

void AxDustGridStructure::write_xy(DustGridPlotFile* outfile) const
{
    for (int i=0; i<=_NR; i++) outfile->writeCircle(_Rv[i]);
}

//////////////////////////////////////////////////////////////////////

void AxDustGridStructure::write_xz(DustGridPlotFile* outfile) const
{
    for (int i=0; i<=_NR; i++)
    {
        outfile->writeLine(_Rv[i], _zmin, _Rv[i], _zmax);
        outfile->writeLine(-_Rv[i], _zmin, -_Rv[i], _zmax);
    }
    for (int k=0; k<=_Nz; k++)
    {
        outfile->writeLine(-_Rmax, _zv[k], _Rmax, _zv[k]);
    }
}

//////////////////////////////////////////////////////////////////////

int
AxDustGridStructure::index(int i, int k)
const
{
    return k+_Nz*i;
}

//////////////////////////////////////////////////////////////////////

void
AxDustGridStructure::invertindex(int m, int& i, int& k)
const
{
    i = m / _Nz;
    k = m % _Nz;
}

//////////////////////////////////////////////////////////////////////
