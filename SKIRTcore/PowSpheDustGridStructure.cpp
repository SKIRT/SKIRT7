/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include <cmath>
#include "FatalError.hpp"
#include "PowSpheDustGridStructure.hpp"

using namespace std;

//////////////////////////////////////////////////////////////////////

PowSpheDustGridStructure::PowSpheDustGridStructure()
    : _ratio(0)
{
}

//////////////////////////////////////////////////////////////////////

void PowSpheDustGridStructure::setupSelfBefore()
{
    SpheDustGridStructure::setupSelfBefore();

    // verify property values
    if (_rmax <= 0) throw FATALERROR("the outer radius rmax should be positive");
    if (_ratio <= 0) throw FATALERROR("the ratio of the inner- and outermost bin widths ratio should be positive");
    if (_Nr <= 0) throw FATALERROR("the number of radial grid points Nr should be positive");

    // grid distribution in r
    _rv.resize(_Nr+1);
    if (fabs(_ratio-1.0)<1e-3)
    {
        for (int i=0; i<=_Nr; i++)
            _rv[i] = i*_rmax/_Nr;
    }
    else
    {
        double q = pow(_ratio,1.0/(_Nr-1.0));
        for (int i=0; i<=_Nr; ++i)
            _rv[i] = (1.0-pow(q,i))/(1.0-pow(q,_Nr)) * _rmax;
    }

    // the total number of cells
    _Ncells = _Nr;
}

////////////////////////////////////////////////////////////////////

void PowSpheDustGridStructure::setExtent(double value)
{
    _rmax = value;
}

////////////////////////////////////////////////////////////////////

double PowSpheDustGridStructure::extent() const
{
    return _rmax;
}

////////////////////////////////////////////////////////////////////

void PowSpheDustGridStructure::setRatio(double value)
{
    _ratio = value;
}

////////////////////////////////////////////////////////////////////

double PowSpheDustGridStructure::ratio() const
{
    return _ratio;
}

////////////////////////////////////////////////////////////////////

void PowSpheDustGridStructure::setPoints(int value)
{
    _Nr = value;
}

////////////////////////////////////////////////////////////////////

int PowSpheDustGridStructure::points() const
{
    return _Nr;
}

//////////////////////////////////////////////////////////////////////
