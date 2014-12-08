/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef SINGLEGRAINSIZEDISTRIBUTION_HPP
#define SINGLEGRAINSIZEDISTRIBUTION_HPP

#include "GrainSizeDistribution.hpp"

////////////////////////////////////////////////////////////////////

/** SingleGrainSizeDistribution represents a grain size distribution approximating a delta function
    at some specific grain size. The single grain size is configured through an attribute managed
    by this class. The amin() and amax() functions in this class return a very narrow range of
    width \f$\Delta a=a_\text{s}/1000\f$ centered on the specified size \f$a_\text{s}\f$, and the
    function dnda() returns a constant distribution normalized to the proportionality factor
    \f$C\f$ managed by the GrainSizeDistribution base class: \f[ \Omega(a) = \frac{C}{\Delta a}
    \qquad \text{for} \quad a_\text{s} - \frac{1}{2}\Delta a \leq a \leq a_\text{s} +
    \frac{1}{2}\Delta a. \f] */
class SingleGrainSizeDistribution: public GrainSizeDistribution
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a single-size dust grain size distribution")

    Q_CLASSINFO("Property", "size")
    Q_CLASSINFO("Title", "the single grain size for this distribution")
    Q_CLASSINFO("Quantity", "grainsize")
    Q_CLASSINFO("MinValue", "1 A")
    Q_CLASSINFO("MaxValue", "1 mm")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE SingleGrainSizeDistribution();

protected:
    /** This function verifies the property values. */
    void setupSelfBefore();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the single grain size \f$a_\text{s}\f$ for this distribution. */
    Q_INVOKABLE void setSize(double value);

    /** Returns the single grain size \f$a_\text{s}\f$ for this distribution. */
    Q_INVOKABLE double size() const;

    //======================== Other Functions =======================

public:
    /** This function implements part of the GrainSizeDistributionInterface. It returns the minimum
        grain size \f$a_\text{min} = a_\text{s} - \frac{1}{2}\Delta a\f$. */
    double amin() const;

    /** This function implements part of the GrainSizeDistributionInterface. It returns the maximum
        grain size \f$a_\text{max} = a_\text{s} + \frac{1}{2}\Delta a\f$. */
    double amax() const;

    /** This function implements part of the GrainSizeDistributionInterface. It returns the value
        of \f$\Omega(a) = C/\Delta a\f$. */
    double dnda(double a) const;

    //======================== Data Members ========================

protected:
    // discoverable attributes
    double _as;
};

////////////////////////////////////////////////////////////////////

#endif // SINGLEGRAINSIZEDISTRIBUTION_HPP
