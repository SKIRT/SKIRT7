/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef FOAMGEOMETRY_HPP
#define FOAMGEOMETRY_HPP

#include "GenGeometry.hpp"
#include "FoamDensity.hpp"
class Foam;

////////////////////////////////////////////////////////////////////

/** The FoamGeometry class is an abstract subclass of the GenGeometry class, and
    allows to construct stellar geometries characterized by an arbitrary three-dimensional density
    distribution \f$\rho({\bf{r}})\f$. For the generation of random positions from the density
    \f$\rho({\bf{r}})\f$, the FoamGeometry class uses a three-dimensional cell structure,
    called the foam. A foam is based on the three-dimensional unit cube \f$[0,1]^3\f$, subdivided
    into a large number of small cuboidal cells. The distribution of the grid cells is performed
    completely automatically from any density distribution. The foam implementation, characterized
    by the Foam class, allows to efficiently generate random numbers drawn from this probability
    distribution on this unit cube. One problem is that the stellar density \f$\rho({\bf{r}})\f$ is
    typically defined on the entire 3D space, whereas the foam requires a density distribution on
    the unit cube. We solve this problem with a coordinate transformation from the usual
    coordinates \f$(x,y,z)\f$ to new coordinates \f$(\bar{x}, \bar{y}, \bar{z})\f$ such that we map
    each infinite interval \f$[-\infty,\infty]\f$ onto the unit interval \f$[0,1]\f$. The stellar
    density \f$\rho(x,y,z)\f$, normalized such that \f[ \int_{-\infty}^\infty {\text{d}}x
    \int_{-\infty}^\infty {\text{d}}y \int_{-\infty}^\infty \rho(x,y,z)\, {\text{d}}z = 1, \f] is
    transformed to a new density \f[ \bar\rho(\bar{x},\bar{y},\bar{z}) = \rho\Bigl( x(\bar{x}),
    y(\bar{y}), z(\bar{z}) \Bigr)\, \frac{{\text{d}}x}{{\text{d}}\bar{x}}\,
    \frac{{\text{d}}y}{{\text{d}}\bar{y}}\, \frac{{\text{d}}z}{{\text{d}}\bar{z}}. \f] It is
    obvious that this new density will now be normalized on the unit cube, \f[ \int_0^1
    {\text{d}}\bar{x} \int_0^1 {\text{d}}\bar{y} \int_0^1 \bar\rho(\bar{x},\bar{y},\bar{z})\,
    {\text{d}}\bar{z} = 1. \f] There are obviously many different transformations we can apply. The
    one applied for the FoamGeometry class is characterized by \f[ \begin{split} x &=
    \frac{a}{\tan(\bar{x}\pi)}, \\ y &= \frac{b}{\tan(\bar{y}\pi)}, \\ z &=
    \frac{c}{\tan(\bar{z}\pi)}, \end{split} \f] with \f$a\f$, \f$b\f$ and \f$c\f$ three scale
    parameters. The inverse transformation is \f[ \begin{split} \bar{x} &= \frac{1}{\pi}
    \arctan\left(\frac{a}{x}\right), \\ \bar{y} &= \frac{1}{\pi} \arctan\left(\frac{b}{y}\right),
    \\ \bar{z} &= \frac{1}{\pi} \arctan\left(\frac{c}{z}\right). \end{split} \f] With this
    transformation, the Jacobian reads \f[ \left|
    \frac{\partial(x,y,z)}{\partial(\bar{x},\bar{y},\bar{z})} \right| = \left|
    \frac{{\text{d}}x}{{\text{d}}\bar{x}}\, \frac{{\text{d}}y}{{\text{d}}\bar{y}}\,
    \frac{{\text{d}}z}{{\text{d}}\bar{z}} \right| = \frac{abc\,\pi^3}{\sin^2(\bar{x}\pi)
    \sin^2(\bar{y}\pi) \sin^2(\bar{z}\pi)}. \f] */
class FoamGeometry : public GenGeometry, FoamDensity
{
    Q_OBJECT
    Q_CLASSINFO("Title", "an arbitrary stellar geometry with non-analytic density distribution")

    //============= Construction - Setup - Destruction =============

protected:
    /** The constructor; it is protected since this is an abstract class. The subclass must provide
        the number of foam cells appropriate for its particular geometry. */
    FoamGeometry(int Ncells);

    /** The destructor; it deallocates the foam object created during setup. */
    ~FoamGeometry();

