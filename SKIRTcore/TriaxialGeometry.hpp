/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef TRIAXIALGEOMETRY_HPP
#define TRIAXIALGEOMETRY_HPP

#include "SpheGeometry.hpp"
#include "GenGeometry.hpp"

////////////////////////////////////////////////////////////////////

/** The TriaxialGeometry class is a Geometry decorator that constructs a triaxial geometry
    based on a spherical geometry. The properties of an TriaxialGeometry object are a reference
    to the SpheGeometry object being decorated and the flattening parameters \f$p\f$ and \f$q\f$.
    If the original spherical geometry is characterized by the density profile \f$
    \rho_{\text{orig}}(r) \f$, the new geometry has as density \f[ \rho(x,y,z) = \frac{1}{p\,q}\,
    \rho_{\text{orig}}\left(\sqrt{x^2 + \frac{y^2}{p^2} + \frac{z^2}{q^2}}\right). \f] This new geometry
    is also normalized to one. */
class TriaxialGeometry : public GenGeometry
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a triaxial variant of any spherical geometry")

    Q_CLASSINFO("Property", "geometry")
    Q_CLASSINFO("Title", "the original spherical geometry")

    Q_CLASSINFO("Property", "yFlattening")
    Q_CLASSINFO("Title", "the flattening parameter p (along the y-axis)")
    Q_CLASSINFO("MinValue", "0")
    Q_CLASSINFO("MaxValue", "1")
    Q_CLASSINFO("Default", "1")

    Q_CLASSINFO("Property", "zFlattening")
    Q_CLASSINFO("Title", "the flattening parameter q (along the z-axis)")
    Q_CLASSINFO("MinValue", "0")
    Q_CLASSINFO("MaxValue", "1")
    Q_CLASSINFO("Default", "1")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE TriaxialGeometry();

    /** This function caches some values for use by subclasses. */
    void setupSelfBefore();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the original spherical geometry (i.e. the geometry being decorated). */
    Q_INVOKABLE void setGeometry(SpheGeometry* value);

    /** Returns the original spherical geometry. */
    Q_INVOKABLE SpheGeometry* geometry() const;

    /** Sets the flattening parameter \f$p\f$. */
    Q_INVOKABLE void setYFlattening(double value);

    /** Returns the flattening parameter \f$p\f$. */
    Q_INVOKABLE double yFlattening() const;

    /** Sets the flattening parameter \f$q\f$. */
    Q_INVOKABLE void setZFlattening(double value);

    /** Returns the flattening parameter \f$q\f$. */
    Q_INVOKABLE double zFlattening() const;

    //======================== Other Functions =======================

public:
    /** This function returns the density \f$\rho({\bf{r}})\f$ at the position \f${\bf{r}}\f$. It
        just implements the analytical formula. */
    double density(Position bfr) const;

    Position generatePosition() const;

    /** This function returns the X-axis surface density, i.e. the integration of the density along
        the entire X-axis, \f[ \Sigma_X = \int_{-\infty}^\infty \rho(x,0,0)\,{\text{d}}x. \f] We
        easily obtain \f[ \Sigma_X = \frac{2}{p\,q} \int_{-\infty}^\infty
        \rho_{\text{orig}}(x)\,{\text{d}}x = \frac{2}{p\,q}\,\Sigma_{r,{\text{orig}}}. \f] */
    double SigmaX() const;

    /** This function returns the Y-axis surface density, i.e. the integration of the density along
        the entire Y-axis, \f[ \Sigma_Y = \int_{-\infty}^\infty \rho(0,y,0)\,{\text{d}}y. \f] We
        easily obtain \f[ \Sigma_Y = \frac{2}{p\,q} \int_{-\infty}^\infty \rho_{\text{orig}}
        \left(\frac{y}{p}\right)\,{\text{d}}y = \frac{2}{q}\,\Sigma_{r,{\text{orig}}}. \f] */
    double SigmaY() const;

    /** This function returns the Z-axis surface density, i.e. the integration of the density along
        the entire Z-axis, \f[ \Sigma_Z = \int_{-\infty}^\infty \rho(0,0,z)\,{\text{d}}z. \f] We
        easily obtain \f[ \Sigma_Z = \frac{2}{p\,q} \int_{-\infty}^\infty \rho_{\text{orig}}
        \left(\frac{z}{q}\right)\,{\text{d}}z = \frac{2}{p}\,\Sigma_{r,{\text{orig}}}. \f] */
    double SigmaZ() const;

    //======================== Data Members ========================

private:
    // data members for discoverable attributes
    SpheGeometry* _geometry;
    double _p;
    double _q;
};

////////////////////////////////////////////////////////////////////

#endif // TRIAXIALGEOMETRY_HPP
