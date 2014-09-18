/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "AxGeometry.hpp"
#include "Geometry.hpp"
#include "DustMix.hpp"
#include "FatalError.hpp"
#include "EdgeOnDustCompNormalization.hpp"

////////////////////////////////////////////////////////////////////

EdgeOnDustCompNormalization::EdgeOnDustCompNormalization()
    : _wavelength(0), _tau(0)
{
}

//////////////////////////////////////////////////////////////////////

void
EdgeOnDustCompNormalization::setupSelfBefore()
{
    DustCompNormalization::setupSelfBefore();

    // verify that we have some wavelength and optical depth
    if (_wavelength <= 0) throw FATALERROR("Wavelength was not set");
    if (_tau <= 0) throw FATALERROR("Optical depth was not set");
}

////////////////////////////////////////////////////////////////////

void
EdgeOnDustCompNormalization::setWavelength(double value)
{
    _wavelength = value;
}

////////////////////////////////////////////////////////////////////

double
EdgeOnDustCompNormalization::wavelength()
const
{
    return _wavelength;
}

////////////////////////////////////////////////////////////////////

void
EdgeOnDustCompNormalization::setOpticalDepth(double value)
{
    _tau = value;
}

////////////////////////////////////////////////////////////////////

double
EdgeOnDustCompNormalization::opticalDepth()
const
{
    return _tau;
}

////////////////////////////////////////////////////////////////////

double
EdgeOnDustCompNormalization::normalizationFactor(const Geometry* geom, const DustMix* mix)
const
{
    const AxGeometry* axgeom = dynamic_cast<const AxGeometry*>(geom);
    if (!axgeom) throw FATALERROR("Geometry is not axisymmetric");
    return _tau / ( axgeom->SigmaR() * mix->kappaext(_wavelength) );
}

//////////////////////////////////////////////////////////////////////
