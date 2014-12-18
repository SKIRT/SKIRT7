/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "TrustNeutralPAHGrainComposition.hpp"

//////////////////////////////////////////////////////////////////////

TrustNeutralPAHGrainComposition::TrustNeutralPAHGrainComposition()
{
}

//////////////////////////////////////////////////////////////////////

TrustNeutralPAHGrainComposition::TrustNeutralPAHGrainComposition(SimulationItem *parent)
{
    setParent(parent);
    setup();
}

//////////////////////////////////////////////////////////////////////

void TrustNeutralPAHGrainComposition::setupSelfBefore()
{
    GrainComposition::setupSelfBefore();

    loadOpticalGrid(true, "GrainComposition/Trust/PAH_28_1201_neu.dat", false, true, false, true);
    loadEnthalpyGrid(true, "GrainComposition/Trust/Graphitic_Calorimetry_1000.dat");
    setBulkDensity(2.24e3);
}

//////////////////////////////////////////////////////////////////////

QString TrustNeutralPAHGrainComposition::name() const
{
    return "Trust_Neutral_PAH";
}

//////////////////////////////////////////////////////////////////////
