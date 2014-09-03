/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef TRUST1GEOMETRY_HPP
#define TRUST1GEOMETRY_HPP

#include "GenGeometry.hpp"
#include "Box.hpp"

////////////////////////////////////////////////////////////////////

/** The Trust1Geometry class is a subclass of the GenGeometry class and describes the geometry of a
    square constant density slab. It is to be used as one of the benchmark models for the suite of
    TRUST 3D benchmark models. The density is constant within the slab, and zero outside. The
    dimension of the slab are defined by \f$x_{\text{min}} = y_{\text{min}} = z_{\text{min}} =
    -5~{\text{pc}}\f$, \f$x_{\text{max}} = y_{\text{max}} = 5~{\text{pc}}\f$ and \f$z_{\text{max}}
    = -2~{\text{pc}}\f$. */
class Trust1Geometry : public GenGeometry
{
    Q_OBJECT
    Q_CLASSINFO("Title", "the geometry from the TRUST1 benchmark model")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE Trust1Geometry();

protected:
    /** This function just sets the values of the parameters, and computes the uniform density. */
    void setupSelfBefore();

    //======================== Other Functions =======================

public:
    /** This function returns the dust mass density \f$\rho({\bf{r}})\f$ at the position
        \f${\bf{r}}\f$. It just sets it to the uniform value inside the slab, and to zero outside.
        */
    double density(Position bfr) const;

    /** This function generates a random position from the geometry, by drawing a random point from
        the three-dimensional probability density \f$p({\bf{r}})\, {\text{d}}{\bf{r}} =
        \rho({\bf{r}})\, {\text{d}}{\bf{r}}\f$. Straightforward in this slab geometry. */
    Position generatePosition() const;

    /** This function returns the X-axis surface density, i.e. the integration of the density along
        the entire X-axis, \f[ \Sigma_X = \int_{-\infty}^\infty \rho(x,0,0)\, {\text{d}}x. \f]
        Trivially zero for this slab geometry. */
    double SigmaX() const;

    /** This function returns the Y-axis surface density, i.e. the integration of the density along
        the entire Y-axis, \f[ \Sigma_Y = \int_{-\infty}^\infty \rho(0,y,0)\, {\text{d}}y. \f]
        Trivially zero for this slab geometry. */
    double SigmaY() const;

    /** This function returns the Z-axis surface density, i.e. the integration of the density along
        the entire Z-axis, \f[ \Sigma_Z = \int_{-\infty}^\infty \rho(0,0,z)\, {\text{d}}z. \f]
        Trivial for this slab geometry. */
    double SigmaZ() const;

    //======================== Data Members ========================

private:
    // data members initialized during setup
    Box _slab;
    double _rho;
};

////////////////////////////////////////////////////////////////////

#endif // TRUST7BGEOMETRY_HPP
