/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef MODIFIEDLOGNORMALGRAINSIZEDISTRIBUTION_HPP
#define MODIFIEDLOGNORMALGRAINSIZEDISTRIBUTION_HPP

#include "LogNormalGrainSizeDistribution.hpp"

////////////////////////////////////////////////////////////////////

/** ModifiedLogNormalGrainSizeDistribution is a GrainSizeDistribution subclass that represents a
    modified log-normal dust grain size distribution of the form \f[ \Omega(a) =
    (\frac{\text{d}n_\text{D}}{\text{d}a}) / n_\text{H} = C \,\frac{1}{a} \,\exp\left[ -
    \frac{(\ln(a/a_0))^2}{2\sigma^2} \right] \, M(a) \qquad \text{for}\quad a_\text{min} \leq a
    \leq a_\text{max} \f] with a mixing term \f[ M(a) = y_0 +
    (y_1-y_0)\frac{\ln(a/a_\text{min})}{\ln(a_\text{max}/a_\text{min})}. \f]

    The size range and the proportionality factor \f$C\f$ of the function can be configured in the
    GrainSizeDistribution base class. The centroid \f$a_0\f$ and the width \f$\sigma\f$ can be
    configured in the LogNormalGrainSizeDistribution class. The remaining two parameters \f$y_0\f$
    and \f$y_1\f$ can be configured as attributes in this class.

    The functional form for the grain size distribution implemented by this class is inspired by the
    DustEM code, which is described in Compiègne et al. 2011 (AA, 525, A103) and can be downloaded
    from http://www.ias.u-psud.fr/DUSTEM/. */
class ModifiedLogNormalGrainSizeDistribution: public LogNormalGrainSizeDistribution
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a modified log-normal dust grain size distribution")

    Q_CLASSINFO("Property", "y0")
    Q_CLASSINFO("Title", "the first mixing parameter y0")
    Q_CLASSINFO("MinValue", "0")
    Q_CLASSINFO("MaxValue", "1")
    Q_CLASSINFO("Default", "1")

    Q_CLASSINFO("Property", "y1")
    Q_CLASSINFO("Title", "the second mixing parameter y1")
    Q_CLASSINFO("MinValue", "0")
    Q_CLASSINFO("MaxValue", "1")
    Q_CLASSINFO("Default", "1")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE ModifiedLogNormalGrainSizeDistribution();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the first mixing parameter \f$y_0\f$. */
    Q_INVOKABLE void setY0(double value);

    /** Returns the first mixing parameter \f$y_0\f$. */
    Q_INVOKABLE double y0() const;

    /** Sets the second mixing parameter \f$y_1\f$. */
    Q_INVOKABLE void setY1(double value);

    /** Returns the second mixing parameter \f$y_1\f$. */
    Q_INVOKABLE double y1() const;

    //======================== Other Functions =======================

public:
    /** This function implements the missing part of the GrainSizeDistributionInterface. It returns
        the value of \f$\Omega(a)\f$ as described in the header for this class. */
    double dnda(double a) const;

    //======================== Data Members ========================

protected:
    // discoverable attributes
    double _y0;
    double _y1;
};

////////////////////////////////////////////////////////////////////

#endif // MODIFIEDLOGNORMALGRAINSIZEDISTRIBUTION_HPP
