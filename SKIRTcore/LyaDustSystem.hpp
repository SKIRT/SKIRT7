/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef LYADUSTSYSTEM_HPP
#define LYADUSTSYSTEM_HPP

#include "DustSystem.hpp"

//////////////////////////////////////////////////////////////////////

/** An LyaDustSystem class object represents a complete dust system for use with Lyα
    simulations. All functionality is implemented in the DustSystem base class. */
class LyaDustSystem : public DustSystem
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a dust system for use with Lyα simulations")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE LyaDustSystem();

    //======================== Other Functions =======================

public:
    /** This function always returns false since Lyα simulations do no support dust
        emission. */
    bool dustemission() const;

    /** The function simulates the absorption of a monochromatic luminosity package in the
        specified dust cell. It should never be invoked for Lyα simulations. */
    void absorb(int m, int ell, double DeltaL, bool ynstellar);
};

//////////////////////////////////////////////////////////////////////

#endif // LYADUSTSYSTEM_HPP
