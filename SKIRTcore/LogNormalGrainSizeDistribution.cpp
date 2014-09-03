/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include <cmath>
#include "FatalError.hpp"
#include "LogNormalGrainSizeDistribution.hpp"

////////////////////////////////////////////////////////////////////

LogNormalGrainSizeDistribution::LogNormalGrainSizeDistribution()
    : _a0(0), _sigma(0)
{
}

////////////////////////////////////////////////////////////////////

void LogNormalGrainSizeDistribution::setupSelfBefore()
{
    RangeGrainSizeDistribution::setupSelfBefore();

    // verify the attributes
    if (_a0 <= 0) throw FATALERROR("The centroid of the log-normal law must be positive");
    if (_sigma <= 0) throw FATALERROR("The width of the log-normal law must be positive");
}

////////////////////////////////////////////////////////////////////

void LogNormalGrainSizeDistribution::setCentroid(double value)
{
    _a0 = value;
}

////////////////////////////////////////////////////////////////////

double LogNormalGrainSizeDistribution::centroid() const
{
    return _a0;
}

////////////////////////////////////////////////////////////////////

void LogNormalGrainSizeDistribution::setWidth(double value)
{
    _sigma = value;
}

////////////////////////////////////////////////////////////////////

double LogNormalGrainSizeDistribution::width() const
{
    return _sigma;
}

////////////////////////////////////////////////////////////////////

double LogNormalGrainSizeDistribution::dnda(double a) const
{
    double x = log(a/_a0) / _sigma;
    return _C / a * exp( -0.5*x*x );
}

////////////////////////////////////////////////////////////////////
