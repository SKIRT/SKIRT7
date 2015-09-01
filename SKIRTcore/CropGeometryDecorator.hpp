/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef CROPGEOMETRYDECORATOR_HPP
#define CROPGEOMETRYDECORATOR_HPP

#include "GenGeometry.hpp"

////////////////////////////////////////////////////////////////////

/** The CropGeometryDecorator class is a decorator that crop any geometry. It sets the
    density equal to zero outside a given cuboidal bounding box, and increases the density
    within this bounding box with a constant factor to ensure that the total mass remains equal
    to one. */
class CropGeometryDecorator : public GenGeometry
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a decorator that crops any geometry")

    Q_CLASSINFO("Property", "geometry")
    Q_CLASSINFO("Title", "the geometry to be cropped")

    Q_CLASSINFO("Property", "extentX")
    Q_CLASSINFO("Title", "the outer radius of the bounding box in the x direction")
    Q_CLASSINFO("Quantity", "length")
    Q_CLASSINFO("MinValue", "0")

    Q_CLASSINFO("Property", "extentY")
    Q_CLASSINFO("Title", "the outer radius of the bounding box in the y direction")
    Q_CLASSINFO("Quantity", "length")
    Q_CLASSINFO("MinValue", "0")

    Q_CLASSINFO("Property", "extentZ")
    Q_CLASSINFO("Title", "the outer radius of the bounding box in the z direction")
    Q_CLASSINFO("Quantity", "length")
    Q_CLASSINFO("MinValue", "0")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE CropGeometryDecorator();

protected:
    /** This function verifies the property values. */
    void setupSelfBefore();

    /** This function estimates the fraction \f$\chi\f$ of the mass from the original model taken
        away by the cropping. It samples the density of the geometry being decorated, and counts
        the number of generated positions that fall outside the bounding box. This value is used to
        renormalize the decorated density distribution to unity: the factor by which the original
        density has to be multiplied is simply \f$1/(1-\chi)\f$. */
    void setupSelfAfter();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the geometry to be cropped (i.e. the geometry being decorated). */
    Q_INVOKABLE void setGeometry(Geometry* value);

    /** Returns the geometry to be cropped (i.e. the geometry being decorated). */
    Q_INVOKABLE Geometry* geometry() const;

    /** Sets the maximum extent of the bounding box in the X direction. */
    Q_INVOKABLE void setExtentX(double value);

    /** Returns the maximum extent of the bounding box in the X direction. */
    Q_INVOKABLE double extentX() const;

    /** Sets the maximum extent of the bounding box in the Y direction. */
    Q_INVOKABLE void setExtentY(double value);

    /** Returns the maximum extent of the bounding box in the Y direction. */
    Q_INVOKABLE double extentY() const;

    /** Sets the maximum extent of the bounding box in the Z direction. */
    Q_INVOKABLE void setExtentZ(double value);

    /** Returns the maximum extent of the bounding box in the Z direction. */
    Q_INVOKABLE double extentZ() const;

    //======================== Other Functions =======================

public:
    /** This function returns the density \f$\rho({\bf{r}})\f$ at the position \f${\bf{r}}\f$. It
        is zero outside the bounding box, and equal to the density of the geometry being
        decorated elsewhere, after an adjustment is made to account for the cropping. */
    double density(Position bfr) const;

    /** This function generates a random position from the geometry, by drawing a random point from
        the three-dimensional probability density \f$p({\bf{r}})\, {\text{d}}{\bf{r}} =
        \rho({\bf{r}})\, {\text{d}}{\bf{r}}\f$. It repeatedly calls the density() function for the
        geometry being decorated until a position is returned that lies inside the cropped box. */
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
    double _xmax, _ymax, _zmax;

    // values initialized during setup
    double _norm;
};

////////////////////////////////////////////////////////////////////

#endif // CROPGEOMETRYDECORATOR_HPP
