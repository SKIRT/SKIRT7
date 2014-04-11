/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef POWERLAWGRAINSIZEDISTRIBUTION_HPP
#define POWERLAWGRAINSIZEDISTRIBUTION_HPP

#include "GrainSizeDistribution.hpp"

////////////////////////////////////////////////////////////////////

/** PowerLawGrainSizeDistribution is a GrainSizeDistribution subclass that represents a dust grain
    size distribution of the form \f[ \Omega(a)=(\frac{\text{d}n_\text{D}}{\text{d}a})/n_\text{H}
    = C \, a^{-\gamma} \qquad \text{for}\quad a_\text{min} \leq a \leq a_\text{max}, \f] where the
    the exponent \f$\gamma>0\f$ can be configured as an attribute. The size range and the
    proportionality factor \f$C\f$ can be configured in the GrainSizeDistribution base class. */
class PowerLawGrainSizeDistribution: public GrainSizeDistribution
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a power-law dust grain size distribution")

    Q_CLASSINFO("Property", "exponent")
    Q_CLASSINFO("Title", "the (absolute value of the) exponent in the power-law distribution function")
    Q_CLASSINFO("MinValue", "0")
    Q_CLASSINFO("MaxValue", "10")
    Q_CLASSINFO("Default", "3.5")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE PowerLawGrainSizeDistribution();

protected:
    /** This function verifies the property values. */
    void setupSelfBefore();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the exponent \f$\gamma\f$ in the power-law distribution function. */
    Q_INVOKABLE void setExponent(double value);

    /** Returns the exponent \f$\gamma\f$ in the power-law distribution function. */
    Q_INVOKABLE double exponent() const;

    //======================== Other Functions =======================

public:
    /** This function implements the missing part of the GrainSizeDistributionInterface. It returns
        the value of \f$\Omega(a) = C\, a^{-\gamma}\f$. */
    double dnda(double a) const;

    //======================== Data Members ========================

protected:
    // discoverable attributes
    double _gamma;
};

////////////////////////////////////////////////////////////////////

#endif // POWERLAWGRAINSIZEDISTRIBUTION_HPP
