/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef SPHGASPARTICLE_HPP
#define SPHGASPARTICLE_HPP

#include "Vec.hpp"
class Box;

////////////////////////////////////////////////////////////////////

/** SPHGasParticle is a technical class representing a single SPH gas particle. An SPHGasParticle
    instance contains the attributes for a gas particle imported from a smoothed particle
    hydrodynamical (SPH) simulation, and offers functions to calculate relevant results from these
    attributes. */
class SPHGasParticle
{
public:
    /** The constructor takes the particle attributes in standard SI units: coordinates of the
        center (m), smoothing length (m), total mass (kg), metallicity (dimensionless fraction). */
    SPHGasParticle(Vec rc, double h, double M, double Z);

    /** This function returns the coordinates of the center of the particle. */
    Vec center() const;

    /** This function returns the x, y, or z-coordinate of the center of the particle, depending on
        the value of dir: 1->x, 2->y, 3->z. For any other value of dir the function returns zero. */
    double center(int dir) const;

    /** This function returns the smoothing length of the particle. */
    double radius() const;

    /** This function returns the total metal mass of the particle. */
    double metalMass() const;

    /** This function returns the metal mass of the particle inside a given box (i.e. a cuboid
        lined up with the coordinate axes). */
    double metalMassInBox(const Box& box) const;

    /** This function returns the metal density of the particle at the specified position. */
    double metalDensity(Vec r) const;

private:
    // data members received as constructor arguments
    Vec _rc;    // center coordinates (m)
    double _h;  // smoothing length (m)
    double _M;  // total mass (kg)
    double _Z;  // metallicity (fraction)

    // data members derived from arguments in constructor (cached for speed)
    double _norm;  // squared distance normalization factor:  1/(h*h)
    double _rho0;  // central metal density:  8.0/M_PI * M/(h*h*h) * Z
    double _rho2;  // twice the central metal density
    double _s;     // the scaling factor (h/2.42)^-1
    double _MZ8;   // the front factor M*Z/8
};

////////////////////////////////////////////////////////////////////

#endif // SPHGASPARTICLE_HPP
