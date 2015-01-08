/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include <cmath>
#include "FatalError.hpp"
#include "PowerLawExpTailGrainSizeDistribution.hpp"

////////////////////////////////////////////////////////////////////

PowerLawExpTailGrainSizeDistribution::PowerLawExpTailGrainSizeDistribution()
    : _alpha(0), _at(0), _ac(0), _gamma(0)
{
}

////////////////////////////////////////////////////////////////////

void PowerLawExpTailGrainSizeDistribution::setupSelfBefore()
{
    RangeGrainSizeDistribution::setupSelfBefore();

    // Verify the attributes
    if (_alpha >= 0) throw FATALERROR("The index of the power-law must be negative");
    if (_ac <= 0) throw FATALERROR("The scale in the exponential decay term must be positive");
}

////////////////////////////////////////////////////////////////////

void PowerLawExpTailGrainSizeDistribution::setAlpha(double value)
{
    _alpha = value;
}

////////////////////////////////////////////////////////////////////

double PowerLawExpTailGrainSizeDistribution::alpha() const
{
    return _alpha;
}

////////////////////////////////////////////////////////////////////

void PowerLawExpTailGrainSizeDistribution::setAt(double value)
{
    _at = value;
}

////////////////////////////////////////////////////////////////////

double PowerLawExpTailGrainSizeDistribution::at() const
{
    return _at;
}

////////////////////////////////////////////////////////////////////

void PowerLawExpTailGrainSizeDistribution::setAc(double value)
{
    _ac = value;
}

////////////////////////////////////////////////////////////////////

double PowerLawExpTailGrainSizeDistribution::ac() const
{
    return _ac;
}

////////////////////////////////////////////////////////////////////

void PowerLawExpTailGrainSizeDistribution::setGamma(double value)
{
    _gamma = value;
}

////////////////////////////////////////////////////////////////////

double PowerLawExpTailGrainSizeDistribution::gamma() const
{
    return _gamma;
}

////////////////////////////////////////////////////////////////////

double PowerLawExpTailGrainSizeDistribution::dnda(double a) const
{
    return _C
            * pow(a,_alpha)
            * ( a<=_at ? 1. : exp(-pow((a-_at)/_ac,_gamma)) );  // exponential decay
}

////////////////////////////////////////////////////////////////////
