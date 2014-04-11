/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include <cfloat>
#include <cmath>
#include <cstdlib>
#include "AxSpheDustGridStructure.hpp"
#include "DustGridPath.hpp"
#include "DustGridPlotFile.hpp"
#include "FatalError.hpp"
#include "Log.hpp"
#include "Random.hpp"

using namespace std;

//////////////////////////////////////////////////////////////////////

AxSpheDustGridStructure::AxSpheDustGridStructure()
    : _Nr(0), _Ntheta(0), _rmax(0)
{
}

//////////////////////////////////////////////////////////////////////

void AxSpheDustGridStructure::setupSelfAfter()
{
    DustGridStructure::setupSelfAfter();

    // pre-calculate the cosines for each angular boundary
    // the xy-plane must occur exactly once (the code in path() checks for a zero c value)
    _cv.resize(_Ntheta+1);
    _cv[0] = 1.;
    _cv[_Ntheta] = -1.;
    int countzeroes = 0;
    for (int k=1; k<_Ntheta; k++)
    {
        _cv[k] = cos(_thetav[k]);
        if (fabs(_cv[k]) < 1e-9)
        {
            countzeroes++;
            _cv[k] = 0.;
        }
    }
    if (countzeroes != 1) throw FATALERROR("the grid point with theta = pi/2 must occur exactly once");
}

//////////////////////////////////////////////////////////////////////

int AxSpheDustGridStructure::dimension() const
{
    return 2;
}

//////////////////////////////////////////////////////////////////////

double AxSpheDustGridStructure::xmax() const
{
    return _rmax;
}

//////////////////////////////////////////////////////////////////////

double AxSpheDustGridStructure::ymax() const
{
    return _rmax;
}

//////////////////////////////////////////////////////////////////////

double AxSpheDustGridStructure::zmax() const
{
    return _rmax;
}

//////////////////////////////////////////////////////////////////////

double AxSpheDustGridStructure::volume(int m) const
{
    int i, k;
    invertindex(m,i,k);
    if (i<0 || i>=_Nr || k<0 || k>=_Ntheta)
        return 0.0;
    else
        return (2.0/3.0)*M_PI * (pow(_rv[i+1],3)-pow(_rv[i],3)) * (cos(_thetav[k])-cos(_thetav[k+1]));
}

//////////////////////////////////////////////////////////////////////

int AxSpheDustGridStructure::whichcell(Position bfr) const
{
    double r, theta, phi;
    bfr.spherical(r, theta, phi);
    int i = whichrcell(r);
    if (i>=_Nr) return -1;
    int k = whichthetacell(theta);
    return index(i,k);
}

//////////////////////////////////////////////////////////////////////

Position AxSpheDustGridStructure::centralPositionInCell(int m) const
{
    int i, k;
    invertindex(m,i,k);
    double r = (_rv[i]+_rv[i+1])/2.0;
    double theta = (_thetav[k]+_thetav[k+1])/2.0;
    double phi = 0.0;
    return Position(r,theta,phi,Position::SPHERICAL);
}

//////////////////////////////////////////////////////////////////////

Position AxSpheDustGridStructure::randomPositionInCell(int m) const
{
    int i, k;
    invertindex(m,i,k);
    double ris = _rv[i]*_rv[i];
    double ri1s = _rv[i+1]*_rv[i+1];
    double r = sqrt( ris + (ri1s-ris)*_random->uniform() );
    double theta = _thetav[k] + (_thetav[k+1]-_thetav[k])*_random->uniform();
    double phi = 2.0*M_PI*_random->uniform();
    return Position(r,theta,phi,Position::SPHERICAL);
}

//////////////////////////////////////////////////////////////////////

namespace
{
    // returns the smallest positive solution of x^2 + 2*b*x + c = 0, or 0 if there is no positive solution
    double smallestPositiveSolution(double b, double c)
    {
        // x1 == -b - sqrt(b*b-c)
        // x2 == -b + sqrt(b*b-c)
        // x1*x2 == c

        if (b*b > c)            // if discriminant is negative, there are no real solutions
        {
            if (b > 0)          // x1 is always negative; x2 is positive only if c<0
            {
                if (c < 0)
                {
                    double x1 = -b - sqrt(b*b-c);
                    return c / x1;
                }
            }
            else                // x2 is always positive; x1 is positive only if c>0
            {
                double x2 = -b + sqrt(b*b-c);
                if (c > 0)
                {
                    double x1 = c / x2;
                    if (x1 < x2) return x1;
                }
                return x2;
            }
        }
        return 0;
    }

    // returns the smallest positive solution of a*x^2 + 2*b*x + c = 0, or 0 if there is no positive solution
    double smallestPositiveSolution(double a, double b, double c)
    {
        if (fabs(a) > 1e-9) return smallestPositiveSolution(b/a, c/a);
        double x = -0.5 * c / b;
        if (x > 0) return x;
        return 0;
    }

    // returns the distance to the first intersection between the ray (bfr,bfk) and the sphere with given radius,
    // or 0 if there is no intersection
    double firstIntersectionSphere(Vec bfr, Vec bfk, double r)
    {
        return smallestPositiveSolution(Vec::dot(bfr,bfk), bfr.norm2()-r*r);
    }

    // returns the distance to the first intersection between the ray (bfr,bfk) and the cone with given cos(theta),
    // or 0 if there is no intersection (the degenarate cone with zero cosine is treated separately)
    double firstIntersectionCone(Vec bfr, Vec bfk, double c)
    {
        return c ? smallestPositiveSolution( c*c                   - bfk.z()*bfk.z(),
                                             c*c*Vec::dot(bfr,bfk) - bfr.z()*bfk.z(),
                                             c*c*bfr.norm2()       - bfr.z()*bfr.z() )
                 : -bfr.z()/bfk.z();  // degenerate cone identical to xy-plane
    }
}

