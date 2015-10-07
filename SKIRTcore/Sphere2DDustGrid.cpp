/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include <cfloat>
#include <cmath>
#include <cstdlib>
#include "Sphere2DDustGrid.hpp"
#include "DustGridPath.hpp"
#include "DustGridPlotFile.hpp"
#include "FatalError.hpp"
#include "Log.hpp"
#include "NR.hpp"
#include "Random.hpp"

using namespace std;

//////////////////////////////////////////////////////////////////////

Sphere2DDustGrid::Sphere2DDustGrid()
    : _random(0), _Nr(0), _Ntheta(0), _meshr(0), _meshtheta(0), _rv(0), _thetav(0)
{
}

//////////////////////////////////////////////////////////////////////

void Sphere2DDustGrid::setupSelfAfter()
{
    // Cache the random number generator
    _random = find<Random>();

    // Set up the radial grid
    _Nr = _meshr->numBins();
    double rmax = maxR();
    _rv = _meshr->mesh() * rmax;

    // Set up the polar grid. One particular curiosity of the Sphere2DDustGrid class is that the
    // path() function requires that there is a grid point corresponding to pi=pi/2 (i.e. the xy-plane).
    // So if it is not there, add it. We are using the vector class since there is no mechanism foreseen
    // in the Array class to insert data points.

    int Nt = _meshtheta->numBins();
    vector<double> tv(Nt+1);
    for (int k=0; k<=Nt; k++) tv[k] = _meshtheta->meshpoint(k);
    bool halfinmesh = false;
    for (int k=0; k<=Nt; k++)
        if (fabs(_meshtheta->meshpoint(k)-0.5) < 1e-11)
        {
            halfinmesh = true;
            break;
        }
    if (!halfinmesh)
    {
        tv.push_back(0.5);
        std::sort(tv.begin(),tv.end());
    }
    _Ntheta = tv.size();
    _thetav.resize(_Ntheta+1);
    for (int k=0; k<=_Ntheta; k++)
        _thetav[k] = tv[k]*M_PI;
    _cv = cos(_thetav);

    // Call the setupSelfAfter function for the SphereDustGrid base class
    SphereDustGrid::setupSelfAfter();
}

//////////////////////////////////////////////////////////////////////

void Sphere2DDustGrid::setMeshR(Mesh* value)
{
    if (_meshr) delete _meshr;
    _meshr = value;
    if (_meshr) _meshr->setParent(this);
}

//////////////////////////////////////////////////////////////////////

Mesh* Sphere2DDustGrid::meshR() const
{
    return _meshr;
}

//////////////////////////////////////////////////////////////////////

void Sphere2DDustGrid::setMeshTheta(Mesh* value)
{
    if (_meshtheta) delete _meshtheta;
    _meshtheta = value;
    if (_meshtheta) _meshtheta->setParent(this);
}

//////////////////////////////////////////////////////////////////////

Mesh* Sphere2DDustGrid::meshTheta() const
{
    return _meshtheta;
}

//////////////////////////////////////////////////////////////////////

int Sphere2DDustGrid::dimension() const
{
    return 2;
}

//////////////////////////////////////////////////////////////////////

double Sphere2DDustGrid::volume(int m) const
{
    int i, k;
    invertindex(m,i,k);
    if (i<0 || i>=_Nr || k<0 || k>=_Ntheta)
        return 0.0;
    else
        return (2.0/3.0)*M_PI * (pow(_rv[i+1],3)-pow(_rv[i],3)) * (cos(_thetav[k])-cos(_thetav[k+1]));
}

//////////////////////////////////////////////////////////////////////

int Sphere2DDustGrid::whichcell(Position bfr) const
{
    double r, theta, phi;
    bfr.spherical(r, theta, phi);
    int i = NR::locate_fail(_rv, r);
    if (i<0) return -1;
    int k = NR::locate_clip(_thetav, theta);
    return index(i,k);
}

//////////////////////////////////////////////////////////////////////

Position Sphere2DDustGrid::centralPositionInCell(int m) const
{
    int i, k;
    invertindex(m,i,k);
    double r = (_rv[i]+_rv[i+1])/2.0;
    double theta = (_thetav[k]+_thetav[k+1])/2.0;
    double phi = 0.0;
    return Position(r,theta,phi,Position::SPHERICAL);
}

//////////////////////////////////////////////////////////////////////

Position Sphere2DDustGrid::randomPositionInCell(int m) const
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

void Sphere2DDustGrid::path(DustGridPath* path) const
{
    // Small value relative to domain size
    double rmax = maxR();
    const double eps = 1e-11 * rmax;

    // Initialize the path
    path->clear();
    Position bfr = path->position();
    Direction bfk = path->direction();

    // Move the photon package to the first grid cell that it will pass.
    // If it does not pass any grid cell, return an empty path.
    // Otherwise calculate the distance covered and add a segment to the path.
    double r2 = bfr.norm2();
    if (r2 > rmax*rmax)
    {
        double ds = firstIntersectionSphere(bfr, bfk, rmax);
        if (!ds) return path->clear();
        path->addSegment(-1, ds);
        bfr += bfk*(ds+eps);
    }
    // Move the position a bit away from the origin so that it has a meaningful cell number
    else if (r2==0)
    {
        bfr += bfk*eps;
    }

    // Determine the indices of the cell containing the starting point.
    double r, theta, phi;
    bfr.spherical(r, theta, phi);
    int i = NR::locate_fail(_rv, r);
    int k = NR::locate_clip(_thetav, theta);

    // Start the loop over cells/path segments until we leave the grid
    int inext = i;
    int knext = k;
    while (i < _Nr && i >= 0)
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
            path->addSegment(index(i,k), ds);
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
            i = NR::locate_fail(_rv, r);
            k = NR::locate_clip(_thetav, theta);
        }
    }
}

//////////////////////////////////////////////////////////////////////

void Sphere2DDustGrid::write_xy(DustGridPlotFile* outfile) const
{
    for (int i=0; i<=_Nr; i++) outfile->writeCircle(_rv[i]);
}

//////////////////////////////////////////////////////////////////////

void Sphere2DDustGrid::write_xz(DustGridPlotFile* outfile) const
{
    double rmax = maxR();
    for (int i=0; i<=_Nr; i++)
    {
        outfile->writeCircle(_rv[i]);
    }
    for (int k=0; k<=_Ntheta; k++)
    {
        double x = rmax * sin(_thetav[k]);
        double z = rmax * cos(_thetav[k]);
        outfile->writeLine(-x, -z, x, z);
    }
}

//////////////////////////////////////////////////////////////////////

int Sphere2DDustGrid::index(int i, int k) const
{
    return k+_Ntheta*i;
}

//////////////////////////////////////////////////////////////////////

void Sphere2DDustGrid::invertindex(int m, int& i, int& k) const
{
    i = m / _Ntheta;
    k = m % _Ntheta;
}

//////////////////////////////////////////////////////////////////////
