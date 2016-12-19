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

    // determine the bulk density and resource filenames based on the grain type
    double density;
    QString heatfile;
    QString opticalfile;
    switch (_type)
    {
    case Crystalline:
        density = 3330.;
        heatfile = "GrainComposition/Min/C_aSil.DAT";
        opticalfile = "GrainComposition/Min/Forsterite_Suto2006.dat";
        break;
    case Amorphous:
        density = 2190.;
        heatfile = "GrainComposition/Themis/C_CM_amFo10Fe30FeS.DAT";
        opticalfile = "GrainComposition/Themis/CM_amFo10Fe30FeS_Jones2013_SKIRT.dat";
        break;
    }
    setBulkDensity(density);
    loadLogHeatCapacityGrid(heatfile);
    loadOpticalGrid(true, opticalfile, false, false, false, false);
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
