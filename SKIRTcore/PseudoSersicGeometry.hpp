/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef PSEUDOSERSICGEOMETRY_HPP
#define PSEUDOSERSICGEOMETRY_HPP

#include "Array.hpp"
#include "SpheGeometry.hpp"

////////////////////////////////////////////////////////////////////

/** The PseudoSersicGeometry class is a subclass of the SpheGeometry class, and
    describes geometries that are a very good approximation of a Sersic density
    profile. This geometry, introduced by Prugniel & Simien (1997, A&A, 321,111) is an
    approximation of the Sersic geometry and has the advantage that the space density is
    known analytically rather than the density projected on the sky. The pseudo-Sersic
    geometry is characterized by the density profile, \f[ \rho(r) = \rho_n
    \left(\frac{r}{r_{\text{eff}}}\right)^{\frac{1}{2n}-1}\,
    \exp\left[-b_n\left(\frac{r}{r_{\text{eff}}}\right)^\frac{1}{n}\right]. \f] The
    parameter \f$b_n\f$ is a dimensionless number given by \f[ b_n = 2n -\frac{1}{3} +
    \frac{4}{405n} + \frac{46}{25515n^2} + \frac{131}{1148175n^3}. \f] The geometry has
    two free parameters: the Sersic index \f$n\f$ and the effective radius
    \f$r_{\text{eff}}\f$. */
class PseudoSersicGeometry : public SpheGeometry
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a pseudo-Sersic geometry")

    Q_CLASSINFO("Property", "index")
    Q_CLASSINFO("Title", "the Sersic index n")
    Q_CLASSINFO("MinValue", "0.5")
    Q_CLASSINFO("MaxValue", "10")
    Q_CLASSINFO("Default", "1")

    Q_CLASSINFO("Property", "radius")
    Q_CLASSINFO("Title", "the effective radius")
    Q_CLASSINFO("Quantity", "length")
    Q_CLASSINFO("MinValue", "0")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE PseudoSersicGeometry();

protected:
    /** This function verifies the validity of the Sersic index \f$n\f$ and the effective radius
        \f$r_{\text{eff}}\f$. The density scale \f$\rho_n\f$
        is set by the normalization condition that the total mass is equal to one. We easily find
        that \f[ \rho_n = \frac{b_n^{2n+\frac12}}{4\pi\,r_{\text{eff}}^3\, n\,
        \Gamma(2n+\tfrac12)}. \f] We also set of a vector with the cumulative mass
        \f[ M(r) = 4\pi \int_0^r \rho(r')\, r'^2\, {\text{d}}r' \f] at a large number
        of radii. For the pseudo-Sersic model, \f[ M(r) =
        P\left[\frac12+2n,b_n\left(\frac{r}{r_{\text{e}}}\right)^{1/n}\right] \f] where
        \f$P(a,x)\f$ is the incomplete Gamma function. */
    void setupSelfBefore();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the Sersic index \f$n\f$. */
    Q_INVOKABLE void setIndex(double value);

    /** Returns the Sersic index \f$n\f$. */
    Q_INVOKABLE double index() const;

    /** Sets the effective radius. */
    Q_INVOKABLE void setRadius(double value);

    /** Returns the effective radius. */
    Q_INVOKABLE double radius() const;

    //======================== Other Functions =======================

public:
    /** This function returns the density \f$\rho(r)\f$ at the radius \f$r\f$. It just
        implements the analytical formula. */
    double density(double r) const;

    /** This function returns the radius of a random position drawn from the pseudo-Sersic
        geometry. We just use the vector of cumulative masses stored internally. */
    double randomradius() const;

    /** This function returns the radial surface density, i.e. the integration of
        the density along a line starting at the centre of the coordinate
        system, \f[ \Sigma_r = \int_0^\infty \rho(r)\,{\text{d}}r. \f] For the pseudo-Sersic
        geometry, one finds \f[ \Sigma_r = \frac{\sqrt{\pi}\, n\, \rho_0\,
        r_{\text{eff}}}{\sqrt{b_n}}. \f] */
    double Sigmar() const;

    //======================== Data Members ========================

private:
    // data members for which there are setters and getters
    double _n;
    double _reff;

    // data members initialized during setup
    double _bn;
    double _rhon;
    Array _rv;
    Array _Xv;
};

////////////////////////////////////////////////////////////////////

#endif // PSEUDOSERSICGEOMETRY_HPP
