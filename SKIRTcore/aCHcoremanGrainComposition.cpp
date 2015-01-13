/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include "aCHcoremanGrainComposition.hpp"

//////////////////////////////////////////////////////////////////////

aCHcoremanGrainComposition::aCHcoremanGrainComposition()
{
}

//////////////////////////////////////////////////////////////////////

void aCHcoremanGrainComposition::setupSelfBefore()
{
    GrainComposition::setupSelfBefore();

    setBulkDensity(3.33e3);
    loadLogHeatCapacityGrid("GrainComposition/DustEM/hcap/C_aCH_coreman_d20.DAT");
    loadOpticalGrid(true, "GrainComposition/Min/aCH_coreman_d20_Jones2013_SKIRT.dat", false, false, false, false);
}

//////////////////////////////////////////////////////////////////////

QString aCHcoremanGrainComposition::name() const
{
    return "aCHcoreman";
}

//////////////////////////////////////////////////////////////////////
