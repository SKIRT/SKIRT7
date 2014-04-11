/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef GRAINSIZEDISTRIBUTION_HPP
#define GRAINSIZEDISTRIBUTION_HPP

#include "GrainSizeDistributionInterface.hpp"
#include "SimulationItem.hpp"

////////////////////////////////////////////////////////////////////

/** GrainSizeDistribution is an abstract class that represents a size distribution for the dust
    grains in a particular dust population. Specifically, it represents a function \f[
    \Omega(a)=(\frac{\text{d}n_\text{D}}{\text{d}a})/n_\text{H} \qquad \text{for}\quad a_\text{min}
    \leq a \leq a_\text{max}, \f] that specifies the number of dust grains with size \f$a\f$ per
    hydrogen atom.

    This base class manages the attributes \f$a_\text{min}\f$ and \f$a_\text{max}\f$, which
    determine the range of the distribution, and the attribute \f$C\f$, a proportionality factor
    that should be used by subclasses as front factor in the function \f$\Omega(a)\f$.

    The GrainSizeDistribution class provides the GrainSizeDistributionInterface, offering public
    access to the size distribution range and function. This base class itself implements the
    functions that expose \f$a_\text{min}\f$ and \f$a_\text{max}\f$, while it expects each subclass
    to provide the actual distribution function by implementing the dnda() function declared in the
    GrainSizeDistributionInterface (using \f$C\f$ as front factor). For historical reasons, the
    function is named dnda() while it in fact returns the value of \f$\Omega(a)\f$ defined above.
    */
class GrainSizeDistribution: public SimulationItem, public GrainSizeDistributionInterface
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a dust grain size distribution")

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

    Q_CLASSINFO("Property", "factor")
    Q_CLASSINFO("Title", "the proportionality factor in the size distribution function")
    Q_CLASSINFO("MinValue", "0")
    Q_CLASSINFO("Default", "1")

    //============= Construction - Setup - Destruction =============

protected:
    /** The default constructor; it is protected since this is an abstract class. */
    GrainSizeDistribution();

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

    /** Sets the proportionality factor \f$C\f$ in the size distribution function. */
    Q_INVOKABLE void setFactor(double value);

    /** Returns the proportionality factor \f$C\f$ in the size distribution function. */
    Q_INVOKABLE double factor() const;

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
    double _C;
};

////////////////////////////////////////////////////////////////////

#endif // GRAINSIZEDISTRIBUTION_HPP
