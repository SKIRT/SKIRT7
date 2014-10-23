/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include <cfloat>
#include <cmath>
#include <cstdlib>
#include "Box.hpp"
#include "CubDustGridStructure.hpp"
#include "DustGridPath.hpp"
#include "DustGridPlotFile.hpp"
#include "FatalError.hpp"
#include "NR.hpp"
#include "Random.hpp"

using namespace std;

//////////////////////////////////////////////////////////////////////

CubDustGridStructure::CubDustGridStructure()
    : _Nx(0), _Ny(0), _Nz(0), _xmin(0), _xmax(0), _ymin(0), _ymax(0), _zmin(0), _zmax(0)
{
}

//////////////////////////////////////////////////////////////////////

double CubDustGridStructure::xmax() const
{
    return max(fabs(_xmin),fabs(_xmax));
}

//////////////////////////////////////////////////////////////////////

double CubDustGridStructure::ymax() const
{
    return max(fabs(_ymin),fabs(_ymax));
}

//////////////////////////////////////////////////////////////////////

double CubDustGridStructure::zmax() const
{
    return max(fabs(_zmin),fabs(_zmax));
}

//////////////////////////////////////////////////////////////////////

double CubDustGridStructure::volume(int m) const
{
    return box(m).volume();
}

//////////////////////////////////////////////////////////////////////

int CubDustGridStructure::whichcell(Position bfr) const
{
    int i = NR::locate_fail(_xv, bfr.x());
    int j = NR::locate_fail(_yv, bfr.y());
    int k = NR::locate_fail(_zv, bfr.z());
    if (i<0 || j<0 || k<0)
        return -1;
    else
        return index(i,j,k);
}

//////////////////////////////////////////////////////////////////////

Position CubDustGridStructure::centralPositionInCell(int m) const
{
    return Position(box(m).center());
}

//////////////////////////////////////////////////////////////////////

Position CubDustGridStructure::randomPositionInCell(int m) const
{
    return _random->position(box(m));
}

//////////////////////////////////////////////////////////////////////

void CubDustGridStructure::path(DustGridPath* path) const
{
    // Determination of the initial position and direction of the path,
    // and calculation of some initial values

    path->clear();
    double kx,ky,kz;
    path->direction().cartesian(kx,ky,kz);
    double x,y,z;
    path->position().cartesian(x,y,z);
    double ds, dsx, dsy, dsz;

    // move the photon package to the first grid cell that it will
    // pass. If it does not pass any grid cell, return an empty path.

    if (x<_xmin)
    {
        if (kx<=0.0) return path->clear();
        else
        {
            ds = (_xmin-x)/kx;
            path->addSegment(-1,ds);
            x = _xmin + 1e-8*(_xv[1]-_xv[0]);
            y += ky*ds;
            z += kz*ds;
        }
    }
    else if (x>_xmax)
    {
        if (kx>=0.0) return path->clear();
        else
        {
            ds = (_xmax-x)/kx;
            path->addSegment(-1,ds);
            x = _xmax - 1e-8*(_xv[_Nx]-_xv[_Nx-1]);
            y += ky*ds;
            z += kz*ds;
        }
    }
    if (y<_ymin)
    {
        if (ky<=0.0) return path->clear();
        else
        {
            ds = (_ymin-y)/ky;
            path->addSegment(-1,ds);
            x += kx*ds;
            y = _ymin + 1e-8*(_yv[1]-_yv[0]);
            z += kz*ds;
        }
    }
    else if (y>_ymax)
    {
        if (ky>=0.0) return path->clear();
        else
        {
            ds = (_ymax-y)/ky;
            path->addSegment(-1,ds);
            x += kx*ds;
            y = _ymax - 1e-8*(_yv[_Ny]-_yv[_Ny-1]);
            z += kz*ds;
        }
    }
    if (z<_zmin)
    {
        if (kz<=0.0) return path->clear();
        else
        {
            ds = (_zmin-z)/kz;
            path->addSegment(-1,ds);
            x += kx*ds;
            y += ky*ds;
            z = _zmin + 1e-8*(_zv[1]-_zv[0]);
        }
    }
    else if (z>_zmax)
    {
        if (kz>=0.0) return path->clear();
        else
        {
            ds = (_zmax-z)/kz;
            path->addSegment(-1,ds);
            x += kx*ds;
            y += ky*ds;
            z = _zmax - 1e-8*(_zv[_Nz]-_zv[_Nz-1]);
        }
    }

    if (x<_xmin || x>_xmax || y<_ymin || y>_ymax || z<_zmin || z>_zmax) return path->clear();

    // Now determine which grid cell we are in...

    int i = NR::locate_clip(_xv, x);
    int j = NR::locate_clip(_yv, y);
    int k = NR::locate_clip(_zv, z);

    // And there we go...

    while (true)
    {
        int m = index(i,j,k);
        double xE = (kx<0.0) ? _xv[i] : _xv[i+1];
        double yE = (ky<0.0) ? _yv[j] : _yv[j+1];
        double zE = (kz<0.0) ? _zv[k] : _zv[k+1];
        dsx = (fabs(kx)>1e-15) ? (xE-x)/kx : DBL_MAX;
        dsy = (fabs(ky)>1e-15) ? (yE-y)/ky : DBL_MAX;
        dsz = (fabs(kz)>1e-15) ? (zE-z)/kz : DBL_MAX;
        if (dsx<=dsy && dsx<=dsz)
        {
            ds = dsx;
            path->addSegment(m, ds);
            i += (kx<0.0) ? -1 : 1;
            if (i>=_Nx || i<0) return;
            else
            {
                x = xE;
                y += ky*ds;
                z += kz*ds;
            }
        }
        else if (dsy< dsx && dsy<=dsz)
        {
            ds = dsy;
            path->addSegment(m, ds);
            j += (ky<0.0) ? -1 : 1;
            if (j>=_Ny || j<0) return;
            else
            {
                x += kx*ds;
                y = yE;
                z += kz*ds;
            }
        }
        else if (dsz< dsx && dsz< dsy)
        {
            ds = dsz;
            path->addSegment(m, ds);
            k += (kz<0.0) ? -1 : 1;
            if (k>=_Nz || k<0) return;
            else
            {
                x += kx*ds;
                y += ky*ds;
                z  = zE;
            }
        }
    }
}

