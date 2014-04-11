/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef DUSTPARTICLEINTERFACE_HPP
#define DUSTPARTICLEINTERFACE_HPP

#include <Vec.hpp>

////////////////////////////////////////////////////////////////////

/** DustParticleInterface is a pure interface. It is implemented by dust distributions that are
    defined through a set of particles (such as SPH particles). In some cases, the locations of
    these particles can serve as a hint for building an appropriate dust grid. */
class DustParticleInterface
{
protected:
    /** The empty constructor for the interface. */
    DustParticleInterface() { }

public:
    /** The empty destructor for the interface. */
    virtual ~DustParticleInterface() { }

    /** This function returns the number of particles defining the dust distribution. */
    virtual int numParticles() const = 0;

    /** This function returns the coordinates of the dust-distribution-defining particle with the
        specified zero-based index. If the index is out of range, a fatal error is thrown. */
    virtual Vec particleCenter(int index) const = 0;
};

/////////////////////////////////////////////////////////////////////////////

#endif // DUSTPARTICLEINTERFACE_HPP
