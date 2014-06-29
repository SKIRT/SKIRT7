/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include "DraineSilicateGrainComposition.hpp"
#include "MieSilicateGrainComposition.hpp"

//////////////////////////////////////////////////////////////////////

MieSilicateGrainComposition::MieSilicateGrainComposition()
{
}

//////////////////////////////////////////////////////////////////////

MieSilicateGrainComposition::MieSilicateGrainComposition(SimulationItem *parent)
{
    setParent(parent);
    setup();
}

//////////////////////////////////////////////////////////////////////

void MieSilicateGrainComposition::setupSelfBefore()
{
    loadOpticalGrid(true, "GrainComposition/Other/MieAmorphousSilicate.dat", false, false, false, false);
    calculateEnthalpyGrid(DraineSilicateGrainComposition::enthalpyfunction);
    setBulkDensity(3.0e3);
}

//////////////////////////////////////////////////////////////////////

QString MieSilicateGrainComposition::name() const
{
    return "Mie_Silicate";
}

//////////////////////////////////////////////////////////////////////
