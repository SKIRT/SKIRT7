/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include "DraineIonizedPAHGrainComposition.hpp"
#include "DraineGraphiteGrainComposition.hpp"

//////////////////////////////////////////////////////////////////////

DraineIonizedPAHGrainComposition::DraineIonizedPAHGrainComposition()
{
}

//////////////////////////////////////////////////////////////////////

DraineIonizedPAHGrainComposition::DraineIonizedPAHGrainComposition(SimulationItem *parent)
{
    setParent(parent);
    setup();
}

//////////////////////////////////////////////////////////////////////

void DraineIonizedPAHGrainComposition::setupSelfBefore()
{
    loadOpticalGrid(true, "GrainComposition/Draine/PAHion_30.dat", true, false, true, false);
    calculateEnthalpyGrid(DraineGraphiteGrainComposition::enthalpyfunction);
    setBulkDensity(2.24e3);
}

//////////////////////////////////////////////////////////////////////

QString DraineIonizedPAHGrainComposition::name() const
{
    return "Draine_Ionized_PAH";
}

//////////////////////////////////////////////////////////////////////
