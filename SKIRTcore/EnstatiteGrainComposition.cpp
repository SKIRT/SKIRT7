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

    // determine the bulk density and resource filenames based on the grain type
    double density;
    QString heatfile;
    QString opticalfile;
    switch (_type)
    {
    case Crystalline:
        density = 2800.;
        heatfile = "GrainComposition/Min/C_aSil.DAT";
        opticalfile = "GrainComposition/Min/Enstatite_Jaeger1998.dat";
        break;
    case Amorphous:
        density = 2190.;
        heatfile = "GrainComposition/Themis/C_CM_amEnst10Fe30FeS.DAT";
        opticalfile = "GrainComposition/Themis/CM_amEnst10Fe30FeS_Jones2013_SKIRT.dat";
        break;
    }

    setBulkDensity(density);
    loadLogHeatCapacityGrid(heatfile);
    loadOpticalGrid(true, opticalfile, false, false, false, false);
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
