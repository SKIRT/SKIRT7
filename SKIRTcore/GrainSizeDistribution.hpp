/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

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

    The GrainSizeDistribution class publishes the GrainSizeDistributionInterface that provides
    access to the size distribution range and function. It expects each subclass to implement the
    functions declared in this interface, i.e. the functions amin() and amax() to specify the grain
    size range, and the function dnda() to specify the grain size distribution function within
    that range. For historical reasons, the latter function is named dnda() while it in fact
    returns the value of \f$\Omega(a)\f$ defined above.

    This base class manages the attribute \f$C\f$, a proportionality factor that should be used by
    subclasses as front factor in the function \f$\Omega(a)\f$.
*/
class GrainSizeDistribution: public SimulationItem, public GrainSizeDistributionInterface
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a dust grain size distribution")

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
    /** Sets the proportionality factor \f$C\f$ in the size distribution function. */
    Q_INVOKABLE void setFactor(double value);

    /** Returns the proportionality factor \f$C\f$ in the size distribution function. */
    Q_INVOKABLE double factor() const;

    //======================== Data Members ========================

protected:
    // discoverable attributes
    double _C;
};

////////////////////////////////////////////////////////////////////

#endif // GRAINSIZEDISTRIBUTION_HPP
