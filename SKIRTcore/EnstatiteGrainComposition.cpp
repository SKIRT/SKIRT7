/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "EnstatiteGrainComposition.hpp"

//////////////////////////////////////////////////////////////////////

EnstatiteGrainComposition::EnstatiteGrainComposition()
    : _type(Crystalline)
{
}

//////////////////////////////////////////////////////////////////////

void EnstatiteGrainComposition::setupSelfBefore()
{
    GrainComposition::setupSelfBefore();

    // determine the bulk density and resource filenames based on the grain type
    double density; QString heatfile; QString opticalfile;
    switch (_type)
    {
    case Crystalline:
        density = 2.80e3;
        heatfile = "C_aSil";
        opticalfile = "Enstatite_Jaeger1998";
        break;
    case Amorphous:
        density = 3.33e3;
        heatfile = "C_CM_amEnst10Fe30FeS";
        opticalfile = "CM_amEnst10Fe30FeS_Jones2013_SKIRT";
        break;
    }

    setBulkDensity(density);
    loadLogHeatCapacityGrid("GrainComposition/DustEM/hcap/" + heatfile + ".DAT");
    loadOpticalGrid(true, "GrainComposition/Min/" + opticalfile + ".dat", false, false, false, false);
}

//////////////////////////////////////////////////////////////////////

QString EnstatiteGrainComposition::name() const
{
    return "Enstatite";
}

//////////////////////////////////////////////////////////////////////

void EnstatiteGrainComposition::setType(EnstatiteGrainComposition::GrainType value)
{
    _type = value;
}

//////////////////////////////////////////////////////////////////////

EnstatiteGrainComposition::GrainType EnstatiteGrainComposition::type() const
{
    return _type;
}

//////////////////////////////////////////////////////////////////////
