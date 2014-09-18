/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "TrustGraphiteGrainComposition.hpp"
#include "TrustNeutralPAHGrainComposition.hpp"
#include "TrustSilicateGrainComposition.hpp"
#include "TrustDustMix.hpp"
#include "ZubkoGraphiteGrainSizeDistribution.hpp"
#include "ZubkoPAHGrainSizeDistribution.hpp"
#include "ZubkoSilicateGrainSizeDistribution.hpp"

using namespace std;

//////////////////////////////////////////////////////////////////////

TrustDustMix::TrustDustMix()
    : _Ngra(0), _Nsil(0), _Npah(0)
{
}

//////////////////////////////////////////////////////////////////////

void TrustDustMix::setupSelfBefore()
{
    MultiGrainDustMix::setupSelfBefore();

    addpopulations(new TrustGraphiteGrainComposition(this), new ZubkoGraphiteGrainSizeDistribution(this), _Ngra);
    addpopulations(new TrustSilicateGrainComposition(this), new ZubkoSilicateGrainSizeDistribution(this), _Nsil);
    addpopulations(new TrustNeutralPAHGrainComposition(this), new ZubkoPAHGrainSizeDistribution(this), _Npah);
}

////////////////////////////////////////////////////////////////////

void TrustDustMix::setGraphitePops(int value)
{
    _Ngra = value;
}

////////////////////////////////////////////////////////////////////

int TrustDustMix::graphitePops() const
{
    return _Ngra;
}

////////////////////////////////////////////////////////////////////

void TrustDustMix::setSilicatePops(int value)
{
    _Nsil = value;
}

////////////////////////////////////////////////////////////////////

int TrustDustMix::silicatePops() const
{
    return _Nsil;
}

////////////////////////////////////////////////////////////////////

void TrustDustMix::setPAHPops(int value)
{
    _Npah = value;
}

////////////////////////////////////////////////////////////////////

int TrustDustMix::PAHPops() const
{
    return _Npah;
}

//////////////////////////////////////////////////////////////////////
