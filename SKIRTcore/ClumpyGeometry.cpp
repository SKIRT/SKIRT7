/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "ClumpyGeometry.hpp"
#include "FatalError.hpp"
#include "NR.hpp"
#include "Random.hpp"

using namespace std;

////////////////////////////////////////////////////////////////////

ClumpyGeometry::ClumpyGeometry()
    : _geometry(0), _f(0), _N(0), _h(0), _cutoff(false), _kernel(0)
{
}

//////////////////////////////////////////////////////////////////////

void ClumpyGeometry::setupSelfBefore()
{
    GenGeometry::setupSelfBefore();

    // verify property values
    if (_f < 0.0 || _f > 1.0)
        throw FATALERROR("the fraction of the mass locked up in clumps must be between 0 and 1");
    if (_N <= 0) throw FATALERROR("the total number of clumps should be positive");
    if (_h <= 0) throw FATALERROR("the scale radius of a single clump should be positive");
}

//////////////////////////////////////////////////////////////////////

void ClumpyGeometry::setupSelfAfter()
{
    GenGeometry::setupSelfAfter();

    // generate the random positions of the clumps
    _clumpv.resize(_N);
    for (int i=0; i<_N; i++)
        _clumpv[i] = _geometry->generatePosition();

    // sort the vector with the positions of the clumps in increasing x-coordinates
    NR::sort(_clumpv);
}

////////////////////////////////////////////////////////////////////

void ClumpyGeometry::setGeometry(Geometry* value)
{
    if (_geometry) delete _geometry;
    _geometry = value;
    if (_geometry) _geometry->setParent(this);
}

////////////////////////////////////////////////////////////////////

Geometry* ClumpyGeometry::geometry() const
{
    return _geometry;
}

////////////////////////////////////////////////////////////////////

void ClumpyGeometry::setClumpFraction(double value)
{
    _f = value;
}

////////////////////////////////////////////////////////////////////

double ClumpyGeometry::clumpFraction() const
{
    return _f;
}

////////////////////////////////////////////////////////////////////

void ClumpyGeometry::setClumpCount(int value)
{
    _N = value;
}

////////////////////////////////////////////////////////////////////

int ClumpyGeometry::clumpCount() const
{
    return _N;
}

////////////////////////////////////////////////////////////////////

void ClumpyGeometry::setClumpRadius(double value)
{
    _h = value;
}

////////////////////////////////////////////////////////////////////

double ClumpyGeometry::clumpRadius() const
{
    return _h;
}

////////////////////////////////////////////////////////////////////

void ClumpyGeometry::setCutoff(bool value)
{
    _cutoff = value;
}

////////////////////////////////////////////////////////////////////

bool ClumpyGeometry::cutoff() const
{
    return _cutoff;
}

////////////////////////////////////////////////////////////////////

void ClumpyGeometry::setKernel(SmoothingKernel* value)
{
    if (_kernel) delete _kernel;
    _kernel = value;
    if (_kernel) _kernel->setParent(this);
}

////////////////////////////////////////////////////////////////////

SmoothingKernel* ClumpyGeometry::kernel() const
{
    return _kernel;
}

////////////////////////////////////////////////////////////////////

double ClumpyGeometry::density(Position bfr) const
{
    double rhosmooth = (1.0-_f) * _geometry->density(bfr);
    if (_cutoff && !rhosmooth) return 0.0;  // don't allow clumps outside of smooth distribution

    double rhoclumpy = 0.0;
    double Mclump = _f/_N; // total mass per clump
    int istart = max(0, NR::locate(_clumpv,bfr-Vec(_h,0,0)));
    int iend = max(0, NR::locate(_clumpv,bfr+Vec(_h,0,0)));
    for (int i=istart; i<=iend; i++)
    {
        double u = (bfr-_clumpv[i]).norm() / _h;
        rhoclumpy += Mclump * _kernel->density(u)/pow(_h,3);
    }

    return rhosmooth + rhoclumpy;
}

////////////////////////////////////////////////////////////////////

Position ClumpyGeometry::generatePosition() const
{
    // loop until an appropriate position has been found
    while (true)
    {
        double X = _random->uniform();
        if (X>_f)
            return _geometry->generatePosition();
        else
        {
            int i = min(static_cast<int>((X/_f)*_N), _N-1); // random clump number based on X
            double u = _kernel->generateRadius();
            Direction bfk(_random->direction());
            Position bfr(_clumpv[i] + u*_h*bfk);

            // reject positions outside of smooth distribution
            if (!_cutoff || _geometry->density(bfr)) return bfr;
        }
    }
}

////////////////////////////////////////////////////////////////////

double ClumpyGeometry::SigmaX() const
{
    return _geometry->SigmaX();
}

////////////////////////////////////////////////////////////////////

double ClumpyGeometry::SigmaY() const
{
    return _geometry->SigmaY();
}

////////////////////////////////////////////////////////////////////

double ClumpyGeometry::SigmaZ() const
{
    return _geometry->SigmaZ();
}

////////////////////////////////////////////////////////////////////
