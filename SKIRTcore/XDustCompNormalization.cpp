/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "Geometry.hpp"
#include "DustMix.hpp"
#include "FatalError.hpp"
#include "XDustCompNormalization.hpp"

////////////////////////////////////////////////////////////////////

XDustCompNormalization::XDustCompNormalization()
    : _wavelength(0), _tau(0)
{
}

//////////////////////////////////////////////////////////////////////

void
XDustCompNormalization::setupSelfBefore()
{
    DustCompNormalization::setupSelfBefore();

    // verify that we have some wavelength and optical depth
    if (_wavelength <= 0) throw FATALERROR("Wavelength was not set");
    if (_tau <= 0) throw FATALERROR("Optical depth was not set");
}

////////////////////////////////////////////////////////////////////

void
XDustCompNormalization::setWavelength(double value)
{
    _wavelength = value;
}

////////////////////////////////////////////////////////////////////

double
XDustCompNormalization::wavelength()
const
{
    return _wavelength;
}

////////////////////////////////////////////////////////////////////

void
XDustCompNormalization::setOpticalDepth(double value)
{
    _tau = value;
}

////////////////////////////////////////////////////////////////////

double
XDustCompNormalization::opticalDepth()
const
{
    return _tau;
}

////////////////////////////////////////////////////////////////////

double
XDustCompNormalization::normalizationFactor(const Geometry* geom, const DustMix* mix)
const
{
    return _tau / ( geom->SigmaX() * mix->kappaext(_wavelength) );
}

//////////////////////////////////////////////////////////////////////
