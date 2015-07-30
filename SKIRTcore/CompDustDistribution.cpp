/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "CompDustDistribution.hpp"
#include "DustComp.hpp"
#include "FatalError.hpp"
#include "Geometry.hpp"
#include "NR.hpp"
#include "Random.hpp"

using namespace std;

//////////////////////////////////////////////////////////////////////

CompDustDistribution::CompDustDistribution()
{
}

//////////////////////////////////////////////////////////////////////

void CompDustDistribution::insertComponent(int index, DustComp* value)
{
    if (!value) throw FATALERROR("Dust component pointer shouldn't be null");
    value->setParent(this);
    _dcv.insert(index, value);
}

//////////////////////////////////////////////////////////////////////

void CompDustDistribution::removeComponent(int index)
{
    delete _dcv.takeAt(index);
}

//////////////////////////////////////////////////////////////////////

QList<DustComp*> CompDustDistribution::components() const
{
    return _dcv;
}

////////////////////////////////////////////////////////////////////

int CompDustDistribution::dimension() const
{
    int result = 1;
    foreach (DustComp* dc, _dcv) result = qMax(result, dc->dimension());
    return result;
}

////////////////////////////////////////////////////////////////////

void CompDustDistribution::setupSelfBefore()
{
    DustDistribution::setupSelfBefore();

    // verify that there is at least one component
    if (_dcv.isEmpty()) throw FATALERROR("There are no dust components");
}

////////////////////////////////////////////////////////////////////

void CompDustDistribution::setupSelfAfter()
{
    DustDistribution::setupSelfAfter();

    // construct a vector with the normalized cumulative masses
    NR::cdf(_cumrhov, _dcv.size(), [this](int i){return _dcv[i]->mass();} );
}

//////////////////////////////////////////////////////////////////////

int
CompDustDistribution::Ncomp()
const
{
    return _dcv.size();
}

//////////////////////////////////////////////////////////////////////

DustMix*
CompDustDistribution::mix(int h)
const
{
    return _dcv[h]->mix();
}

//////////////////////////////////////////////////////////////////////

double
CompDustDistribution::density(int h, Position bfr)
const
{
    return _dcv[h]->density(bfr);
}

//////////////////////////////////////////////////////////////////////

double
CompDustDistribution::density(Position bfr)
const
{
    double sum = 0.0;
    foreach (DustComp* dc, _dcv) sum += dc->density(bfr);
    return sum;
}

//////////////////////////////////////////////////////////////////////

Position CompDustDistribution::generatePosition() const
{
    Random* random = find<Random>();
    int h = NR::locate_clip(_cumrhov, random->uniform());
    return _dcv[h]->geometry()->generatePosition();
}

//////////////////////////////////////////////////////////////////////

double
CompDustDistribution::mass(int h)
const
{
    return _dcv[h]->mass();
}

//////////////////////////////////////////////////////////////////////

double
CompDustDistribution::mass()
const
{
    double sum = 0.0;
    foreach (DustComp* dc, _dcv) sum += dc->mass();
    return sum;
}

//////////////////////////////////////////////////////////////////////

double
CompDustDistribution::SigmaX()
const
{
    double sum = 0.0;
    foreach (DustComp* dc, _dcv) sum += dc->SigmaX();
    return sum;
}

//////////////////////////////////////////////////////////////////////

double
CompDustDistribution::SigmaY()
const
{
    double sum = 0.0;
    foreach (DustComp* dc, _dcv) sum += dc->SigmaY();
    return sum;
}

//////////////////////////////////////////////////////////////////////

double
CompDustDistribution::SigmaZ()
const
{
    double sum = 0.0;
    foreach (DustComp* dc, _dcv) sum += dc->SigmaZ();
    return sum;
}

//////////////////////////////////////////////////////////////////////

QList<SimulationItem*> CompDustDistribution::interfaceCandidates(const std::type_info& interfaceTypeInfo)
{
    QList<SimulationItem*> candidates = DustDistribution::interfaceCandidates(interfaceTypeInfo);
    if (_dcv.size() == 1) candidates << _dcv[0]->geometry();
    return candidates;
}

//////////////////////////////////////////////////////////////////////
