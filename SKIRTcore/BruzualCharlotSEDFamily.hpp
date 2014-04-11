/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef BRUZUALCHARLOTSEDFAMILY_HPP
#define BRUZUALCHARLOTSEDFAMILY_HPP

#include "ArrayTable.hpp"
class SimulationItem;
class WavelengthGrid;

//////////////////////////////////////////////////////////////////////

/** The BruzualCharlotSEDFamily class is a technical helper class representing the family of
    Bruzual & Charlot SEDs for stellar populations, parameterized on metallicity and age (Bruzual &
    Charlot 2003, RAS 344, 1000-1026). The data was downloaded from
    http://www2.iap.fr/users/charlot/bc2003/. We use the low resolution version of the
    Padova1994/chabrier model, which is one of the two recommended models. The Bruzual & Charlot
    library data is read from the appropriate resource files in the constructor, and it is
    subsequently interpolated to the desired parameters and wavelength grid points by calling the
    luminosities() function as often as needed. */
class BruzualCharlotSEDFamily
{
public:
    /** The constructor reads the Bruzual & Charlot library data from the appropriate resource
        files and stores all relevant information internally. The specified simulation item is used
        to retrieve the simulation's wavelength grid and log object. */
    BruzualCharlotSEDFamily(SimulationItem* item);

    /** This function returns the luminosity \f$L_\ell\f$ at each wavelength in the simulation's
        wavelength grid for a stellar population with given initial mass \em M (in \f$M_\odot\f$
        at \f$t=0\f$), metallicity \em Z (as a dimensionless fraction), and age \em t (in years).
        The luminosity is defined as the emissivity multiplied by the width of the wavelength bin.
        */
    Array luminosities(double M, double Z, double t) const;

private:
    WavelengthGrid* _lambdagrid;

    // contents of the library, read by constructor
    Array _lambdav;
    Array _tv;
    Array _Zv;
    ArrayTable<3> _jvv;
};

////////////////////////////////////////////////////////////////////

#endif // BRUZUALCHARLOTSEDFAMILY_HPP
