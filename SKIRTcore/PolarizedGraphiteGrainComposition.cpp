/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "PolarizedGraphiteGrainComposition.hpp"
#include "DraineGraphiteGrainComposition.hpp"

//////////////////////////////////////////////////////////////////////

PolarizedGraphiteGrainComposition::PolarizedGraphiteGrainComposition()
{
}

//////////////////////////////////////////////////////////////////////

void PolarizedGraphiteGrainComposition::setupSelfBefore()
{
    GrainComposition::setupSelfBefore();

    setBulkDensity(2.24e3);
    calculateEnthalpyGrid(DraineGraphiteGrainComposition::enthalpyfunction);
    loadPolarizedOpticalGrid(true, "GrainComposition/Polarized/Graphite_STOKES_Sxx_001.DAT");
}

//////////////////////////////////////////////////////////////////////

QString PolarizedGraphiteGrainComposition::name() const
{
    return "Polarized_Draine_Graphite";
}

//////////////////////////////////////////////////////////////////////
