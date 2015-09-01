/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef CYLINDRICALCAVITYGEOMETRYDECORATOR_HPP
#define CYLINDRICALCAVITYGEOMETRYDECORATOR_HPP

#include "Geometry.hpp"

////////////////////////////////////////////////////////////////////

/** The CylindricalCavityGeometryDecorator class is a decorator that forces the density of any
    geometry to zero in a cylindrical volume. The cylinder is infinitely long, centered at the origin
    and oriented perpendicular to the XY-plane. The radius of the cylinder can be chosen. The properties
    of a CylindricalCavityGeometryDecorator object are a reference to the Geometry object being
    decorated, and the radius of the cylinder. The current implementation does not properly adjust the
    surface densities along the coordinate axes for the mass taken away by the cavity. */
class CylindricalCavityGeometryDecorator : public Geometry
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a decorator that forces a cylindrical cavity in any geometry")

    Q_CLASSINFO("Property", "geometry")
    Q_CLASSINFO("Title", "the geometry to be decorated which a cylindrical cavity")

    Q_CLASSINFO("Property", "radius")
    Q_CLASSINFO("Title", "the radius of the cylindrical cavity")
    Q_CLASSINFO("Quantity", "length")
    Q_CLASSINFO("MinValue", "0")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE CylindricalCavityGeometryDecorator();

protected:
    /** This function estimates the fraction of the mass taken away by the cylindrical cavity by
        sampling the density of the geometry being decorated. This value is used to renormalize the
        decorated density distribution to unity. The mass in the cylinder is estimated by means of
        an iterative Monte Carlo integration. */
    void setupSelfAfter();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the geometry to be adjusted (i.e. the geometry being decorated). */
    Q_INVOKABLE void setGeometry(Geometry* value);

    /** Returns the geometry to be adjusted (i.e. the geometry being decorated). */
    Q_INVOKABLE Geometry* geometry() const;

    /** Sets the radius of the cylinder. */
    Q_INVOKABLE void setRadius(double value);

    /** Returns the radius of the cylinder. */
    Q_INVOKABLE double radius() const;

    //======================== Other Functions =======================

public:
    /** This function returns the dimension of the geometry. If the original geometry has a dimension 3,
        so will the new geometry. Otherwise, i.e. if the original geometry is spherically or axisymmetric,
        the dimension is 2. */
    int dimension() const;

    /** This function returns the density \f$\rho({\bf{r}})\f$ at the position \f${\bf{r}}\f$. It
        is zero within the volume of the cylinder, and equal to the density of the geometry being
        decorated elsewhere (after renormalisation). */
    double density(Position bfr) const;

    /** This function generates a random position from the geometry, by drawing a random point from
        the three-dimensional probability density \f$p({\bf{r}})\, {\text{d}}{\bf{r}} =
        \rho({\bf{r}})\, {\text{d}}{\bf{r}}\f$. It repeatedly calls the density() function for the
        geometry being decorated until a position is returned that lies outside the cylinder. */
    Position generatePosition() const;

    /** This function returns the X-axis surface density, i.e. the integration of the density along
        the entire X-axis, \f[ \Sigma_X = \int_{-\infty}^\infty \rho(x,0,0)\,{\text{d}}x. \f] It
        simply calls the corresponding function of the geometry being decorated and applies the appropriate
        normalisation. */
    double SigmaX() const;

    /** This function returns the Y-axis surface density, i.e. the integration of the density along
        the entire Y-axis, \f[ \Sigma_Y = \int_{-\infty}^\infty \rho(0,y,0)\,{\text{d}}y. \f] It
        simply calls the corresponding function of the geometry being decorated and applies the appropriate
        normalisation.  */
    double SigmaY() const;

    /** This function returns the Z-axis surface density, i.e. the integration of the density along
        the entire Z-axis, \f[ \Sigma_Z = \int_{-\infty}^\infty \rho(0,0,z)\,{\text{d}}z. \f] It
        return zero in this case. */
    double SigmaZ() const;

    /** This function implements part of the AngularDistribution interface. It returns the
        probability \f$P(\Omega)\f$ for a given direction \f$(\theta,\phi)\f$ at the specified
        position. For the cavity decorator, this function simply calls the corresponding function
        for the geometry being decorated. */
    double probabilityForDirection(int ell, Position bfr, Direction bfk) const;

    /** This function implements part of the AngularDistribution interface. It generates a random
        direction \f$(\theta,\phi)\f$ drawn from the probability distribution \f$P(\Omega)
        \,{\mathrm{d}}\Omega\f$ at the specified position. For the cavity decorator, this function
        simply calls the corresponding function for the geometry being decorated. */
    Direction generateDirection(int ell, Position bfr) const;

    //======================== Data Members ========================

private:
    // data members for discoverable attributes
    Geometry* _geometry;
    double _radius;

    // other data members
    double _norm;

};

////////////////////////////////////////////////////////////////////

#endif // CYLINDRICALCAVITYGEOMETRYDECORATOR_HPP
