/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef STATICSPHEREHIDISTRIBUTION_HPP
#define STATICSPHEREHIDISTRIBUTION_HPP

#include "AnalyticalHIDistribution.hpp"
class Random;

//////////////////////////////////////////////////////////////////////

/** The StaticSphereHIDistribution class represents a static, uniform density sphere of
    neutral hydrogen gas. */
class StaticSphereHIDistribution : public AnalyticalHIDistribution
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a static uniform sphere")

    Q_CLASSINFO("Property", "radius")
    Q_CLASSINFO("Title", "the radius")
    Q_CLASSINFO("Quantity", "length")
    Q_CLASSINFO("MinValue", "0")

    Q_CLASSINFO("Property", "temperature")
    Q_CLASSINFO("Title", "the gas temperature")
    Q_CLASSINFO("Quantity", "temperature")
    Q_CLASSINFO("MinValue", "0")

    Q_CLASSINFO("Property", "mass")
    Q_CLASSINFO("Title", "the total mass")
    Q_CLASSINFO("Quantity", "mass")
    Q_CLASSINFO("MinValue", "0")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE StaticSphereHIDistribution();

protected:
    /** This function verifies the validity of the arguments. */
    void setupSelfBefore();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the radius \f$R\f$. */
    Q_INVOKABLE void setRadius(double value);

    /** Returns the radius \f$R\f$. */
    Q_INVOKABLE double radius() const;

    /** Sets the gas temperature. */
    Q_INVOKABLE void setTemperature(double value);

    /** Returns the gas temperature. */
    Q_INVOKABLE double temperature() const;

    /** Sets the total neutral hydrogen gas mass. */
    Q_INVOKABLE void setMass(double value);

    /** Returns the total neutral hydrogen gas mass. */
    Q_INVOKABLE double mass() const;

    //======================== Other Functions =======================

public:
    /** This function returns the dimension of the neutral hydrogen distribution. In this case 1 is returned. */
    int dimension() const;

    /** This function returns the mass density \f$\rho({\bf{r}})\f$ at the position \f${\bf{r}}\f$. The
    constant density is returned if the position lies within the sphere. */
    double density(Position bfr) const;

    /** This function returns the gas temperature \f$T({\bf{r}})\f$ at the position \f${\bf{r}}\f$. */
    double gastemperature(Position bfr) const;

    /** This function returns the bulk velocity \f$\bar{\bf{v}}({\bf{r}})\f$ at the position \f${\bf{r}}\f$. For
     a static sphere, the bulk velocity is zero everywhere. */
    Vec bulkvelocity(Position bfr) const;

    /** This pure virtual function generates a random position from the neutral hydrogen gas distribution, by
        drawing a random point from the three-dimensional probability density \f$p({\bf{r}})\,
        {\text{d}}{\bf{r}} = \rho({\bf{r}})\, {\text{d}}{\bf{r}}\f$, where \f$\rho({\bf{r}})\f$ is
        the total mass density. This is trivial for a constant density sphere. */
    Position generatePosition() const;

    //======================== Data Members ========================

private:
    // data members for which there are setters and getters
    double _R;
    double _T;
    double _M;

    // data members initialized during setup
    double _rho;
    Random* _random;
};

//////////////////////////////////////////////////////////////////////

#endif // STATICSPHEREDUSTDISTRIBUTION_HPP
