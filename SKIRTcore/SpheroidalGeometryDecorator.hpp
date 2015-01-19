/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef SPHEROIDALGEOMETRYDECORATOR_HPP
#define SPHEROIDALGEOMETRYDECORATOR_HPP

#include "SpheGeometry.hpp"
#include "AxGeometry.hpp"

////////////////////////////////////////////////////////////////////

/** The SpheroidalGeometryDecorator class is a geometry decorator that constructs a spheroidal
    geometry based on a spherical (isotropic) geometry. The properties of a
    SpheroidalGeometryDecorator object are a reference to the SpheGeometry object being decorated
    and the flattening parameter \f$q\f$. If the original spherical geometry is characterized by
    the density profile \f$ \rho_{\text{s}}(r) \f$, the new geometry has as density \f[ \rho(R,z) =
    \frac{1}{q}\, \rho_{\text{s}}\left(\sqrt{R^2 + \frac{z^2}{q^2}}\right). \f] This new geometry
    is also normalized to one. */
class SpheroidalGeometryDecorator : public AxGeometry
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a decorator that constructs a spheroidal variant of any spherical geometry")

    Q_CLASSINFO("Property", "geometry")
    Q_CLASSINFO("Title", "the spherical geometry to be made spheroidal")

    Q_CLASSINFO("Property", "flattening")
    Q_CLASSINFO("Title", "the flattening parameter q")
    Q_CLASSINFO("MinValue", "0")
    Q_CLASSINFO("MaxValue", "1")
    Q_CLASSINFO("Default", "1")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE SpheroidalGeometryDecorator();

    /** This function caches some values for use by subclasses. */
    void setupSelfBefore();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the original spherical geometry (i.e. the geometry being decorated). */
    Q_INVOKABLE void setGeometry(SpheGeometry* value);

    /** Returns the original spherical geometry. */
    Q_INVOKABLE SpheGeometry* geometry() const;

    /** Sets the flattening parameter \f$q\f$. */
    Q_INVOKABLE void setFlattening(double value);

    /** Returns the flattening parameter \f$q\f$. */
    Q_INVOKABLE double flattening() const;

    //======================== Other Functions =======================

public:
    /** This function returns the density \f$\rho(R,z)\f$ at the cylindrical radius \f$R\f$ and
        the height \f$z\f$. It just implements the analytical formula. */
    double density(double R, double z) const;

    /** This function generates a random position from the geometry, by drawing a random
        point from the three-dimensional probability density \f$p({\bf{r}})\, {\text{d}}{\bf{r}} =
        \rho({\bf{r}})\, {\text{d}}{\bf{r}}\f$. It first generates a random position
        \f${\bf{r}}_{\text{s}}\f$ by calling the generatePosition() function of the geometry
        being decorated and applies a simple linear transformation to the coordinates, \f$x = x_{\text{s}},
        y = y_{\text{s}}, z = q\,z_{\text{s}}\f$. */
    Position generatePosition() const;

    /** This function returns the radial surface density, i.e. the integration of
        the density along a line in the equatorial plane starting at the centre of the coordinate
        system, \f[ \Sigma_R = \int_0^\infty \rho(R,0)\,{\text{d}}R. \f] We easily obtain
        \f[ \Sigma_R = \frac{1}{q} \int_0^\infty \rho_{\text{orig}}(R)\,{\text{d}}R =
        \frac{1}{q}\,\Sigma_{r,{\text{orig}}}. \f] */
    double SigmaR() const;

    /** This function returns the Z-axis surface density, i.e. the integration of the density
        along the entire Z-axis, \f[ \Sigma_Z = \int_{-\infty}^\infty \rho(0,0,z)\,{\text{d}}z. \f]
        We easily obtain \f[ \Sigma_Z = \frac{2}{q} \int_0^\infty \rho_{\text{orig}}
        \left(\frac{z}{q}\right)\,{\text{d}}z =
        2 \int_0^\infty \rho_{\text{orig}}(r)\,{\text{d}}r = 2\,\Sigma_{r,{\text{orig}}}. \f] */
    double SigmaZ() const;

    //======================== Data Members ========================

private:
    // data members for discoverable attributes
    SpheGeometry* _geometry;
    double _q;
};

////////////////////////////////////////////////////////////////////

#endif // SPHEROIDALGEOMETRYDECORATOR_HPP
