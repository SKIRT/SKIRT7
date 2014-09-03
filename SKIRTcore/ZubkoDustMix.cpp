/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "DraineGraphiteGrainComposition.hpp"
#include "DraineIonizedPAHGrainComposition.hpp"
#include "DraineNeutralPAHGrainComposition.hpp"
#include "DraineSilicateGrainComposition.hpp"
#include "ZubkoDustMix.hpp"
#include "ZubkoGraphiteGrainSizeDistribution.hpp"
#include "ZubkoPAHGrainSizeDistribution.hpp"
#include "ZubkoSilicateGrainSizeDistribution.hpp"

using namespace std;

//////////////////////////////////////////////////////////////////////

ZubkoDustMix::ZubkoDustMix()
    : _Ngra(0), _Nsil(0), _Npah(0)
{
}

//////////////////////////////////////////////////////////////////////

void ZubkoDustMix::setupSelfBefore()
{
    MultiGrainDustMix::setupSelfBefore();

    addpopulations(new DraineGraphiteGrainComposition(this), new ZubkoGraphiteGrainSizeDistribution(this), _Ngra);
    addpopulations(new DraineSilicateGrainComposition(this), new ZubkoSilicateGrainSizeDistribution(this), _Nsil);
    addpopulations(new DraineNeutralPAHGrainComposition(this), new ZubkoPAHGrainSizeDistribution(this, 0.5), _Npah);
    addpopulations(new DraineIonizedPAHGrainComposition(this), new ZubkoPAHGrainSizeDistribution(this, 0.5), _Npah);
}

////////////////////////////////////////////////////////////////////

void ZubkoDustMix::setGraphitePops(int value)
{
    _Ngra = value;
}

////////////////////////////////////////////////////////////////////

int ZubkoDustMix::graphitePops() const
{
    return _Ngra;
}

////////////////////////////////////////////////////////////////////

void ZubkoDustMix::setSilicatePops(int value)
{
    _Nsil = value;
}

////////////////////////////////////////////////////////////////////

int ZubkoDustMix::silicatePops() const
{
    return _Nsil;
}

////////////////////////////////////////////////////////////////////

void ZubkoDustMix::setPAHPops(int value)
{
    _Npah = value;
}

////////////////////////////////////////////////////////////////////

int ZubkoDustMix::PAHPops() const
{
    return _Npah;
}

//////////////////////////////////////////////////////////////////////
