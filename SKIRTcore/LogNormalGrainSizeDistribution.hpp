/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef LOGNORMALGRAINSIZEDISTRIBUTION_HPP
#define LOGNORMALGRAINSIZEDISTRIBUTION_HPP

#include "RangeGrainSizeDistribution.hpp"

////////////////////////////////////////////////////////////////////

/** LogNormalGrainSizeDistribution is a GrainSizeDistribution subclass that represents a log-normal
    dust grain size distribution of the form \f[ \Omega(a) = (\frac{\text{d}n_\text{D}}{\text{d}a})
    / n_\text{H} = C \,\frac{1}{a} \,\exp\left[ - \frac{(\ln(a/a_0))^2}{2\sigma^2} \right]
    \qquad \text{for}\quad a_\text{min} \leq a \leq a_\text{max}. \f]

    The size range and the proportionality factor \f$C\f$ of the function can be configured in the
    GrainSizeDistribution base class. The remaining two parameters, the centroid \f$a_0\f$ and the
    width \f$\sigma\f$, can be configured as attributes in this class.

    The functional form for the grain size distribution implemented by this class is inspired by the
    DustEM code, which is described in Compiègne et al. 2011 (AA, 525, A103) and can be downloaded
    from http://www.ias.u-psud.fr/DUSTEM/. */
class LogNormalGrainSizeDistribution: public RangeGrainSizeDistribution
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a log-normal dust grain size distribution")

    Q_CLASSINFO("Property", "centroid")
    Q_CLASSINFO("Title", "the centroid a0 of the log-normal law")
    Q_CLASSINFO("Quantity", "grainsize")
    Q_CLASSINFO("MinValue", "0")
    Q_CLASSINFO("MaxValue", "1 mm")
    Q_CLASSINFO("Default", "1 nm")

    Q_CLASSINFO("Property", "width")
    Q_CLASSINFO("Title", "the width σ of the log-normal law")
    Q_CLASSINFO("MinValue", "0")
    Q_CLASSINFO("MaxValue", "5")
    Q_CLASSINFO("Default", "0.4")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE LogNormalGrainSizeDistribution();

protected:
    /** This function verifies the property values. */
    void setupSelfBefore();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the centroid \f$a_0\f$ of the log-normal law. */
    Q_INVOKABLE void setCentroid(double value);

    /** Returns the centroid \f$a_0\f$ of the log-normal law. */
    Q_INVOKABLE double centroid() const;

    /** Sets the width \f$\sigma\f$ of the log-normal law. */
    Q_INVOKABLE void setWidth(double value);

    /** Returns the width \f$\sigma\f$ of the log-normal law. */
    Q_INVOKABLE double width() const;

    //======================== Other Functions =======================

public:
    /** This function implements the missing part of the GrainSizeDistributionInterface. It returns
        the value of \f$\Omega(a)\f$ as described in the header for this class. */
    double dnda(double a) const;

    //======================== Data Members ========================

protected:
    // discoverable attributes
    double _a0;
    double _sigma;
};

////////////////////////////////////////////////////////////////////

#endif // LOGNORMALGRAINSIZEDISTRIBUTION_HPP
