/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include "ConfigurableDustMix.hpp"
#include "DustMixPopulation.hpp"
#include "FatalError.hpp"
#include "GrainComposition.hpp"
#include "GrainSizeDistribution.hpp"

//////////////////////////////////////////////////////////////////////

ConfigurableDustMix::ConfigurableDustMix()
{
}

//////////////////////////////////////////////////////////////////////

void ConfigurableDustMix::setupSelfBefore()
{
    MultiGrainDustMix::setupSelfBefore();

    // verify that there is at least one dust population
    if (_populations.size() < 1) throw FATALERROR("There should be at least one dust population");

    // add the dust populations to the dust mix
    foreach (DustMixPopulation* population, _populations)
    {
        population->setup();    // since we're in setupSelfBefore, our children aren't yet setup
        addpopulations(population->composition(), population->sizeDistribution(), population->subPops());
    }
}

////////////////////////////////////////////////////////////////////

void ConfigurableDustMix::addPopulation(DustMixPopulation* value)
{
    if (!value) throw FATALERROR("DustMixPopulation pointer shouldn't be null");
    value->setParent(this);
    _populations << value;
}

////////////////////////////////////////////////////////////////////

QList<DustMixPopulation*> ConfigurableDustMix::populations() const
{
    return _populations;
}

//////////////////////////////////////////////////////////////////////
