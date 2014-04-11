/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef EINASTOGEOMETRY_HPP
#define EINASTOGEOMETRY_HPP

#include "Array.hpp"
#include "SpheGeometry.hpp"

////////////////////////////////////////////////////////////////////

/** The EinastoGeometry class is a subclass of the SpheGeometry class, and
    describes spherical geometries characterized by an Einasto density profile, \f[ \rho(r)
    = \rho_{\text{s}}\, \exp\left\{-d_n \left[ \left(
    \frac{r}{r_{\text{s}}}\right)^{1/n}-1\right] \right\}.\f] This geometry has
    two free parameters: the half mass radius \f$r_{\text{s}}\f$, and the Einasto index
    \f$n\f$. The quantity \f$d_n\f$ is not a free parameter, but a numerical
    constant that guarantees that \f$r_{\text{s}}\f$ is the radius of the sphere that contains
    half of the total mass. A numerical approximation for \f$d_n\f$ is provided by
    Retana-Montenegro et al. (2012): \f[ d_n = 3n + -\frac13 + \frac{8}{1215\,n} +
    \frac{184}{229635\,n^2} + \frac{1048}{31000725\,n^3}. \f] */
class EinastoGeometry : public SpheGeometry
{
    Q_OBJECT
    Q_CLASSINFO("Title", "an Einasto geometry")

    Q_CLASSINFO("Property", "radius")
    Q_CLASSINFO("Title", "the half-mass radius")
    Q_CLASSINFO("Quantity", "length")
    Q_CLASSINFO("MinValue", "0")

    Q_CLASSINFO("Property", "index")
    Q_CLASSINFO("Title", "the Einasto index n")
    Q_CLASSINFO("MinValue", "0")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE EinastoGeometry();

protected:
    /** This function verifies the validity of the half-mass radius \f$r_{\text{s}}\f$. The
        half-mass density \f$\rho_{\text{s}}\f$ is set by the normalization condition that the
        total mass is equal to one. For the Einasto model we find (see Retana-Montenegro
        et al. 2012) \f[ \rho_{\text{s}} = \frac{d_n^{3n}}{ 4\pi\,{\text{e}}^{d_n}\,
        n\, \Gamma(3n)\,r_{\text{s}}^3 }. \f] We also set of a vector with the cumulative
        mass \f[ M(r) = 4\pi \int_0^r \rho(r')\, r'^2\, {\text{d}}r' \f] at a large number
        of radii. For the Einasto model, \f[ M(r) =
        P\left[3n,d_n\left(\frac{r}{r_{\text{s}}}\right)^{1/n}\right] \f] where
        \f$P(a,x)\f$ is the incomplete Gamma function. */
    void setupSelfBefore();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the half-mass radius \f$r_{\text{s}}\f$. */
    Q_INVOKABLE void setRadius(double value);

    /** Returns the half-mass radius \f$r_{\text{s}}\f$. */
    Q_INVOKABLE double radius() const;

    /** Sets the Einasto index \f$n\f$. */
    Q_INVOKABLE void setIndex(double value);

    /** Returns the Einasto index \f$n\f$. */
    Q_INVOKABLE double index() const;

    //======================== Other Functions =======================

public:
    /** This function returns the density \f$\rho(r)\f$ at the radius \f$r\f$. It just
        implements the analytical formula. */
    double density(double r) const;

    /** This function returns the radius of a random position drawn from the Einasto
        geometry. We just use the vector of cumulative masses stored internally. */
    double randomradius() const;

    /** This function returns the radial surface density, i.e. the integration of
        the density along a line starting at the centre of the coordinate
        system, \f[ \Sigma_r = \int_0^\infty \rho(r)\,{\text{d}}r. \f] For the Einasto
        geometry, one obtains
        \f[ \Sigma_r = n\,Gamma(n)\, \rho_{\text{s}}\, r_{\text{s}}\,
        \frac{{\text{e}}^{d_n}}{d_n^n}. \f] */
    double Sigmar() const;

    //======================== Data Members ========================

private:
    // data members for which there are setters and getters
    double _rs;
    double _n;

    // data members initialized during setup
    double _dn;
    double _rhos;
    Array _rv;
    Array _Xv;
};

////////////////////////////////////////////////////////////////////

#endif // EINASTOGEOMETRY_HPP
