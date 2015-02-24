/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef FOAMGEOMETRYDECORATOR_HPP
#define FOAMGEOMETRYDECORATOR_HPP

#include "GenGeometry.hpp"
#include "FoamDensity.hpp"
class Foam;

////////////////////////////////////////////////////////////////////

/** The FoamGeometryDecorator class is a decorator for the Geometry class that
    provides an alternative method to generate random positions. The FoamDecoGeometry class
    uses a three-dimensional cell structure, called the foam. A foam is based on the
    three-dimensional unit cube \f$[0,1]^3\f$, subdivided into a large number of small
    cuboidal cells. The distribution of the grid cells is performed completely
    automatically, based on the density distribution of the geometry that is being
    decorated. The foam implementation, characterized by the Foam class, allows to
    efficiently generate random numbers drawn from this probability distribution on this
    unit cube. One problem is that the stellar density \f$\rho({\bf{r}})\f$ is typically
    defined on the entire 3D space, whereas the foam requires a density distribution on
    the unit cube. We solve this problem using a simple linear transformation, where we
    map the volume from which we sample (assumed to be a box) to the unit cube. */
class FoamGeometryDecorator : public GenGeometry, FoamDensity
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a decorator that provides an alternative random position generator")

    Q_CLASSINFO("Property", "geometry")
    Q_CLASSINFO("Title", "the geometry to be alternatively sampled")

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

    Q_CLASSINFO("Property", "numCells")
    Q_CLASSINFO("Title", "the number of cells in the foam")
    Q_CLASSINFO("MinValue", "1000")
    Q_CLASSINFO("MaxValue", "1000000")
    Q_CLASSINFO("Default", "10000")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE FoamGeometryDecorator();

    /** The destructor; it deallocates the foam object created during setup. */
    ~FoamGeometryDecorator();

protected:
    /** This function sets up the foam. The Foam constructor needs a pointer to an instance of the
        FoamDensity interface, which is implemented for this purpose by the FoamGeometryDecorator
        class. */
    void setupSelfAfter();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the geometry to be decorated. */
    Q_INVOKABLE void setGeometry(Geometry* value);

    /** Returns the geometry to be decorated. */
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

    /** Sets the number of cells in the foam. */
    Q_INVOKABLE void setNumCells(int value);

    /** Returns the number of cells in the foam. */
    Q_INVOKABLE int numCells() const;

    //======================== Other Functions =======================

public:

    /** This function returns the density \f$\rho({\bf{r}})\f$ at the position
        \f${\bf{r}}\f$. It simply calls the density() function for the geometry being
        decorated. */
    double density(Position bfr) const;

    /** This function generates a random position from the geometry, by drawing a random
        point from the three-dimensional probability density \f$p({\bf{r}})\, {\text{d}}
        {\bf{r}} = \rho({\bf{r}})\, {\text{d}}{\bf{r}}\f$. This task is accomplished by
        drawing a random point \f$(\bar{\bf{r}})\f$ from the foam and converting this to
        a position \f${\bf{r}}\f$ using the appropriate transformation. */
    Position generatePosition() const;

    /** This function returns the X-axis surface density, i.e. the integration of the
        density along the entire X-axis, \f[ \Sigma_X = \int_{-\infty}^\infty \rho(x,0,0)
        \,{\text{d}}x. \f] It just calls the corresponding function for the geometry being
        decorated. */
    double SigmaX() const;

    /** This function returns the Y-axis surface density, i.e. the integration of the
        density along the entire Y-axis, \f[ \Sigma_Y = \int_{-\infty}^\infty \rho(0,y,0)
        \,{\text{d}}y. \f] It just calls the corresponding function for the geometry being
        decorated. */
    double SigmaY() const;

    /** This function returns the Z-axis surface density, i.e. the integration of the
        density along the entire Z-axis, \f[ \Sigma_Z = \int_{-\infty}^\infty \rho(0,0,z)
        \,{\text{d}}z. \f] It just calls the corresponding function for the geometry being
        decorated. */
    double SigmaZ() const;

    /** This function implements the FoamDensity interface to convert the stellar density
        distribution \f$\rho({\bf{r}})\f$ to a new probability density distribution
        \f$\bar\rho(\bar{\bf{r}})\f$ on the three-dimensional unit cube \f$[0,1]^3\f$ such
        that it can be used as input for the construction of a foam. The function returns the
        density \f$\bar\rho(\bar{\bf{r}})\f$ in a point \f$\bar{\bf{r}}\f$ in the unit cube,
        defined by the input parameters (\f$n_{\text{par}}\f$ is always equal to 3 and the
        pointer contains the three cartesian parameters \f$(\bar{x},\bar{y},\bar{z})\f$ that
        define the position \f$\bar{\bf{r}}\f$). The required density \f$\bar\rho(
        \bar{\bf{r}})\f$ is determined as \f[ \bar\rho(\bar{x},\bar{y},\bar{z}) = \rho
        \Bigl( x(\bar{x}), y(\bar{y}), z(\bar{z}) \Bigr)\, \frac{{\text{d}}x}{{\text{d}}
        \bar{x}}\, \frac{{\text{d}}y}{{\text{d}}\bar{y}}\, \frac{{\text{d}}z}{{\text{d}}
        \bar{z}}, \f] with \f$\rho(x,y,z)\f$ the stellar density. The transformation between
        the coordinates \f$(x,y,z)\f$ and \f$(\bar{x},\bar{y},\bar{z})\f$ is a simple linear
        transformation from the cuboid defined by \f$-x_{\text{max}} < x < x_{\text{max}}\f$,
        \f$-y_{\text{max}} < y < y_{\text{max}}\f$ and \f$-z_{\text{max}} < z <
        z_{\text{max}}\f$ to the unit cube. */
    double foamdensity(int ndim, double* par) const;

    //======================== Data Members ========================

private:
    // data members for which there are setters and getters
    Geometry* _geometry;
    double _xmax;
    double _ymax;
    double _zmax;
    int _Ncells;

    // data member initialized during setup
    Foam* _foam;
};

////////////////////////////////////////////////////////////////////

#endif // FOAMGEOMETRYDECORATOR_HPP