//////////////////////////////////////////////////////////////////////

DustGridPath AxSpheDustGridStructure::path(Position bfr, Direction bfk) const
{
    // Small value relative to domain size
    const double eps = 1e-11 * _rmax;

    // Construct the path with its initial position and direction
    DustGridPath path(bfr, bfk, 2*_Nr + _Ntheta + 2);

    // Move the photon package to the first grid cell that it will pass.
    // If it does not pass any grid cell, return an empty path.
    // Otherwise calculate the distance covered and add a segment to the path.
    double r2 = bfr.norm2();
    if (r2 > _rmax*_rmax)
    {
        double ds = firstIntersectionSphere(bfr, bfk, _rmax);
        if (!ds) return path.clear();
        path.addsegment(-1, ds);
        bfr += bfk*(ds+eps);
    }
    // Move the position a bit away from the origin so that it has a meaningful cell number
    else if (r2==0)
    {
        bfr += bfk*eps;
    }

    // Determine the indices of the cell containing the starting point.
    // In the rare cases that the point is still outside the grid, return an empty path.
    double r, theta, phi;
    bfr.spherical(r, theta, phi);
    int i = whichrcell(r);
    int k = whichthetacell(theta);
    if (i >= _Nr) return path.clear();

    // Start the loop over cells/path segments until we leave the grid
    int inext = i;
    int knext = k;
    while (i < _Nr)
    {
        // Calculate the distance travelled inside the cell by considering
        // the potential exit points for each of the four cell boundaries;
        // the smallest positive intersection "distance" wins.
        double ds = DBL_MAX;    // very large, but not infinity (so that infinite values are discarded)

        // inner radial boundary (not applicable to innermost cell)
        if (i > 0)
        {
            double s = firstIntersectionSphere(bfr, bfk, _rv[i]);
            if (s > 0 && s < ds)
            {
                ds = s;
                inext = i-1;
                knext = k;
            }
        }

        // outer radial boundary (always applicable)
        {
            double s = firstIntersectionSphere(bfr, bfk, _rv[i+1]);
            if (s > 0 && s < ds)
            {
                ds = s;
                inext = i+1;  // this will cause the loop to terminate if incremented beyond the outermost boundary
                knext = k;
            }
        }

        // upper angular boundary (not applicable to uppermost cell)
        if (k > 0)
        {
            double s = firstIntersectionCone(bfr, bfk, _cv[k]);
            if (s > 0 && s < ds)
            {
                ds = s;
                inext = i;
                knext = k-1;
            }
        }

        // lower angular boundary (not applicable to lowest cell)
        if (k < _Ntheta-1)
        {
            double s = firstIntersectionCone(bfr, bfk, _cv[k+1]);
            if (s > 0 && s < ds)
            {
                ds = s;
                inext = i;
                knext = k+1;
            }
        }

        // If an exit point was found, add a segment to the path,
        // move to the next current point, and update the cell indices
        if (inext!=i || knext!=k)
        {
            path.addsegment(index(i,k), ds);
            bfr += bfk*(ds+eps);
            i = inext;
            k = knext;
        }
        // Otherwise, move a tiny bit along the path and reset the current cell indices
        else
        {
            find<Log>()->warning("No exit point found from dust grid cell");
            bfr += bfk*eps;
            double r, theta, phi;
            bfr.spherical(r, theta, phi);
            i = whichrcell(r);
            k = whichthetacell(theta);
        }
    }

    return path;
}

//////////////////////////////////////////////////////////////////////

void AxSpheDustGridStructure::write_xy(DustGridPlotFile* outfile) const
{
    for (int i=0; i<=_Nr; i++) outfile->writeCircle(_rv[i]);
}

//////////////////////////////////////////////////////////////////////

void AxSpheDustGridStructure::write_xz(DustGridPlotFile* outfile) const
{
    for (int i=0; i<=_Nr; i++)
    {
        outfile->writeCircle(_rv[i]);
    }
    for (int k=0; k<=_Ntheta; k++)
    {
        double x = _rmax * sin(_thetav[k]);
        double z = _rmax * cos(_thetav[k]);
        outfile->writeLine(-x, -z, x, z);
    }
}

//////////////////////////////////////////////////////////////////////

int AxSpheDustGridStructure::whichrcell(double r) const
{
    if (r<0.0) return -1;
    else if (r>_rmax) return _Nr;
    int il=-1, iu=_Nr, im;
    while (iu-il>1)
    {
        im = (iu+il)>>1;
        if (r>=_rv[im])
            il = im;
        else
            iu = im;
    }
    return il;
}

//////////////////////////////////////////////////////////////////////

int AxSpheDustGridStructure::whichthetacell(double theta) const
{
    if (theta<=0) return 0;
    int kl=-1, ku=_Ntheta, km;
    while (ku-kl>1)
    {
        km = (ku+kl)>>1;
        if (theta>=_thetav[km])
            kl = km;
        else
            ku = km;
    }
    return kl;
}

//////////////////////////////////////////////////////////////////////

int AxSpheDustGridStructure::index(int i, int k) const
{
    return k+_Ntheta*i;
}

//////////////////////////////////////////////////////////////////////

void AxSpheDustGridStructure::invertindex(int m, int& i, int& k) const
{
    i = m / _Ntheta;
    k = m % _Ntheta;
}

//////////////////////////////////////////////////////////////////////
