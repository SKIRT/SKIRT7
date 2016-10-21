#include <cfloat>
#include <cmath>
#include <cstdlib>
#include "Cylinder2DDustGrid.hpp"
#include "DustGridPath.hpp"
#include "DustGridPlotFile.hpp"
#include "FatalError.hpp"
#include "Log.hpp"
#include "NR.hpp"
#include "Random.hpp"

using namespace std;

//////////////////////////////////////////////////////////////////////

Cylinder2DDustGrid::Cylinder2DDustGrid()
    : _meshR(0), _meshz(0), _random(), _NR(0), _Nz(0)
{
}

//////////////////////////////////////////////////////////////////////

void Cylinder2DDustGrid::setupSelfAfter()
{
    // initialize our local mesh arrays
    _NR = _meshR->numBins();
    _Nz = _meshz->numBins();
    double Rmax = maxR();
    double zmin = minZ();
    double zmax = maxZ();
    _Rv = _meshR->mesh()*Rmax;
    _zv = _meshz->mesh()*(zmax-zmin) + zmin;

    // cache the random number generator
    _random = find<Random>();

    // base class setupSelfAfter() depends on initialization performed above
    CylinderDustGrid::setupSelfAfter();
}

//////////////////////////////////////////////////////////////////////

void Cylinder2DDustGrid::setMeshR(Mesh* value)
{
    if (_meshR) delete _meshR;
    _meshR = value;
    if (_meshR) _meshR->setParent(this);
}

//////////////////////////////////////////////////////////////////////

Mesh* Cylinder2DDustGrid::meshR() const
{
    return _meshR;
}

//////////////////////////////////////////////////////////////////////

void Cylinder2DDustGrid::setMeshZ(MoveableMesh* value)
{
    if (_meshz) delete _meshz;
    _meshz = value;
    if (_meshz) _meshz->setParent(this);
}

//////////////////////////////////////////////////////////////////////

MoveableMesh* Cylinder2DDustGrid::meshZ() const
{
    return _meshz;
}

//////////////////////////////////////////////////////////////////////

int Cylinder2DDustGrid::dimension() const
{
    return 2;
}

//////////////////////////////////////////////////////////////////////

int Cylinder2DDustGrid::numCells() const
{
    return _NR*_Nz;
}

//////////////////////////////////////////////////////////////////////

double Cylinder2DDustGrid::volume(int m) const
{
    int i, k;
    invertindex(m,i,k);
    if (i<0 || i>=_NR || k<0 || k>=_Nz)
        return 0.0;
    else
        return M_PI*(_zv[k+1]-_zv[k])*(_Rv[i+1]-_Rv[i])*(_Rv[i+1]+_Rv[i]);
}

//////////////////////////////////////////////////////////////////////

int Cylinder2DDustGrid::whichcell(Position bfr) const
{
    int i = NR::locate_fail(_Rv, bfr.cylradius());
    int k = NR::locate_fail(_zv, bfr.height());
    if (i<0 || k<0) return -1;
    return index(i,k);
}

//////////////////////////////////////////////////////////////////////

Position Cylinder2DDustGrid::centralPositionInCell(int m) const
{
    int i, k;
    invertindex(m,i,k);
    double R = (_Rv[i]+_Rv[i+1])/2.0;
    double phi = 0.0;
    double z = (_zv[k]+_zv[k+1])/2.0;
    return Position(R,phi,z,Position::CYLINDRICAL);
}

//////////////////////////////////////////////////////////////////////

Position Cylinder2DDustGrid::randomPositionInCell(int m) const
{
    int i, k;
    invertindex(m,i,k);
    double R = _Rv[i] + (_Rv[i+1]-_Rv[i])*_random->uniform();
    double phi = 2.0*M_PI*_random->uniform();
    double z = _zv[k] + (_zv[k+1]-_zv[k])*_random->uniform();
    return Position(R,phi,z,Position::CYLINDRICAL);
}

//////////////////////////////////////////////////////////////////////

void Cylinder2DDustGrid::path(DustGridPath* path) const
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
    double Rmax = maxR();
    double zmin = minZ();
    double zmax = maxZ();

    // Move the photon package to the first grid cell that it will pass.
    // If it does not pass any grid cell, return an empty path.
    // Otherwise calculate the distance covered and add a segment to the path.

    if (R>=Rmax)
    {
        if (q>0.0 || p>Rmax) return path->clear();
        else
        {
            R = Rmax - 1e-8*(_Rv[_NR]-_Rv[_NR-1]);
            double qmax = sqrt((Rmax-p)*(Rmax+p));
            double ds = (qmax-q)/kq;
            path->addSegment(-1,ds);
            q = qmax;
            z += kz*ds;
        }
    }
    if (z<zmin)
    {
        if (kz<=0.0) return path->clear();
        else
        {
            double ds = (zmin-z)/kz;
            path->addSegment(-1,ds);
            q += kq*ds;
            R = sqrt(p*p+q*q);
            z = zmin + 1e-8*(_zv[1]-_zv[0]);
        }
    }
    else if (z>zmax)
    {
        if (kz>=0.0) return path->clear();
        else
        {
            double ds = (zmax-z)/kz;
            path->addSegment(-1,ds);
            q += kq*ds;
            R = sqrt(p*p+q*q);
            z = zmax - 1e-8*(_zv[_Nz]-_zv[_Nz-1]);
        }
    }
    if (std::isinf(R) || std::isnan(R) || std::isinf(z) || std::isnan(z) ||
        R>=Rmax || z<=zmin || z>=zmax) return path->clear();

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

void Cylinder2DDustGrid::write_xy(DustGridPlotFile* outfile) const
{
    for (int i=0; i<=_NR; i++) outfile->writeCircle(_Rv[i]);
}

//////////////////////////////////////////////////////////////////////

void Cylinder2DDustGrid::write_xz(DustGridPlotFile* outfile) const
{
    double Rmax = maxR();
    double zmin = minZ();
    double zmax = maxZ();
    for (int i=0; i<=_NR; i++)
    {
        outfile->writeLine(_Rv[i], zmin, _Rv[i], zmax);
        outfile->writeLine(-_Rv[i], zmin, -_Rv[i], zmax);
    }
    for (int k=0; k<=_Nz; k++)
    {
        outfile->writeLine(-Rmax, _zv[k], Rmax, _zv[k]);
    }
}

//////////////////////////////////////////////////////////////////////

int Cylinder2DDustGrid::index(int i, int k) const
{
    return k+_Nz*i;
}

//////////////////////////////////////////////////////////////////////

void Cylinder2DDustGrid::invertindex(int m, int& i, int& k) const
{
    i = m / _Nz;
    k = m % _Nz;
}

//////////////////////////////////////////////////////////////////////
