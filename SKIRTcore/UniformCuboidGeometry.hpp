/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef UNIFORMCUBOIDGEOMETRY_HPP
#define UNIFORMCUBOIDGEOMETRY_HPP

#include "BoxGeometry.hpp"
#include "GenGeometry.hpp"

////////////////////////////////////////////////////////////////////

/** The UniformCuboidGeometry class describes a 3D geometry consisting of a uniform and isotropic
    cuboid aligned with the coordinate system. */
class UniformCuboidGeometry : public BoxGeometry
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a uniform cuboid geometry")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE UniformCuboidGeometry();

protected:
    /** This function calculates the (constant) density at each point, assuming a total mass of 1.
        */
    void setupSelfBefore();

    //======================== Other Functions =======================

public:
    /** This function returns the density \f$\rho({\bf{r}})\f$ for this geometry at the position
        \f${\bf{r}}\f$. It returns the constant value
        \f$\rho=1/(x_\text{width}\,y_\text{width}\,z_\text{width})\f$ for all points inside the
        cuboid, and zero outside the cuboid. */
    double density(Position bfr) const;

    /** This function generates a random position from the geometry, by drawing a uniform random
        point inside the cuboid. */
    Position generatePosition() const;

    /** This function returns the X-axis surface density of the geometry, defined as the
        integration of the density along the entire X-axis. */
    double SigmaX() const;

    /** This function returns the Y-axis surface density of the geometry, defined as the
        integration of the density along the entire Y-axis. */
    double SigmaY() const;

    /** This function returns the Z-axis surface density of the geometry, defined as the
        integration of the density along the entire Z-axis. */
    double SigmaZ() const;

    //======================== Data Members ========================

private:
    // other data members
    double _rho;
};

////////////////////////////////////////////////////////////////////

#endif // UNIFORMCUBOIDGEOMETRY_HPP
