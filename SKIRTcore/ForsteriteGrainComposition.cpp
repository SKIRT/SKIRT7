/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "ForsteriteGrainComposition.hpp"

//////////////////////////////////////////////////////////////////////

ForsteriteGrainComposition::ForsteriteGrainComposition()
    : _type(Crystalline)
{
}

//////////////////////////////////////////////////////////////////////

void ForsteriteGrainComposition::setupSelfBefore()
{
    GrainComposition::setupSelfBefore();

    // determine the bulk density and resource filenames based on the grain type
    double density; QString heatfile; QString opticalfile;
    switch (_type)
    {
    case Crystalline:
        density = 3.33e3;
        heatfile = "C_aSil";
        opticalfile = "Forsterite_Suto2006";
        break;
    case Amorphous:
        density = 3.33e3;
        heatfile = "C_CM_amFo10Fe30FeS";
        opticalfile = "CM_amFo10Fe30FeS_Jones2013_SKIRT";
        break;
    }

    setBulkDensity(density);
    loadLogHeatCapacityGrid("GrainComposition/DustEM/hcap/" + heatfile + ".DAT");
    loadOpticalGrid(true, "GrainComposition/Min/" + opticalfile + ".dat", false, false, false, false);
}

//////////////////////////////////////////////////////////////////////

void ForsteriteGrainComposition::setType(ForsteriteGrainComposition::GrainType value)
{
    _type = value;
}

//////////////////////////////////////////////////////////////////////

ForsteriteGrainComposition::GrainType ForsteriteGrainComposition::type() const
{
    return _type;
}

//////////////////////////////////////////////////////////////////////

QString ForsteriteGrainComposition::name() const
{
    switch (_type)
    {
    case Crystalline:
        return "Crystalline_Forsterite";
    case Amorphous:
        return "Amorphous_Forsterite";
    }
}

//////////////////////////////////////////////////////////////////////
