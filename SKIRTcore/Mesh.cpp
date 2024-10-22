/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "FatalError.hpp"
#include "Mesh.hpp"

using namespace std;

////////////////////////////////////////////////////////////////////

Mesh::Mesh()
    : _N(0)
{
}

////////////////////////////////////////////////////////////////////

void Mesh::setupSelfBefore()
{
    SimulationItem::setupSelfBefore();
    if (_N <= 0) throw FATALERROR("the number of bins should be positive");
}

////////////////////////////////////////////////////////////////////

void Mesh::setNumBins(int value)
{
    _N = value;
}

////////////////////////////////////////////////////////////////////

int Mesh::numBins() const
{
    return _N;
}

////////////////////////////////////////////////////////////////////

