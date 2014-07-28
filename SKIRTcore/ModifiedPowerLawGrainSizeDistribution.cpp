/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include <cmath>
#include "FatalError.hpp"
#include "ModifiedPowerLawGrainSizeDistribution.hpp"

////////////////////////////////////////////////////////////////////

ModifiedPowerLawGrainSizeDistribution::ModifiedPowerLawGrainSizeDistribution()
    : _alpha(0), _at(0), _ac(0), _gamma(0), _au(0), _zeta(0), _eta(0)
{
}

////////////////////////////////////////////////////////////////////

void ModifiedPowerLawGrainSizeDistribution::setupSelfBefore()
{
    RangeGrainSizeDistribution::setupSelfBefore();

    // verify the attributes
    if (_alpha >= 0) throw FATALERROR("The index of the power-law must be negative");
    if (_ac <= 0) throw FATALERROR("The scale in the exponential decay term must be positive");
    if (_au <= 0) throw FATALERROR("The scale in the curvature term must be positive");
}

////////////////////////////////////////////////////////////////////

void ModifiedPowerLawGrainSizeDistribution::setAlpha(double value)
{
    _alpha = value;
}

////////////////////////////////////////////////////////////////////

double ModifiedPowerLawGrainSizeDistribution::alpha() const
{
    return _alpha;
}

////////////////////////////////////////////////////////////////////

void ModifiedPowerLawGrainSizeDistribution::setAt(double value)
{
    _at = value;
}

////////////////////////////////////////////////////////////////////

double ModifiedPowerLawGrainSizeDistribution::at() const
{
    return _at;
}

////////////////////////////////////////////////////////////////////

void ModifiedPowerLawGrainSizeDistribution::setAc(double value)
{
    _ac = value;
}

////////////////////////////////////////////////////////////////////

double ModifiedPowerLawGrainSizeDistribution::ac() const
{
    return _ac;
}

////////////////////////////////////////////////////////////////////

void ModifiedPowerLawGrainSizeDistribution::setGamma(double value)
{
    _gamma = value;
}

////////////////////////////////////////////////////////////////////

double ModifiedPowerLawGrainSizeDistribution::gamma() const
{
    return _gamma;
}

////////////////////////////////////////////////////////////////////

void ModifiedPowerLawGrainSizeDistribution::setAu(double value)
{
    _au = value;
}

////////////////////////////////////////////////////////////////////

double ModifiedPowerLawGrainSizeDistribution::au() const
{
    return _au;
}

////////////////////////////////////////////////////////////////////

void ModifiedPowerLawGrainSizeDistribution::setZeta(double value)
{
    _zeta = value;
}

////////////////////////////////////////////////////////////////////

double ModifiedPowerLawGrainSizeDistribution::zeta() const
{
    return _zeta;
}

////////////////////////////////////////////////////////////////////

void ModifiedPowerLawGrainSizeDistribution::setEta(double value)
{
    _eta = value;
}

////////////////////////////////////////////////////////////////////

double ModifiedPowerLawGrainSizeDistribution::eta() const
{
    return _eta;
}

////////////////////////////////////////////////////////////////////

double ModifiedPowerLawGrainSizeDistribution::dnda(double a) const
{
    return _C
            * pow(a,_alpha)
            * pow(1. + fabs(_zeta)*pow(a/_au,_eta), _zeta>=0 ? 1 : -1)  // curvature
            * ( a<=_at ? 1. : exp(-pow((a-_at)/_ac,_gamma)) );          // exponential decay
}

////////////////////////////////////////////////////////////////////
