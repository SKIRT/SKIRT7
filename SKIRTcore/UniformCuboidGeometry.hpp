/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef UNIFORMCUBOIDGEOMETRY_HPP
#define UNIFORMCUBOIDGEOMETRY_HPP

#include "Box.hpp"
#include "GenGeometry.hpp"

////////////////////////////////////////////////////////////////////

/** The UniformCuboidGeometry class describes a 3D geometry consisting of a uniform and isotropic
    cuboid centered on the origin. */
class UniformCuboidGeometry : public GenGeometry
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a uniform cuboid geometry")

    Q_CLASSINFO("Property", "extentX")
    Q_CLASSINFO("Title", "the half-width of the cuboid in the x direction")
    Q_CLASSINFO("Quantity", "length")
    Q_CLASSINFO("MinValue", "0")

    Q_CLASSINFO("Property", "extentY")
    Q_CLASSINFO("Title", "the half-width of the cuboid in the y direction")
    Q_CLASSINFO("Quantity", "length")
    Q_CLASSINFO("MinValue", "0")

    Q_CLASSINFO("Property", "extentZ")
    Q_CLASSINFO("Title", "the half-width of the cuboid in the z direction")
    Q_CLASSINFO("Quantity", "length")
    Q_CLASSINFO("MinValue", "0")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE UniformCuboidGeometry();

protected:
    /** This function verifies the property values and calculates the (constant) density at each
        point, assuming a total mass of 1. */
    void setupSelfBefore();

    //======== Setters & Getters for Discoverable Attributes =======

public:
   /** Sets the half-width \f$x_\text{max}\f$ of the cuboid in the X direction. The total size of
       the cuboid in this direction is twice as large. */
    Q_INVOKABLE void setExtentX(double value);

    /** Returns the half-width \f$x_\text{max}\f$ of the cuboid in the X direction. */
    Q_INVOKABLE double extentX() const;

    /** Sets the half-width \f$y_\text{max}\f$ of the cuboid in the Y direction. The total size of
        the cuboid in this direction is twice as large. */
    Q_INVOKABLE void setExtentY(double value);

    /** Returns the half-width \f$y_\text{max}\f$ of the cuboid in the Y direction. */
    Q_INVOKABLE double extentY() const;

    /** Sets the half-width \f$z_\text{max}\f$ of the cuboid in the Z direction. The total size of
        the cuboid in this direction is twice as large. */
    Q_INVOKABLE void setExtentZ(double value);

    /** Returns the half-width \f$z_\text{max}\f$ of the cuboid in the Z direction. */
    Q_INVOKABLE double extentZ() const;

    //======================== Other Functions =======================

public:
    /** This function returns the density \f$\rho({\bf{r}})\f$ for this geometry at the position
        \f${\bf{r}}\f$. It returns the constant value
        \f$\rho=1/(8\,x_\text{max}\,y_\text{max}\,z_\text{max})\f$ for all points inside the
        cuboid, and zero outside the cuboid. */
    double density(Position bfr) const;

    /** This function generates a random position from the geometry, by drawing a uniform random
        point inside the cuboid. */
    Position generatePosition() const;

    /** This function returns the X-axis surface density of the geometry, defined as the
        integration of the density along the entire X-axis, \f[ \Sigma_X = \int_{-\infty}^\infty
        \rho(x,0,0)\, {\text{d}}x = \int_{-x_\text{max}}^{x_\text{max}}
        \frac{1}{8\,x_\text{max}\,y_\text{max}\,z_\text{max}}\, {\text{d}}x =
        \frac{1}{4\,y_\text{max}\,z_\text{max}}.\f] */
    double SigmaX() const;

    /** This function returns the Y-axis surface density of the geometry, defined as the
        integration of the density along the entire Y-axis, \f[ \Sigma_Y = \int_{-\infty}^\infty
        \rho(0,y,0)\, {\text{d}}y = \int_{-y_\text{max}}^{y_\text{max}}
        \frac{1}{8\,x_\text{max}\,y_\text{max}\,z_\text{max}}\, {\text{d}}x =
        \frac{1}{4\,x_\text{max}\,z_\text{max}}.\f] */
    double SigmaY() const;

    /** This function returns the Z-axis surface density of the geometry, defined as the
        integration of the density along the entire Z-axis, \f[ \Sigma_Z = \int_{-\infty}^\infty
        \rho(0,0,z)\, {\text{d}}z = \int_{-z_\text{max}}^{z_\text{max}}
        \frac{1}{8\,x_\text{max}\,y_\text{max}\,z_\text{max}}\, {\text{d}}x =
        \frac{1}{4\,x_\text{max}\,y_\text{max}}.\f] */
    double SigmaZ() const;

    //======================== Data Members ========================

private:
    // discoverable attributes
    double _xmax;
    double _ymax;
    double _zmax;

    // other data members
    Box _extent;
    double _rho;
};

////////////////////////////////////////////////////////////////////

#endif // UNIFORMCUBOIDGEOMETRY_HPP
