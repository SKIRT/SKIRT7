/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include "DustMixPopulation.hpp"
#include "FatalError.hpp"
#include "GrainComposition.hpp"
#include "GrainSizeDistribution.hpp"

////////////////////////////////////////////////////////////////////

DustMixPopulation::DustMixPopulation()
    : _gc(0), _gs(0), _Nbins(0)
{
}

////////////////////////////////////////////////////////////////////

void DustMixPopulation::setupSelfBefore()
{
    SimulationItem::setupSelfBefore();

    // verify the attributes
    if (!_gc) throw FATALERROR("The grain composition is missing");
    if (!_gs) throw FATALERROR("The grain size distribution is missing");
    if (_Nbins < 1) throw FATALERROR("There must be at least one grain size bin");
}

////////////////////////////////////////////////////////////////////

void DustMixPopulation::setComposition(GrainComposition* value)
{
    if (_gc) delete _gc;
    _gc = value;
    if (_gc) _gc->setParent(this);
}

////////////////////////////////////////////////////////////////////

GrainComposition* DustMixPopulation::composition() const
{
    return _gc;
}

////////////////////////////////////////////////////////////////////

void DustMixPopulation::setSizeDistribution(GrainSizeDistribution* value)
{
    if (_gs) delete _gs;
    _gs = value;
    if (_gs) _gs->setParent(this);
}

////////////////////////////////////////////////////////////////////

GrainSizeDistribution* DustMixPopulation::sizeDistribution() const
{
    return _gs;
}

////////////////////////////////////////////////////////////////////

void DustMixPopulation::setSubPops(int value)
{
    _Nbins = value;
}

////////////////////////////////////////////////////////////////////

int DustMixPopulation::subPops() const
{
    return _Nbins;
}

////////////////////////////////////////////////////////////////////
