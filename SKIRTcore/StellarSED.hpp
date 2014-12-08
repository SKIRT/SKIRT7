/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef STELLARSED_HPP
#define STELLARSED_HPP

#include "SED.hpp"

////////////////////////////////////////////////////////////////////

/** StellarSED is an abstract subclass of the SED class and represents spectral energy
    distributions corresponding to stellar sources. */
class StellarSED : public SED
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a stellar SED")

    //============= Construction - Setup - Destruction =============

protected:
    /** The default constructor; it is protected since this is an abstract class. */
    StellarSED();
};

////////////////////////////////////////////////////////////////////

#endif // STELLARSED_HPP