    /** Given a density distribution \f$\rho({\bf{r}})\f$, this function sets up the foam for the
        density distribution \f$\bar\rho(\bar{\bf{r}})\f$. In the standard setup, only cubical
        cells are used; the foam also offers the possility to create simplical cells (or a
        combination of both), but this is more memory intensive. The number of cells to be used is
        a free parameter. The density distribution \f$\bar\rho(\bar{\bf{r}})\f$ on the unit cube
        that is used to determine the distribution of the grid cells is characterized by an object
        of the technical FoamGeometryIntegrand class. As a user, you don't need to worry
        about this, as this object is created automatically from the desired density
        \f$\rho({\bf{r}})\f$. */
    void setupSelfAfter();

    //======================== Other Functions =======================

public:
    /** This function generates a random position from the stellar geometry, by
        drawing a random point from the three-dimensional probability density \f$p({\bf{r}})\,
        {\text{d}}{\bf{r}} = \rho({\bf{r}})\, {\text{d}}{\bf{r}}\f$. For a geometry of the
        FoamGeometry, this task is accomplished by drawing a random point
        \f$(\bar{\bf{r}})\f$ from the foam and converting this to a position \f${\bf{r}}\f$ using
        the appropriate transformation. */
    Position generatePosition() const;

    /** This virtual function return the scale parameter \f$a\f$ for the transformation between the
        normal coordinate \f$x\f$ and the corresponding normalized coordinate \f$\bar{x}\f$ on the
        unit interval. */
    virtual double xscale() const = 0;

    /** This virtual function return the scale parameter \f$b\f$ for the transformation between the
        normal coordinate \f$y\f$ and the corresponding normalized coordinate \f$\bar{y}\f$ on the
        unit interval. */
    virtual double yscale() const = 0;

    /** This virtual function return the scale parameter \f$c\f$ for the transformation between the
        normal coordinate \f$z\f$ and the corresponding normalized coordinate \f$\bar{z}\f$ on the
        unit interval. */
    virtual double zscale() const = 0;

    /** This function implements the FoamDensity interface to convert the stellar density
        distribution \f$\rho({\bf{r}})\f$ to a new probability density distribution
        \f$\bar\rho(\bar{\bf{r}})\f$ on the three-dimensional unit cube \f$[0,1]^3\f$ such that it
        can be used as input for the construction of a foam. The function returns the density
        \f$\bar\rho(\bar{\bf{r}})\f$ in a point \f$\bar{\bf{r}}\f$ in the unit cube, defined by the
        input parameters (\f$n_{\text{par}}\f$ is always equal to 3 and the pointer contains the
        three cartesian parameters \f$(\bar{x},\bar{y},\bar{z})\f$ that define the position
        \f$\bar{\bf{r}}\f$). The required density \f$\bar\rho(\bar{\bf{r}})\f$ is determined as \f[
        \bar\rho(\bar{x},\bar{y},\bar{z}) = \rho\Bigl( x(\bar{x}), y(\bar{y}), z(\bar{z}) \Bigr)\,
        \frac{{\text{d}}x}{{\text{d}}\bar{x}}\, \frac{{\text{d}}y}{{\text{d}}\bar{y}}\,
        \frac{{\text{d}}z}{{\text{d}}\bar{z}}, \f] with \f$\rho(x,y,z)\f$ the stellar density of
        the stellar geometry that implicitly defines the FoamGeometryIntegrand object. The
        transformation between the coordinates \f$(x,y,z)\f$ and \f$(\bar{x},\bar{y},\bar{z})\f$ is
        \f[ \begin{split} x &= \frac{a}{\tan(\bar{x}\pi)}, \\ y &= \frac{b}{\tan(\bar{y}\pi)}, \\ z
        &= \frac{c}{\tan(\bar{z}\pi)}, \end{split} \f] with \f$a\f$, \f$b\f$ and \f$c\f$ the three
        scale parameters defined in the stellar geometry. With this transformation, the Jacobian
        reads \f[ \left| \frac{\partial(x,y,z)}{\partial(\bar{x},\bar{y},\bar{z})} \right| = \left|
        \frac{{\text{d}}x}{{\text{d}}\bar{x}}\, \frac{{\text{d}}y}{{\text{d}}\bar{y}}\,
        \frac{{\text{d}}z}{{\text{d}}\bar{z}} \right| = \frac{abc\,\pi^3}{\sin^2(\bar{x}\pi)
        \sin^2(\bar{y}\pi) \sin^2(\bar{z}\pi)}. \f] */
    double foamdensity(int ndim, double* par) const;

    //======================== Data Members ========================

private:
    // data member initialized by constructor
    int _Ncells;

    // data member initialized during setup
    Foam* _foam;

    // data members that need to be initialized by the subclasses
    double _xscale;
    double _yscale;
    double _zscale;
};

////////////////////////////////////////////////////////////////////

#endif // FOAMGEOMETRY_HPP
