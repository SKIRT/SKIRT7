/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef SUNSED_HPP
#define SUNSED_HPP

#include "StellarSED.hpp"

////////////////////////////////////////////////////////////////////

/** The SunSED class represents the spectral energy distribution from the Sun. */
class SunSED : public StellarSED
{
    Q_OBJECT
    Q_CLASSINFO("Title", "the SED of the Sun")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE SunSED();

protected:
    /** This function reads the solar fluxes from a resource file into a vector, which is
        then resampled on the global wavelength grid. */
    void setupSelfBefore();

    //======================== Other Functions =======================

public:
    /** This static function returns the solar luminosity \f$L_\ell\f$ at the wavelength index
        \f$\ell\f$ for the global wavelength grid associated with the specified simulation
        hierarchy. To find this value, the function creates and deletes a temporary SunSED
        instance as a child of the specified simulation item. */
    static double solarluminosity(SimulationItem* parent, int ell);
};

////////////////////////////////////////////////////////////////////

#endif // SUNSED_HPP
