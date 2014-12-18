/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "PolarizedSilicateGrainComposition.hpp"
#include "DraineSilicateGrainComposition.hpp"

//////////////////////////////////////////////////////////////////////

PolarizedSilicateGrainComposition::PolarizedSilicateGrainComposition()
{
}

//////////////////////////////////////////////////////////////////////

void PolarizedSilicateGrainComposition::setupSelfBefore()
{
    GrainComposition::setupSelfBefore();

    setBulkDensity(3.0e3);
    calculateEnthalpyGrid(DraineSilicateGrainComposition::enthalpyfunction);
    loadPolarizedOpticalGrid(true, "GrainComposition/Polarized/Silicate_STOKES_Sxx.DAT");
}

//////////////////////////////////////////////////////////////////////

QString PolarizedSilicateGrainComposition::name() const
{
    return "Polarized_Draine_Silicate";
}

//////////////////////////////////////////////////////////////////////
