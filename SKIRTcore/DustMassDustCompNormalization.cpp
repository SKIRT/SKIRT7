/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include "Geometry.hpp"
#include "DustMassDustCompNormalization.hpp"
#include "FatalError.hpp"

////////////////////////////////////////////////////////////////////

DustMassDustCompNormalization::DustMassDustCompNormalization()
    : _Mdust(0)
{
}

//////////////////////////////////////////////////////////////////////

void DustMassDustCompNormalization::setupSelfBefore()
{
    DustCompNormalization::setupSelfBefore();

    // verify that we have some dust mass
    if (_Mdust <= 0) throw FATALERROR("Dust mass was not set");
}

////////////////////////////////////////////////////////////////////

void
DustMassDustCompNormalization::setDustMass(double value)
{
    _Mdust = value;
}

////////////////////////////////////////////////////////////////////

double
DustMassDustCompNormalization::dustMass()
const
{
    return _Mdust;
}

////////////////////////////////////////////////////////////////////

double
DustMassDustCompNormalization::normalizationFactor(const Geometry* /*geom*/, const DustMix* /*mix*/)
const
{
    return _Mdust; // / geom->totalmass();  new geometry class always has mass = 1...
}

//////////////////////////////////////////////////////////////////////
