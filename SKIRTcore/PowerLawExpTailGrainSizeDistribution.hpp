/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef POWERLAWEXPTAILGRAINSIZEDISTRIBUTION_HPP
#define POWERLAWEXPTAILGRAINSIZEDISTRIBUTION_HPP

#include "RangeGrainSizeDistribution.hpp"

////////////////////////////////////////////////////////////////////

/** PowerLawExpTailGrainSizeDistribution is a GrainSizeDistribution subclass that represents a dust
    grain size distribution of the form \f[ \Omega(a) = (\frac{\text{d}n_\text{D}}{\text{d}a}) /
    n_\text{H} = C \,a^{-\alpha} \,f_\text{ed}(a) \qquad\text{for}\quad a_\text{min} \leq a \leq
    a_\text{max}, \f] with an exponential decay term \f[ f_\text{ed}(a) = \begin{cases} 1 & \quad
    a\leq a_\text{t} \\ \exp\left(-[(a-a_\text{t})/a_\text{c}]^\gamma \right) & \quad a>a_\text{t}
    \end{cases} \f].

    The size range and the proportionality factor \f$C\f$ of the function can be configured in the
    GrainSizeDistribution base class. The remaining four parameters \f$\alpha\f$, \f$a_\text{t}\f$,
    \f$a_\text{c}\f$ and \f$\gamma\f$ can be configured as attributes in this class.

    The functional form for the grain size distribution implemented by this class is inspired by the
    Jones dust model, which is described in Jones et al. 2013 (A&A, 558, A62). */
class PowerLawExpTailGrainSizeDistribution: public RangeGrainSizeDistribution
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a power-law with exponential tail dust grain size distribution")

    Q_CLASSINFO("Property", "alpha")
    Q_CLASSINFO("Title", "the (absolute value of the) exponent α of the power law")
    Q_CLASSINFO("MinValue", "0")
    Q_CLASSINFO("MaxValue", "10")
    Q_CLASSINFO("Default", "3.5")

    Q_CLASSINFO("Property", "turnoff")
    Q_CLASSINFO("Title", "the turn-off point of the exponential decay term")
    Q_CLASSINFO("Quantity", "grainsize")
    Q_CLASSINFO("MinValue", "0")
    Q_CLASSINFO("MaxValue", "1 mm")
    Q_CLASSINFO("Default", "0.1 micron")

    Q_CLASSINFO("Property", "scalefactor")
    Q_CLASSINFO("Title", "the scale factor in the exponential decay term")
    Q_CLASSINFO("Quantity", "grainsize")
    Q_CLASSINFO("MinValue", "1 A")
    Q_CLASSINFO("MaxValue", "1 mm")
    Q_CLASSINFO("Default", "0.1 micron")

    Q_CLASSINFO("Property", "gamma")
    Q_CLASSINFO("Title", "the exponent γ in the exponential decay term")
    Q_CLASSINFO("MinValue", "0")
    Q_CLASSINFO("MaxValue", "10")
    Q_CLASSINFO("Default", "3")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE PowerLawExpTailGrainSizeDistribution();

protected:
    /** This function verifies the property values. */
    void setupSelfBefore();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the exponent \f$\alpha\f$ of the power law. */
    Q_INVOKABLE void setAlpha(double value);

    /** Returns the exponent \f$\alpha\f$ of the power law. */
    Q_INVOKABLE double alpha() const;

    /** Sets the turn-off point \f$a_\text{t}\f$ for the exponential decay term. */
    Q_INVOKABLE void setTurnoff(double value);

    /** Returns the turn-off point \f$a_\text{t}\f$ for the exponential decay term. */
    Q_INVOKABLE double turnoff() const;

    /** Sets the scale factor \f$a_\text{c}\f$ in the exponential decay term. */
    Q_INVOKABLE void setScalefactor(double value);

    /** Returns the scale factor \f$a_\text{c}\f$ in the exponential decay term. */
    Q_INVOKABLE double scalefactor() const;

    /** Sets the exponent \f$\gamma\f$ in the exponential decay term. */
    Q_INVOKABLE void setGamma(double value);

    /** Returns the exponent \f$\gamma\f$ in the exponential decay term. */
    Q_INVOKABLE double gamma() const;

    //======================== Other Functions =======================

public:
    /** This function implements the missing part of the GrainSizeDistributionInterface. It returns
        the value of \f$\Omega(a)\f$ as described in the header for this class. */
    double dnda(double a) const;

    //======================== Data Members ========================

protected:
    // discoverable attributes
    double _alpha;
    double _at;
    double _ac;
    double _gamma;
};

////////////////////////////////////////////////////////////////////

#endif // POWERLAWEXPTAILGRAINSIZEDISTRIBUTION_HPP
