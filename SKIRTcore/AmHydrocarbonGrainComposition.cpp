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

AmHydrocarbonGrainComposition::AmHydrocarbonGrainComposition(SimulationItem *parent, double bulkDensity)
{
    setParent(parent);
    setup();
    setBulkDensity(bulkDensity);
}

//////////////////////////////////////////////////////////////////////

void AmHydrocarbonGrainComposition::setupSelfBefore()
{
    GrainComposition::setupSelfBefore();

    setBulkDensity(1600.);
    loadLogHeatCapacityGrid("GrainComposition/ThemisJ17/C_CM20.DAT");
    loadOpticalGrid("GrainComposition/ThemisJ17/LAMBDA.DAT",
                    "GrainComposition/ThemisJ17/Q_CM20.DAT",
                    "GrainComposition/ThemisJ17/G_CM20.DAT");
}

//////////////////////////////////////////////////////////////////////

QString AmHydrocarbonGrainComposition::name() const
{
    return "Amorphous_Hydrocarbon";
}

//////////////////////////////////////////////////////////////////////
