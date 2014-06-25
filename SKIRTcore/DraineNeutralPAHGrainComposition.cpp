/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include "DraineNeutralPAHGrainComposition.hpp"
#include "DraineGraphiteGrainComposition.hpp"

//////////////////////////////////////////////////////////////////////

DraineNeutralPAHGrainComposition::DraineNeutralPAHGrainComposition()
{
}

//////////////////////////////////////////////////////////////////////

DraineNeutralPAHGrainComposition::DraineNeutralPAHGrainComposition(SimulationItem *parent)
{
    setParent(parent);
    setup();
}

//////////////////////////////////////////////////////////////////////

void DraineNeutralPAHGrainComposition::setupSelfBefore()
{
    loadOpticalGrid("GrainComposition/Draine/PAHneu_30.dat", true, false, true, false);
    calculateEnthalpyGrid(DraineGraphiteGrainComposition::enthalpyfunction);
    setBulkDensity(2.24e3);
}

//////////////////////////////////////////////////////////////////////

QString DraineNeutralPAHGrainComposition::name() const
{
    return "Draine_Neutral_PAH";
}

//////////////////////////////////////////////////////////////////////
