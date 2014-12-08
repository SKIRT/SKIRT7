/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef RANGEGRAINSIZEDISTRIBUTION_HPP
#define RANGEGRAINSIZEDISTRIBUTION_HPP

#include "GrainSizeDistribution.hpp"

////////////////////////////////////////////////////////////////////

/** RangeGrainSizeDistribution is an abstract class that represents a grain size distribution with
    a configurable grain size range. Specifically, it manages the attributes \f$a_\text{min}\f$ and
    \f$a_\text{max}\f$, which determine the range of the distribution, in addition to the
    proportionality factor \f$C\f$ managed by the GrainSizeDistribution base class. The
    RangeGrainSizeDistribution class consequently implements the functions amin() and amax(), while
    it still expects each subclass to provide the actual distribution function by implementing the
    dnda() function. */
class RangeGrainSizeDistribution: public GrainSizeDistribution
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a dust grain size distribution with a configurable size range")

    Q_CLASSINFO("Property", "minSize")
    Q_CLASSINFO("Title", "the minimum grain size for this distribution")
    Q_CLASSINFO("Quantity", "grainsize")
    Q_CLASSINFO("MinValue", "1 A")
    Q_CLASSINFO("MaxValue", "1 mm")

    Q_CLASSINFO("Property", "maxSize")
    Q_CLASSINFO("Title", "the maximum grain size for this distribution")
    Q_CLASSINFO("Quantity", "grainsize")
    Q_CLASSINFO("MinValue", "1 A")
    Q_CLASSINFO("MaxValue", "1 mm")

    //============= Construction - Setup - Destruction =============

protected:
    /** The default constructor; it is protected since this is an abstract class. */
    RangeGrainSizeDistribution();

    /** This function verifies the property values. */
    void setupSelfBefore();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the minimum grain size \f$a_\text{min}\f$ for this distribution. */
    Q_INVOKABLE void setMinSize(double value);

    /** Returns the minimum grain size \f$a_\text{min}\f$ for this distribution. */
    Q_INVOKABLE double minSize() const;

    /** Sets the maximum grain size \f$a_\text{max}\f$ for this distribution. */
    Q_INVOKABLE void setMaxSize(double value);

    /** Returns the maximum grain size \f$a_\text{max}\f$ for this distribution. */
    Q_INVOKABLE double maxSize() const;

    //======================== Other Functions =======================

public:
    /** This function implements part of the GrainSizeDistributionInterface. It returns the minimum
        grain size \f$a_\text{min}\f$. */
    double amin() const;

    /** This function implements part of the GrainSizeDistributionInterface. It returns the maximum
        grain size \f$a_\text{max}\f$. */
    double amax() const;

    //======================== Data Members ========================

protected:
    // discoverable attributes
    double _amin;
    double _amax;
};

////////////////////////////////////////////////////////////////////

#endif // RANGEGRAINSIZEDISTRIBUTION_HPP