//////////////////////////////////////////////////////////////////////

void CubDustGridStructure::write_xy(DustGridPlotFile* outfile) const
{
    for (int i=0; i<=_Nx; i++) outfile->writeLine(_xv[i], _ymin, _xv[i], _ymax);
    for (int j=0; j<=_Ny; j++) outfile->writeLine(_xmin, _yv[j], _xmax, _yv[j]);
}

//////////////////////////////////////////////////////////////////////

void CubDustGridStructure::write_xz(DustGridPlotFile* outfile) const
{
    for (int i=0; i<=_Nx; i++) outfile->writeLine(_xv[i], _zmin, _xv[i], _zmax);
    for (int k=0; k<=_Nz; k++) outfile->writeLine(_xmin, _zv[k], _xmax, _zv[k]);
}

//////////////////////////////////////////////////////////////////////

void CubDustGridStructure::write_yz(DustGridPlotFile* outfile) const
{
    for (int j=0; j<=_Ny; j++) outfile->writeLine(_yv[j], _zmin, _yv[j], _zmax);
    for (int k=0; k<=_Nz; k++) outfile->writeLine(_ymin, _zv[k], _ymax, _zv[k]);
}

//////////////////////////////////////////////////////////////////////

void CubDustGridStructure::write_xyz(DustGridPlotFile* outfile) const
{
    for (int i=0; i<=_Nx; i++)
        for (int j=0; j<=_Ny; j++)
            outfile->writeLine(_xv[i], _yv[j], _zmin, _xv[i], _yv[j], _zmax);
    for (int i=0; i<=_Nx; i++)
        for (int k=0; k<=_Nz; k++)
            outfile->writeLine(_xv[i], _ymin, _zv[k], _xv[i], _ymax, _zv[k]);
    for (int j=0; j<=_Ny; j++)
        for (int k=0; k<=_Nz; k++)
            outfile->writeLine(_xmin, _yv[j], _zv[k], _xmax, _yv[j], _zv[k]);
}

//////////////////////////////////////////////////////////////////////

int CubDustGridStructure::index(int i, int j, int k) const
{
    return k+_Nz*j+_Nz*_Ny*i;
}

//////////////////////////////////////////////////////////////////////

Box CubDustGridStructure::box(int m) const
{
    int i = m / (_Nz*_Ny);
    int j = (m/_Nz) % _Ny;
    int k = m % _Nz;

    if (i<0 || j<0 || k<0 || i>=_Nx || j>=_Ny || k>=_Nz)
        return Box();
    else
        return Box(_xv[i],_yv[j],_zv[k], _xv[i+1],_yv[j+1],_zv[k+1]);
}

//////////////////////////////////////////////////////////////////////
