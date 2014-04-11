/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include "AxGeometry.hpp"
#include "Geometry.hpp"
#include "DustMix.hpp"
#include "FatalError.hpp"
#include "FaceOnDustCompNormalization.hpp"

////////////////////////////////////////////////////////////////////

FaceOnDustCompNormalization::FaceOnDustCompNormalization()
    : _wavelength(0), _tau(0)
{
}

//////////////////////////////////////////////////////////////////////

void
FaceOnDustCompNormalization::setupSelfBefore()
{
    DustCompNormalization::setupSelfBefore();

    // verify that we have some wavelength and optical depth
    if (_wavelength <= 0) throw FATALERROR("Wavelength was not set");
    if (_tau <= 0) throw FATALERROR("Optical depth was not set");
}

////////////////////////////////////////////////////////////////////

void
FaceOnDustCompNormalization::setWavelength(double value)
{
    _wavelength = value;
}

////////////////////////////////////////////////////////////////////

double
FaceOnDustCompNormalization::wavelength()
const
{
    return _wavelength;
}

////////////////////////////////////////////////////////////////////

void
FaceOnDustCompNormalization::setOpticalDepth(double value)
{
    _tau = value;
}

////////////////////////////////////////////////////////////////////

double
FaceOnDustCompNormalization::opticalDepth()
const
{
    return _tau;
}

////////////////////////////////////////////////////////////////////

double
FaceOnDustCompNormalization::normalizationFactor(const Geometry* geom, const DustMix* mix)
const
{
    const AxGeometry* axgeom = dynamic_cast<const AxGeometry*>(geom);
    if (!axgeom) throw FATALERROR("Geometry is not axisymmetric");
    return _tau / ( axgeom->SigmaZ() * mix->kappaext(_wavelength) );
}

//////////////////////////////////////////////////////////////////////
