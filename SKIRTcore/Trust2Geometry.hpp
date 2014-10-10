/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef TRUST2GEOMETRY_HPP
#define TRUST2GEOMETRY_HPP

#include "GenGeometry.hpp"

////////////////////////////////////////////////////////////////////

/** The Trust2Geometry class is a subclass of the GenGeometry class and describes the geometry of two
    constant density spheres, embedded in a low-density medium. It is to be used as one of the benchmark
    models for the suite of TRUST 3D benchmark models. Contrary to the model presciption, we have moved
    the entire geometry so that the centre of the coordinate system is located at the centre of the
    system. More precisely, the density can be written as a sum of three components, \f[ \rho({\bf{r}})
    = \rho_0({\bf{r}}) + \rho_1({\bf{r}}) + \rho_2({\bf{r}}), \f] where each of the three densities are
    constant within the boundaries of their volume. For the low-density component, we have a cubic
    volume of size \f$2L_0 \times 2L_0 \times 2L_0\f$, with \f$L_0 = 30~{\text{AU}}\f$. From this cube,
    a spherical part is carved out; the centre of this sphere is located at the bottom-left-front corner
    and the radius of the sphere is \f$R_0 = 15~{\text{AU}}\f$. The second component is a sphere of
    radius \f$R_1 = 5~{\text{AU}}\f$ and it is centered at the position \f${\bf{r}}_1 = (-20~{\text{AU}},
    -15~{\text{AU}},-10~{\text{AU}})\f$. Finally, the third component is a second sphere of radius
    \f$R_2 = 20~{\text{AU}}\f$, centered at the position \f${\bf{r}}_2 = (-10/3~{\text{AU}},
    5/3~{\text{AU}},-5/3~{\text{AU}})\f$. The relative weights of these three components is set by the
    requirement that the surface mass densities of the three components (through their centres) have
    the ratio 1:10000:1000. This leads to total mass contributions of 0.2616%, 38.3609%, and
    61.3775% respectively.  */
class Trust2Geometry : public GenGeometry
{
    Q_OBJECT
    Q_CLASSINFO("Title", "the geometry from the TRUST2 benchmark model")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE Trust2Geometry();

protected:
    /** This function just sets the values of the parameters, and computes the density of the two
        spheres and the low-density medium. */
    void setupSelfBefore();

    //======================== Other Functions =======================

public:
    /** This function returns the dust mass density \f$\rho({\bf{r}})\f$ at the position
        \f${\bf{r}}\f$, according to the prescriptions of the model. */
    double density(Position bfr) const;

    /** This function generates a random position from the geometry, by drawing a random point from
        the three-dimensional probability density \f$p({\bf{r}})\, {\text{d}}{\bf{r}} =
        \rho({\bf{r}})\, {\text{d}}{\bf{r}}\f$. The routine consists of two steps. In the first step
        we generate a uniform deviate that is used to determine from which component the location
        should be drawn. Once this is determined, we locate a random position in either the low-density
        cube (rejecting locations from the curved out sphere) or from either of the two spheres. */
    Position generatePosition() const;

    /** This function returns the X-axis surface density, i.e. the integration of the density along
        the entire X-axis, \f[ \Sigma_X = \int_{-\infty}^\infty \rho(x,0,0)\, {\text{d}}x. \f]
        As the X-axis does not intersect the first sphere, we just have to take into account the
        contribution of the low-density cube and the second sphere. One finds fairly easily
        \f[ \Sigma_X = 2\,\rho_0\,L_0 + 2\,\rho_2 \sqrt{R_2^2-y_2^2-z_2^2}. \f] */
    double SigmaX() const;

    /** This function returns the Y-axis surface density, i.e. the integration of the density along
        the entire Y-axis, \f[ \Sigma_Y = \int_{-\infty}^\infty \rho(0,y,0)\, {\text{d}}y. \f]
        As the Y-axis does not intersect the first sphere, we just have to take into account the
        contribution of the low-density cube and the second sphere. One finds fairly easily
        \f[ \Sigma_Y = 2\,\rho_0\,L_0 + 2\,\rho_2 \sqrt{R_2^2-x_2^2-z_2^2}. \f] */
    double SigmaY() const;

    /** This function returns the Z-axis surface density, i.e. the integration of the density along
        the entire Z-axis, \f[ \Sigma_Z = \int_{-\infty}^\infty \rho(0,0,z)\, {\text{d}}z. \f]
        As the Z-axis does not intersect the first sphere, we just have to take into account the
        contribution of the low-density cube and the second sphere. One finds fairly easily
        \f[ \Sigma_Z = 2\,\rho_0\,L_0 + 2\,\rho_2 \sqrt{R_2^2-x_2^2-y_2^2}. \f] */
    double SigmaZ() const;

    //======================== Data Members ========================

private:
    // data members initialized during setup
    double _M0, _L0, _R0, _rho0;
    double _M1, _R1, _rho1;
    Position _bfr1;
    double _M2, _R2, _rho2;
    Position _bfr2;
};

////////////////////////////////////////////////////////////////////

#endif // TRUST2GEOMETRY_HPP
