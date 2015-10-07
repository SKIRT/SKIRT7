/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "TwoPhaseDustGrid.hpp"
#include "FatalError.hpp"
#include "Random.hpp"

using namespace std;

//////////////////////////////////////////////////////////////////////

TwoPhaseDustGrid::TwoPhaseDustGrid()
    : _contrast(0), _fillingfactor(0), _weightv(0)
{
}

////////////////////////////////////////////////////////////////////

void TwoPhaseDustGrid::setupSelfAfter()
{
    CartesianDustGrid::setupSelfAfter();
    Random* random = find<Random>();

    // verify property values
    if (_fillingfactor <= 0 || _fillingfactor >= 1)
        throw FATALERROR("the volume filling factor of the high-density medium should be between 0 and 1");
    if (_contrast <= 0)
        throw FATALERROR("the density contrast between the high- and low-density medium should be positive");

    // construction of the weight matrix
    _weightv.resize(numCells());
    for (int m=0; m<numCells(); m++)
    {
        double X = random->uniform();
        _weightv[m] = (X<_fillingfactor) ?
                          _contrast/(_contrast*_fillingfactor+1.0-_fillingfactor) :
                          1.0/(_contrast*_fillingfactor+1.0-_fillingfactor);
    }
}

////////////////////////////////////////////////////////////////////

void TwoPhaseDustGrid::setFillingFactor(double value)
{
    _fillingfactor = value;
}

////////////////////////////////////////////////////////////////////

double TwoPhaseDustGrid::fillingFactor() const
{
    return _fillingfactor;
}

////////////////////////////////////////////////////////////////////

void TwoPhaseDustGrid::setContrast(double value)
{
    _contrast = value;
}

////////////////////////////////////////////////////////////////////

double TwoPhaseDustGrid::contrast() const
{
    return _contrast;
}

//////////////////////////////////////////////////////////////////////

double TwoPhaseDustGrid::weight(int m) const
{
    return (m==-1) ? 0.0 : _weightv[m];
}

//////////////////////////////////////////////////////////////////////
