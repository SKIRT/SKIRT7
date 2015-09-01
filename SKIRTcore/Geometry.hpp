/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef GEOMETRY_HPP
#define GEOMETRY_HPP

#include "AngularDistribution.hpp"
#include "SimulationItem.hpp"
class Random;

//////////////////////////////////////////////////////////////////////

/** Geometry is the general abstract class that describes the geometry of a stellar or dust
    component. It is an abstract class, which essentially describes the different tasks that need
    to be fulfilled by any subclass. There are two key member functions that each Geometry subclass
    should have: a function returning the density \f$\rho({\bf{r}})\f$ and a member function
    drawing a random position from this geometry. We normalize each geometry such that the total
    mass is equal to one, i.e. \f[ \iiint \rho({\bf{r}})\, {\text{d}}{\bf{r}} = 1.\f]

    The Geometry class implements the AngularDistribution interface. By default the emission from a
    geometry is isotropic, i.e. a direction is drawn randomly from a uniform distribution on the
    unit sphere. To implement anisotropic emission a Geometry subclass should override the
    functions probabilityForDirection() and generateDirection(). */
class Geometry : public SimulationItem, public AngularDistribution
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a geometry")

    //============= Construction - Setup - Destruction =============

protected:
    /** The default constructor; it is protected since this is an abstract class. */
    Geometry();

    /** This function caches some values for use by subclasses. */
    void setupSelfBefore();

    //======================== Other Functions =======================

public:
    /** This pure virtual function returns the dimension of the geometry, which depends on its
        (lack of) symmetry. A value of 1 means spherical symmetry, 2 means axial symmetry and 3
        means none of these symmetries. The function's implementation must be provided in a
        subclass. */
    virtual int dimension() const = 0;

    /** This pure virtual function returns the density \f$\rho({\bf{r}})\f$ at the position
        \f${\bf{r}}\f$. */
    virtual double density(Position bfr) const = 0;

    /** This pure virtual function generates a random position from the geometry, by
        drawing a random point from the three-dimensional probability density \f$p({\bf{r}})\,
        {\text{d}}{\bf{r}} = \rho({\bf{r}})\, {\text{d}}{\bf{r}}\f$. */
    virtual Position generatePosition() const = 0;

    /** This pure virtual function returns the X-axis surface density, i.e. the integration of
        the density along the entire X-axis, \f[ \Sigma_X = \int_{-\infty}^\infty \rho(x,0,0)\,
        {\text{d}}x. \f] */
    virtual double SigmaX() const = 0;

    /** This pure virtual function returns the Y-axis surface density, i.e. the integration of
        the density along the entire Y-axis, \f[ \Sigma_Y = \int_{-\infty}^\infty \rho(0,y,0)\,
        {\text{d}}y. \f] */
    virtual double SigmaY() const = 0;

    /** This pure virtual function returns the Z-axis surface density, i.e. the integration of
        the density along the entire Z-axis, \f[ \Sigma_Z = \int_{-\infty}^\infty \rho(0,0,z)\,
        {\text{d}}z. \f] */
    virtual double SigmaZ() const = 0;

    // - - - - - - AngularDistribution interface - - - - - -

    /** This function implements part of the AngularDistribution interface. It returns the
        probability \f$P(\Omega)\f$ for a given direction \f$(\theta,\phi)\f$ at the specified
        wavelength and position. The default implementation provided in this top-level class offers
        an isotropic distribution, in other words this function always returns 1 for any direction,
        and for any wavelength and position. Override this function and generateDirection() to
        implement anisotropic emission. */
    double probabilityForDirection(int ell, Position bfr, Direction bfk) const;

    /** This function implements part of the AngularDistribution interface. It generates a random
        direction \f$(\theta,\phi)\f$ drawn from the probability distribution \f$P(\Omega)
        \,{\mathrm{d}}\Omega\f$ at the specified wavelength and position. The default
        implementation provided in this top-level class offers an isotropic distribution, i.e. a
        direction is drawn randomly from a uniform distribution on the unit sphere, regardless of
        wavelength and position. Override this function and probabilityForDirection() to implement
        anisotropic emission. */
    Direction generateDirection(int ell, Position bfr) const;

    //======================== Data Members ========================

protected:
    // data member initialized in this class, as a service to subclasses
    Random* _random;
};

////////////////////////////////////////////////////////////////////

#endif // GEOMETRY_HPP
