/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef GAMMAGEOMETRY_HPP
#define GAMMAGEOMETRY_HPP

#include "SpheGeometry.hpp"

////////////////////////////////////////////////////////////////////

/** The GammaGeometry class is a subclass of the SpheGeometry class, and describes
    spherically symmetric geometries characterized by a gamma density profile, \f[ \rho(r)
    = \rho_0 \left( \frac{r}{b} \right)^{-\gamma} \left( 1 + \frac{r}{b} \right)^{\gamma-4}. \f]
    This model has two free parameters, namely the scale length \f$b\f$ and the logarithmic cusp
    slope \f$\gamma\f$, which should assume values between 0 and 3. The gamma models were
    introduced by Dehnen (1993, MNRAS, 265, 250) and Tremaine et al. (1994, AJ, 107, 634). Special
    cases are the Hernquist (1990, ApJ, 356, 359) model, corresponding to \f$\gamma=1\f$, and the
    Jaffe (1983, MNRAS, 202, 995) model, corresponding to \f$\gamma=2\f$. */
class GammaGeometry : public SpheGeometry
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a gamma geometry")

    Q_CLASSINFO("Property", "scale")
    Q_CLASSINFO("Title", "the scale length")
    Q_CLASSINFO("Quantity", "length")
    Q_CLASSINFO("MinValue", "0")

    Q_CLASSINFO("Property", "gamma")
    Q_CLASSINFO("Title", "the central density slope γ")
    Q_CLASSINFO("MinValue", "0")
    Q_CLASSINFO("MaxValue", "3")
    Q_CLASSINFO("Default", "1")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE GammaGeometry();

protected:
    /** This function verifies the validity of the scale length \f$b\f$ and the cusp slope
        \f$\gamma\f$. The central density \f$\rho_0\f$ is set by the normalization condition that
        the total mass is equal to one. For the gamma model we find \f[ \rho_0 =
        \frac{3-\gamma}{4\pi b^3}.\f] */
    void setupSelfBefore();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the scale length \f$b\f$. */
    Q_INVOKABLE void setScale(double value);

    /** Returns the scale length \f$b\f$. */
    Q_INVOKABLE double scale() const;

    /** Sets the central density slope \f$\gamma\f$. */
    Q_INVOKABLE void setGamma(double value);

    /** Returns the central density slope \f$\gamma\f$. */
    Q_INVOKABLE double gamma() const;

    //======================== Other Functions =======================

public:
    /** This function returns the density \f$\rho(r)\f$ at a radius \f$r\f$. It just implements the
        analytical formula. */
    double density(double r) const;

    /** This function returns the radius of a random position drawn from the gamma
        distribution. This is accomplished by generating a uniform deviate \f${\cal{X}}\f$, and
        solving the equation \f[ {\cal{X}} = M(r) = 4\pi \int_0^r \rho(r')\, r'{}^2\, {\text{d}}r'
        \f] for \f$r\f$. For the gamma models, we obtain the simple expression \f[ r = b\, \frac{
        {\cal{X}}^{1/(3-\gamma)} }{ 1- {\cal{X}}^{1/(3-\gamma)} }. \f] */
    double randomradius() const;

    /** This function returns the radial surface density, i.e. the integration of
        the density along a line starting at the centre of the coordinate
        system, \f[ \Sigma_r = \int_0^\infty \rho(r)\,{\text{d}}r. \f] For the gamma
        geometry, this integral is
        infinity if \f$\gamma>1\f$, otherwise it is equal to \f[ \Sigma_r =
        \frac{1}{2\pi\,b^2\,(1-\gamma)\,(2-\gamma)}. \f] */
    double Sigmar() const;

    //======================== Data Members ========================

private:
    // data members for which there are setters and getters
    double _b;
    double _gamma;

    // data members initialized during setup
    double _rho0;
};

////////////////////////////////////////////////////////////////////

#endif // GAMMAGEOMETRY_HPP
