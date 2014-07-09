/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef OLIGODUSTSYSTEM_HPP
#define OLIGODUSTSYSTEM_HPP

#include "DustSystem.hpp"

//////////////////////////////////////////////////////////////////////

/** An OligoDustSystem class object represents a complete dust system for use with oligochromatic
    simulations. All functionality is implemented in the DustSystem base class. */
class OligoDustSystem : public DustSystem
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a dust system for use with oligochromatic simulations")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE OligoDustSystem();

    //======================== Other Functions =======================

public:
    /** The function simulates the absorption of a monochromatic luminosity package in the
        specified dust cell. It should never be invoked for oligochromatic simulations. */
    void absorb(int m, int ell, double DeltaL, bool ynstellar);
};

//////////////////////////////////////////////////////////////////////

#endif // OLIGODUSTSYSTEM_HPP
