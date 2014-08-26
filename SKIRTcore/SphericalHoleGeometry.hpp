/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef SPHERICALHOLEGEOMETRY_HPP
#define SPHERICALHOLEGEOMETRY_HPP

#include "Geometry.hpp"

////////////////////////////////////////////////////////////////////

/** The SphericalHoleGeometry class is a Geometry decorator that forces the density to zero in a
    spherical volume with given position and radius. The properties of a SphericalHoleGeometry
    object include (1) a reference to the Geometry object being decorated, (2) the radius of the
    hole, and (3) the position of the hole's center. The dimension of the geometry implemented by a
    SphericalHoleGeometry object depends on the symmetries of the geometry being decorated and on
    the position of the hole. The current implementation assumes that the hole is sufficiently
    small so that the effect on the total mass of the geometry is negligible. */
class SphericalHoleGeometry : public Geometry
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a geometry that forces a spherical hole in any geometry")

    Q_CLASSINFO("Property", "geometry")
    Q_CLASSINFO("Title", "the geometry to be adjusted")

    Q_CLASSINFO("Property", "radius")
    Q_CLASSINFO("Title", "the radius of the hole")
    Q_CLASSINFO("Quantity", "length")
    Q_CLASSINFO("MinValue", "0")

    Q_CLASSINFO("Property", "centerX")
    Q_CLASSINFO("Title", "the x coordinate of the hole's center")
    Q_CLASSINFO("Quantity", "length")
    Q_CLASSINFO("Default", "0")

    Q_CLASSINFO("Property", "centerY")
    Q_CLASSINFO("Title", "the y coordinate of the hole's center")
    Q_CLASSINFO("Quantity", "length")
    Q_CLASSINFO("Default", "0")

    Q_CLASSINFO("Property", "centerZ")
    Q_CLASSINFO("Title", "the z coordinate of the hole's center")
    Q_CLASSINFO("Quantity", "length")
    Q_CLASSINFO("Default", "0")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE SphericalHoleGeometry();

protected:
    /** This function calculates some frequently used values. */
    void setupSelfBefore();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the geometry to be adjusted (i.e. the geometry being decorated). */
    Q_INVOKABLE void setGeometry(Geometry* value);

    /** Returns the geometry to be adjusted (i.e. the geometry being decorated). */
    Q_INVOKABLE Geometry* geometry() const;

    /** Sets the radius of the hole. */
    Q_INVOKABLE void setRadius(double value);

    /** Returns the radius of the hole. */
    Q_INVOKABLE double radius() const;

    /** Sets the x coordinate of the hole's center. */
    Q_INVOKABLE void setCenterX(double value);

    /** Returns the x coordinate of the hole's center. */
    Q_INVOKABLE double centerX() const;

    /** Sets the y coordinate of the hole's center. */
    Q_INVOKABLE void setCenterY(double value);

    /** Returns the y coordinate of the hole's center. */
    Q_INVOKABLE double centerY() const;

    /** Sets the z coordinate of the hole's center. */
    Q_INVOKABLE void setCenterZ(double value);

    /** Returns the z coordinate of the hole's center. */
    Q_INVOKABLE double centerZ() const;

    //======================== Other Functions =======================

public:
    /** This function returns the dimension of the geometry, which is the larger of two dimensions:
        the dimension of the geometry being decorated and the dimension of the hole. The dimension
        of the hole is 1 if its center is at the origin, 2 if the center is on the Z-axis, and 3 if
        the center is elsewhere. */
    int dimension() const;

    /** This function returns the density \f$\rho({\bf{r}})\f$ at the position \f${\bf{r}}\f$. It
        is zero within the volume of the hole, and equal to the density of the geometry being
        decorated elsewhere. */
    double density(Position bfr) const;

    /** This function generates a random position from the geometry, by drawing a random point from
        the three-dimensional probability density \f$p({\bf{r}})\, {\text{d}}{\bf{r}} =
        \rho({\bf{r}})\, {\text{d}}{\bf{r}}\f$. It repeatedly calls the density() function for the
        geometry being decorated until a position is returned that lies outside the volume of the
        hole. */
    Position generatePosition() const;

    /** This function returns the X-axis surface density, i.e. the integration of the density along
        the entire X-axis, \f[ \Sigma_X = \int_{-\infty}^\infty \rho(x,0,0)\,{\text{d}}x. \f] It
        simply calls the corresponding function of the geometry being decorated. */
    double SigmaX() const;

    /** This function returns the Y-axis surface density, i.e. the integration of the density along
        the entire Y-axis, \f[ \Sigma_Y = \int_{-\infty}^\infty \rho(0,y,0)\,{\text{d}}y. \f] It
        simply calls the corresponding function of the geometry being decorated. */
    double SigmaY() const;

    /** This function returns the Z-axis surface density, i.e. the integration of the density along
        the entire Z-axis, \f[ \Sigma_Z = \int_{-\infty}^\infty \rho(0,0,z)\,{\text{d}}z. \f] It
        simply calls the corresponding function of the geometry being decorated. */
    double SigmaZ() const;

    //======================== Data Members ========================

private:
    // data members for discoverable attributes
    Geometry* _geometry;
    double _radius, _centerX, _centerY, _centerZ;

    // values initialized during setup
    Position _center;
    double _radius2;
};

////////////////////////////////////////////////////////////////////

#endif // SPHERICALHOLEGEOMETRY_HPP
