/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef MODIFIEDPOWERLAWGRAINSIZEDISTRIBUTION_HPP
#define MODIFIEDPOWERLAWGRAINSIZEDISTRIBUTION_HPP

#include "GrainSizeDistribution.hpp"

////////////////////////////////////////////////////////////////////

/** ModifiedPowerLawGrainSizeDistribution is a GrainSizeDistribution subclass that represents a
    dust grain size distribution of the form \f[ \Omega(a) = (\frac{\text{d}n_\text{D}}{\text{d}a})
    / n_\text{H} = C \,a^{\alpha} \,f_\text{ed}(a) \,f_\text{cv}(a) \qquad\text{for}\quad
    a_\text{min} \leq a \leq a_\text{max}, \f] with an exponential decay term \f[ f_\text{ed}(a) =
    \begin{cases} 1 & \quad a\leq a_\text{t} \\ \exp\left(-[(a-a_\text{t})/a_\text{c}]^\gamma
    \right) & \quad a>a_\text{t} \end{cases} \f] and a curvature term \f[ f_\text{cv}(a) = \left[
    1+|\zeta|\,(a/a_u)^\eta \right]^{\text{sign}(\zeta)}. \f]

    The size range and the proportionality factor \f$C\f$ of the function can be configured in the
    GrainSizeDistribution base class. The remaining seven parameters \f$\alpha\f$, \f$a_\text{t}\f$,
    \f$a_\text{c}\f$, \f$\gamma\f$, \f$a_\text{u}\f$, \f$\zeta\f$ and \f$\eta\f$ can be configured
    as attributes in this class.

    The functional form for the grain size distribution implemented by this class is inspired by the
    DustEM code, which is described in Compiègne et al. 2011 (AA, 525, A103) and can be downloaded
    from http://www.ias.u-psud.fr/DUSTEM/. */
class ModifiedPowerLawGrainSizeDistribution: public GrainSizeDistribution
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a modified power-law dust grain size distribution")

    Q_CLASSINFO("Property", "alpha")
    Q_CLASSINFO("Title", "the index α of the power law")
    Q_CLASSINFO("MinValue", "-10")
    Q_CLASSINFO("MaxValue", "0")
    Q_CLASSINFO("Default", "-3.5")

    Q_CLASSINFO("Property", "at")
    Q_CLASSINFO("Title", "the turn-off point a_t in the exponential decay term")
    Q_CLASSINFO("Quantity", "grainsize")
    Q_CLASSINFO("MinValue", "0")
    Q_CLASSINFO("MaxValue", "1 mm")
    Q_CLASSINFO("Default", "0.1 micron")

    Q_CLASSINFO("Property", "ac")
    Q_CLASSINFO("Title", "the scale a_c in the exponential decay term")
    Q_CLASSINFO("Quantity", "grainsize")
    Q_CLASSINFO("MinValue", "1 A")
    Q_CLASSINFO("MaxValue", "1 mm")
    Q_CLASSINFO("Default", "0.1 micron")

    Q_CLASSINFO("Property", "gamma")
    Q_CLASSINFO("Title", "the exponent γ in the exponential decay term")
    Q_CLASSINFO("MinValue", "0")
    Q_CLASSINFO("MaxValue", "10")
    Q_CLASSINFO("Default", "3")

    Q_CLASSINFO("Property", "au")
    Q_CLASSINFO("Title", "the scale a_u in the curvature term")
    Q_CLASSINFO("Quantity", "grainsize")
    Q_CLASSINFO("MinValue", "1 A")
    Q_CLASSINFO("MaxValue", "1 mm")
    Q_CLASSINFO("Default", "0.1 micron")

    Q_CLASSINFO("Property", "zeta")
    Q_CLASSINFO("Title", "the strength ζ in the curvature term")
    Q_CLASSINFO("MinValue", "-10")
    Q_CLASSINFO("MaxValue", "10")
    Q_CLASSINFO("Default", "0.3")

    Q_CLASSINFO("Property", "eta")
    Q_CLASSINFO("Title", "the exponent η in the curvature term")
    Q_CLASSINFO("MinValue", "0")
    Q_CLASSINFO("MaxValue", "10")
    Q_CLASSINFO("Default", "1")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE ModifiedPowerLawGrainSizeDistribution();

protected:
    /** This function verifies the property values. */
    void setupSelfBefore();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the index \f$\alpha\f$ of the power law. */
    Q_INVOKABLE void setAlpha(double value);

    /** Returns the index \f$\alpha\f$ of the power law. */
    Q_INVOKABLE double alpha() const;

    /** Sets the turn-off point \f$a_\text{t}\f$ in the exponential decay term. */
    Q_INVOKABLE void setAt(double value);

    /** Returns the turn-off point \f$a_\text{t}\f$ in the exponential decay term. */
    Q_INVOKABLE double at() const;

    /** Sets the scale \f$a_\text{c}\f$ in the exponential decay term. */
    Q_INVOKABLE void setAc(double value);

    /** Returns the scale \f$a_\text{c}\f$ in the exponential decay term. */
    Q_INVOKABLE double ac() const;

    /** Sets the exponent \f$\gamma\f$ in the exponential decay term. */
    Q_INVOKABLE void setGamma(double value);

    /** Returns the exponent \f$\gamma\f$ in the exponential decay term. */
    Q_INVOKABLE double gamma() const;

    /** Sets the scale \f$a_\text{u}\f$ in the curvature term. */
    Q_INVOKABLE void setAu(double value);

    /** Returns the scale \f$a_\text{u}\f$ in the curvature term. */
    Q_INVOKABLE double au() const;

    /** Sets the strength \f$\zeta\f$ in the curvature term. */
    Q_INVOKABLE void setZeta(double value);

    /** Returns the strength \f$\zeta\f$ in the curvature term. */
    Q_INVOKABLE double zeta() const;

    /** Sets the exponent \f$\eta\f$ in the curvature term. */
    Q_INVOKABLE void setEta(double value);

    /** Returns the exponent \f$\eta\f$ in the curvature term. */
    Q_INVOKABLE double eta() const;

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
    double _au;
    double _zeta;
    double _eta;
};

////////////////////////////////////////////////////////////////////

#endif // MODIFIEDPOWERLAWGRAINSIZEDISTRIBUTION_HPP
