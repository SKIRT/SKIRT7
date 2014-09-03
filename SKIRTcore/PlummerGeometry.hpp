/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef PLUMMERGEOMETRY_HPP
#define PLUMMERGEOMETRY_HPP

#include "SpheGeometry.hpp"

////////////////////////////////////////////////////////////////////

/** The PlummerGeometry class is a subclass of the SpheGeometry class, and describes spherically
    symmetric geometries characterized by a Plummer density profile, \f[ \rho(r) = \rho_0\,
    \left(1+\frac{r^2}{c^2}\right)^{-5/2}. \f] The only free parameter is the scale length \f$c\f$.
    See Plummer (1911, MNRAS, 71, 460–470) and Dejonghe (1987, MNRAS, 224, 13–39). */
class PlummerGeometry : public SpheGeometry
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a Plummer geometry")

    Q_CLASSINFO("Property", "scale")
    Q_CLASSINFO("Title", "the scale length")
    Q_CLASSINFO("Quantity", "length")
    Q_CLASSINFO("MinValue", "0")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE PlummerGeometry();

protected:
    /** This function verifies the validity of the scale length \f$c\f$. The central density
        \f$\rho_0\f$ is set by the normalization condition that the total mass equals one. For the
        Plummer model we find \f[ \rho_0 = \frac{3}{4\pi\,c^3}.\f] */
    void setupSelfBefore();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the scale length \f$c\f$. */
    Q_INVOKABLE void setScale(double value);

    /** Returns the scale length \f$c\f$. */
    Q_INVOKABLE double scale() const;

    //======================== Other Functions =======================

public:
    /** This function returns the density \f$\rho(r)\f$ at a radius \f$r\f$. It just implements the
        analytical formula. */
    double density(double r) const;

    /** This function returns the radius of a random position drawn from the Plummer density
        distribution. This is accomplished by generating a uniform deviate \f${\cal{X}}\f$, and
        solving the equation \f[ {\cal{X}} = M(r) = 4\pi \int_0^r \rho(r')\, r'{}^2\, {\text{d}}r'
        \f] for \f$r\f$. For the Plummer model, we obtain the simple expression \f[ r = c\,
        \frac{{\cal{X}}^{1/3}}{\sqrt{1-{\cal{X}}^{2/3}}}. \f] */
    double randomradius() const;

    /** This function returns the radial surface density, i.e. the integration of
        the density along a line starting at the centre of the coordinate
        system, \f[ \Sigma_r = \int_0^\infty \rho(r)\,{\text{d}}r. \f] For the Plummer geometry,
        one obtains \f[ \Sigma_r = \frac{1}{2\pi\,c^2}. \f] */
    double Sigmar() const;

    //======================== Data Members ========================

private:
    // data members for which there are setters and getters
    double _c;

    // data members initialized during setup
    double _rho0;
};

////////////////////////////////////////////////////////////////////

#endif // PLUMMERGEOMETRY_HPP
