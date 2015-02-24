/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include "AmHydrocarbonGrainComposition.hpp"

//////////////////////////////////////////////////////////////////////

AmHydrocarbonGrainComposition::AmHydrocarbonGrainComposition()
{
}

//////////////////////////////////////////////////////////////////////

void AmHydrocarbonGrainComposition::setupSelfBefore()
{
    GrainComposition::setupSelfBefore();

    setBulkDensity(3.33e3);
    loadLogHeatCapacityGrid("GrainComposition/DustEM/hcap/C_aCH_coreman_d20.DAT");
    loadOpticalGrid(true, "GrainComposition/Min/aCH_coreman_d20_Jones2013_SKIRT.dat", false, false, false, false);
}

//////////////////////////////////////////////////////////////////////

QString AmHydrocarbonGrainComposition::name() const
{
    return "Amorphous_Hydrocarbon";
}

//////////////////////////////////////////////////////////////////////
