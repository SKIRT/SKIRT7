/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "DustEmGrainComposition.hpp"

//////////////////////////////////////////////////////////////////////

DustEmGrainComposition::DustEmGrainComposition()
    : _bulkmassdensity(0)

{
}

//////////////////////////////////////////////////////////////////////

DustEmGrainComposition::DustEmGrainComposition(SimulationItem *parent, QString graintype, double rhobulk)
    : _graintype(graintype), _bulkmassdensity(rhobulk)
{
    setParent(parent);
    setup();
}

//////////////////////////////////////////////////////////////////////

void DustEmGrainComposition::setupSelfBefore()
{
    GrainComposition::setupSelfBefore();

    setBulkDensity(_bulkmassdensity);
    loadLogHeatCapacityGrid("GrainComposition/DustEM/hcap/C_" + _graintype + ".DAT");
    loadOpticalGrid("GrainComposition/DustEM/oprop/LAMBDA.DAT",
                    "GrainComposition/DustEM/oprop/Q_" + _graintype + ".DAT",
                    "GrainComposition/DustEM/oprop/G_" + _graintype + ".DAT");
}

//////////////////////////////////////////////////////////////////////

QString DustEmGrainComposition::name() const
{
    return "DustEM_" + _graintype;
}

//////////////////////////////////////////////////////////////////////

void DustEmGrainComposition::setGrainType(QString value)
{
    _graintype = value;
}

//////////////////////////////////////////////////////////////////////

QString DustEmGrainComposition::grainType() const
{
    return _graintype;
}

//////////////////////////////////////////////////////////////////////

void DustEmGrainComposition::setBulkMassDensity(double value)
{
    _bulkmassdensity = value;
}

//////////////////////////////////////////////////////////////////////

double DustEmGrainComposition::bulkMassDensity() const
{
    return _bulkmassdensity;
}

//////////////////////////////////////////////////////////////////////
