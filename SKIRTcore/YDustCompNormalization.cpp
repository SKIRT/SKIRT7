/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "Geometry.hpp"
#include "DustMix.hpp"
#include "FatalError.hpp"
#include "YDustCompNormalization.hpp"

////////////////////////////////////////////////////////////////////

YDustCompNormalization::YDustCompNormalization()
    : _wavelength(0), _tau(0)
{
}

//////////////////////////////////////////////////////////////////////

void
YDustCompNormalization::setupSelfBefore()
{
    DustCompNormalization::setupSelfBefore();

    // verify that we have some wavelength and optical depth
    if (_wavelength <= 0) throw FATALERROR("Wavelength was not set");
    if (_tau <= 0) throw FATALERROR("Optical depth was not set");
}

////////////////////////////////////////////////////////////////////

void
YDustCompNormalization::setWavelength(double value)
{
    _wavelength = value;
}

////////////////////////////////////////////////////////////////////

double
YDustCompNormalization::wavelength()
const
{
    return _wavelength;
}

////////////////////////////////////////////////////////////////////

void
YDustCompNormalization::setOpticalDepth(double value)
{
    _tau = value;
}

////////////////////////////////////////////////////////////////////

double
YDustCompNormalization::opticalDepth()
const
{
    return _tau;
}

////////////////////////////////////////////////////////////////////

double
YDustCompNormalization::normalizationFactor(const Geometry* geom, const DustMix* mix)
const
{
    return _tau / ( geom->SigmaY() * mix->kappaext(_wavelength) );
}

//////////////////////////////////////////////////////////////////////
