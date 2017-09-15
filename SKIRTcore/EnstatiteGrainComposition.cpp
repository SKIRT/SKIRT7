/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "EnstatiteGrainComposition.hpp"
#include "FatalError.hpp"

//////////////////////////////////////////////////////////////////////

EnstatiteGrainComposition::EnstatiteGrainComposition()
    : _type(Crystalline)
{
}

//////////////////////////////////////////////////////////////////////

EnstatiteGrainComposition::EnstatiteGrainComposition(SimulationItem *parent, GrainType type)
{
    setParent(parent);
    setType(type);
    setup();
}

//////////////////////////////////////////////////////////////////////

void EnstatiteGrainComposition::setupSelfBefore()
{
    GrainComposition::setupSelfBefore();

    // set the bulk density and load the appropriate resources based on the grain type
    switch (_type)
    {
    case Crystalline:
        setBulkDensity(2800.);
        loadLogHeatCapacityGrid("GrainComposition/Min/C_aSil.DAT");
        loadOpticalGrid(true, "GrainComposition/Min/Enstatite_Jaeger1998.dat", false, false, false, false);
        break;
    case Amorphous:
        setBulkDensity(2190.);
        loadLogHeatCapacityGrid("GrainComposition/ThemisJ17/C_aPyM5.DAT");
        loadOpticalGrid("GrainComposition/ThemisJ17/LAMBDA.DAT",
                        "GrainComposition/ThemisJ17/Q_aPyM5.DAT",
                        "GrainComposition/ThemisJ17/G_aPyM5.DAT");
        break;
    }
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

QString EnstatiteGrainComposition::name() const
{
    switch (_type)
    {
    case Crystalline:
        return "Crystalline_Enstatite";
    case Amorphous:
        return "Amorphous_Enstatite";
    }
}

//////////////////////////////////////////////////////////////////////
