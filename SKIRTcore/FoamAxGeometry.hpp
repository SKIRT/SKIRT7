/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef FOAMAXGEOMETRY_HPP
#define FOAMAXGEOMETRY_HPP

#include "AxGeometry.hpp"
#include "FoamDensity.hpp"
class Foam;

////////////////////////////////////////////////////////////////////

/** The FoamAxGeometry class is an abstract subclass of the AxGeometry class, and
    allows to construct stellar geometries characterized by an arbitrary axially symmetric density
    distribution \f$\rho({\bf{r}}) = \rho(R,z)\f$. For the generation of random positions from the
    density \f$\rho({\bf{r}})\f$, the FoamAxGeometry class uses a two-dimensional cell
    structure, called the foam. A foam is based on the two-dimensional unit square \f$[0,1]^2\f$,
    divided into many small rectangular cells. The distribution of the grid cells is performed
    completely automatically from any density distribution on the foam. The foam implementation,
    characterized by the Foam class, allows to efficiently generate random numbers drawn from this
    probability distribution on this unit square. One problem is that the stellar density
    \f$\rho(R,z)\f$ is defined on the infinite space \f$[0,\infty]\times[-\infty,\infty]\f$,
    whereas the foam requires a density distribution on the unit square. We solve this problem with
    a coordinate transformation from the usual cylindrical coordinates \f$(R,z)\f$ to new
    coordinates \f$(\bar{R}, \bar{z})\f$. The stellar density \f$\rho(R,z)\f$, normalized such that
    \f[ 2\pi \int_{-\infty}^\infty {\text{d}}z \int_0^\infty \rho(R,z)\, R\, {\text{d}}R = 1, \f]
    is transformed to a new density \f[ \bar\rho(\bar{R},\bar{z}) = 2\pi \rho\Bigl( R(\bar{R}),
    z(\bar{z}) \Bigr)\, R(\bar{R})\, \frac{{\text{d}}R}{{\text{d}}\bar{R}}\,
    \frac{{\text{d}}z}{{\text{d}}\bar{z}}. \f] It is obvious that this new density will now be
    normalized on the unit square, \f[ \int_0^1 {\text{d}}\bar{R} \int_0^1
    \bar\rho(\bar{R},\bar{z})\, {\text{d}}\bar{z} = 1. \f] There are obviously many different
    transformations we can apply. The one applied for the FoamAxGeometry class is
    characterized by \f[ \begin{split} R &= -a\ln\bar{R}, \\ z &= \frac{c}{\tan(\bar{z}\pi)},
    \end{split} \f] with \f$a\f$ and \f$c\f$ two scale parameters. The inverse transformation is
    \f[ \begin{split} \bar{R} &= \exp\left(-\frac{R}{a}\right), \\ \bar{z} &= \frac{1}{\pi}
    \arctan\left(\frac{c}{z}\right). \end{split} \f] With this transformation, the Jacobian reads
    \f[ \frac{{\text{d}}R}{{\text{d}}\bar{R}}\, \frac{{\text{d}}z}{{\text{d}}\bar{z}} =
    \frac{ac\,\pi}{\bar{R}\sin^2(\bar{z}\pi)}. \f] */
class FoamAxGeometry : public AxGeometry, FoamDensity
{
    Q_OBJECT
    Q_CLASSINFO("Title", "an axisymmetric stellar geometry with non-analytic density distribution")

    //============= Construction - Setup - Destruction =============

protected:
    /** The constructor; it is protected since this is an abstract class. The subclass must
        provide the number of foam cells appropriate for its particular geometry. */
    FoamAxGeometry(int Ncells);

    /** The destructor; it deallocates the foam object created during setup. */
    ~FoamAxGeometry();

    /** Given a density distribution \f$\rho({\bf{r}})\f$, this function sets up the foam for the
        density distribution \f$\bar\rho(\bar{\bf{r}})\f$. In the standard setup, only cubical
        cells are used; the foam also offers the possility to create simplical cells (or a
        combination of both), but this is more memory intensive. The number of cells to be used is
        a free parameter in the model. The density distribution \f$\bar\rho(\bar{\bf{r}})\f$ on the
        unit cube that is used to determine the distribution of the grid cells is characterized by
        an object of the technical FoamAxGeometryIntegrand class. As a user, you don't need
        to worry about this, as this object is created automatically from the desired density
        \f$\rho({\bf{r}})\f$. */
    void setupSelfAfter();

    //======================== Other Functions =======================

public:
    /** This function generates a random position from the stellar geometry, by
        drawing a random point from the two-dimensional probability density \f[ p(R,z)\,
        {\text{d}}R\, {\text{d}}z = 2\pi\, \rho(R,z)\, R\, {\text{d}}R\, {\text{d}}z. \f] For a
        geometry of the FoamAxGeometry, this task is accomplished by drawing a random point
        \f$(\bar{\bf{r}})=(\bar{R},\bar{z})\f$ from the foam and converting this to a position
        \f${\bf{r}}\f$ using the appropriate transformation and a random azimuth angle. */
    Position generatePosition() const;

    /** This function implements the FoamDensity interface to convert the stellar density
        distribution \f$\rho(R,z)\f$ to a new probability density distribution
        \f$\bar\rho(\bar{R},\bar{z})\f$ on the two-dimensional unit square \f$[0,1]^2\f$ such that
        it can be used as input for the construction of a foam. The function returns the density
        \f$\bar\rho(\bar{R},\bar{z})\f$ in a point \f$(\bar{R},\bar{z})\f$ in the unit square,
        defined by the input parameters (\f$n_{\text{par}}\f$ is always equal to 2 and the pointer
        contains the two coordinates \f$(\bar{R},\bar{z})\f$). The required density
        \f$\bar\rho(\bar{R},\bar{z})\f$ is determined as \f[ \bar\rho(\bar{R},\bar{z}) = 2\pi
        \rho\Bigl( R(\bar{R}), z(\bar{z}) \Bigr)\, R(\bar{R})\,
        \frac{{\text{d}}R}{{\text{d}}\bar{R}}\, \frac{{\text{d}}z}{{\text{d}}\bar{z}} \f] with
        \f$\rho(R,z)\f$ the stellar density of the stellar geometry that implicitly defines the
        FoamAxGeometryIntegrand object. The transformation between the coordinates
        \f$(R,z)\f$ and \f$(\bar{R},\bar{z})\f$ is \f[ \begin{split} R &= -a\ln\bar{R}, \\ z &=
        \frac{c}{\tan(\bar{z}\pi)}, \end{split} \f] with \f$a\f$ and \f$c\f$ the two scale
        parameters defined in the stellar geometry. With this transformation, the Jacobian reads
        \f[ \frac{{\text{d}}R}{{\text{d}}\bar{R}}\, \frac{{\text{d}}z}{{\text{d}}\bar{z}} =
        \frac{ac\,\pi}{\bar{R}\sin^2(\bar{z}\pi)}. \f] */
    double foamdensity(int ndim, double* par) const;

    //======================== Data Members ========================

private:
    // data member initialized by constructor
    int _Ncells;

    // data member initialized during setup
    Foam* _foam;

protected:
    // data members that need to be initialized by the subclasses
    double _Rscale;
    double _zscale;
};

////////////////////////////////////////////////////////////////////

#endif // FOAMAXGEOMETRY_HPP
