/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "ForsteriteGrainComposition.hpp"
#include "FatalError.hpp"

//////////////////////////////////////////////////////////////////////

ForsteriteGrainComposition::ForsteriteGrainComposition()
    : _type(Crystalline)
{
}

//////////////////////////////////////////////////////////////////////

ForsteriteGrainComposition::ForsteriteGrainComposition(SimulationItem *parent, GrainType type)
{
    setParent(parent);
    setType(type);
    setup();
}

//////////////////////////////////////////////////////////////////////

void ForsteriteGrainComposition::setupSelfBefore()
{
    GrainComposition::setupSelfBefore();

    // set the bulk density and load the appropriate resources based on the grain type
    switch (_type)
    {
    case Crystalline:
        setBulkDensity(3330.);
        loadLogHeatCapacityGrid("GrainComposition/Min/C_aSil.DAT");
        loadOpticalGrid(true, "GrainComposition/Min/Forsterite_Suto2006.dat", false, false, false, false);
        break;
    case Amorphous:
        setBulkDensity(2190.);
        loadLogHeatCapacityGrid("GrainComposition/ThemisJ17/C_aOlM5.DAT");
        loadOpticalGrid("GrainComposition/ThemisJ17/LAMBDA.DAT",
                        "GrainComposition/ThemisJ17/Q_aOlM5.DAT",
                        "GrainComposition/ThemisJ17/G_aOlM5.DAT");
        break;
    }
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
