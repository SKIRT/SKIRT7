/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "Geometry.hpp"
#include "DustMix.hpp"
#include "FatalError.hpp"
#include "RadialDustCompNormalization.hpp"
#include "SpheGeometry.hpp"

////////////////////////////////////////////////////////////////////

RadialDustCompNormalization::RadialDustCompNormalization()
    : _wavelength(0), _tau(0)
{
}

//////////////////////////////////////////////////////////////////////

void
RadialDustCompNormalization::setupSelfBefore()
{
    DustCompNormalization::setupSelfBefore();

    // verify that we have some wavelength and optical depth
    if (_wavelength <= 0) throw FATALERROR("Wavelength was not set");
    if (_tau <= 0) throw FATALERROR("Optical depth was not set");
}

////////////////////////////////////////////////////////////////////

void
RadialDustCompNormalization::setWavelength(double value)
{
    _wavelength = value;
}

////////////////////////////////////////////////////////////////////

double
RadialDustCompNormalization::wavelength()
const
{
    return _wavelength;
}

////////////////////////////////////////////////////////////////////

void
RadialDustCompNormalization::setOpticalDepth(double value)
{
    _tau = value;
}

////////////////////////////////////////////////////////////////////

double
RadialDustCompNormalization::opticalDepth()
const
{
    return _tau;
}

////////////////////////////////////////////////////////////////////

double
RadialDustCompNormalization::normalizationFactor(const Geometry* geom, const DustMix* mix)
const
{
    const SpheGeometry* sphegeom = dynamic_cast<const SpheGeometry*>(geom);
    if (!sphegeom) throw FATALERROR("Geometry is not spherically symmetric");
    return _tau / ( sphegeom->Sigmar() * mix->kappaext(_wavelength) );
}

//////////////////////////////////////////////////////////////////////
