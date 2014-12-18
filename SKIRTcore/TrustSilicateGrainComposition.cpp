/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "TrustSilicateGrainComposition.hpp"

//////////////////////////////////////////////////////////////////////

TrustSilicateGrainComposition::TrustSilicateGrainComposition()
{
}

//////////////////////////////////////////////////////////////////////

TrustSilicateGrainComposition::TrustSilicateGrainComposition(SimulationItem *parent)
{
    setParent(parent);
    setup();
}

//////////////////////////////////////////////////////////////////////

void TrustSilicateGrainComposition::setupSelfBefore()
{
    GrainComposition::setupSelfBefore();

    loadOpticalGrid(true, "GrainComposition/Trust/suvSil_121_1201.dat", false, true, false, true);
    loadEnthalpyGrid(true, "GrainComposition/Trust/Silicate_Calorimetry_1000.dat");
    setBulkDensity(3.5e3);
}

//////////////////////////////////////////////////////////////////////

QString TrustSilicateGrainComposition::name() const
{
    return "Trust_Silicate";
}

//////////////////////////////////////////////////////////////////////
