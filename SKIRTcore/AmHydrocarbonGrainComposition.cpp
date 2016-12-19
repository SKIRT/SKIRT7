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

AmHydrocarbonGrainComposition::AmHydrocarbonGrainComposition(SimulationItem *parent)
{
    setParent(parent);
    setup();
}

//////////////////////////////////////////////////////////////////////

void AmHydrocarbonGrainComposition::setupSelfBefore()
{
    GrainComposition::setupSelfBefore();

    setBulkDensity(1600.);
    loadLogHeatCapacityGrid("GrainComposition/Themis/C_aCH_coreman_d20.DAT");
    loadOpticalGrid(true, "GrainComposition/Themis/aCH_coreman_d20_Jones2013_SKIRT.dat", false, false, false, false);
}

//////////////////////////////////////////////////////////////////////

QString AmHydrocarbonGrainComposition::name() const
{
    return "Amorphous_Hydrocarbon";
}

//////////////////////////////////////////////////////////////////////
