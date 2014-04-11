/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef MAPPINGSSEDFAMILY_HPP
#define MAPPINGSSEDFAMILY_HPP

#include "ArrayTable.hpp"
class SimulationItem;
class WavelengthGrid;

//////////////////////////////////////////////////////////////////////

/** The MappingsSEDFamily class is a technical helper class representing the family of MAPPINGS III
    starburst template SEDs, parameterized on metallicity, compactness, ISM pressure and PDR
    covering factor, as described in Groves et al. (2008) ApJS,176,438. The data was downloaded
    from http://www.mpia-hd.mpg.de/~brent/starburst.html -> Cparam_models.save and converted to
    plain text files using a simple IDL script. The MAPPINGS III library data is read from the
    appropriate resource files in the constructor, and it is subsequently interpolated to the
    desired parameters and wavelength grid points by calling the luminosities() function as often
    as needed. */
class MappingsSEDFamily
{
public:
    /** The constructor reads the MAPPINGS III library data from the appropriate resource
        files and stores all relevant information internally. The specified simulation item is used
        to retrieve the simulation's wavelength grid and log object. */
    MappingsSEDFamily(SimulationItem* item);

    /** This function returns the luminosity \f$L_\ell\f$ at each wavelength in the simulation's
        wavelength grid for a starbursting population, given the star formation rate \f$\dot{M}\f$
        (assumed to be constant over the past 10 Myr, in \f$M_\odot\,{\text{yr}}^{-1}\f$),
        metallicity \f$Z\f$, the logarithm of the compactness \f$\log C\f$, the ISM pressure
        \f$p\f$ (in Pa), and the dimensionless PDR covering factor \f$f_{\text{PDR}}\f$. */
    Array luminosities(double SFR, double Z, double logC, double pressure, double fPDR) const;

private:
    WavelengthGrid* _lambdagrid;

    // contents of the library, read by constructor
    Array _lambdav;
    Array _Zrelv;
    Array _logCv;
    Array _logpv;
    ArrayTable<4> _j0vv;
    ArrayTable<4> _j1vv;
};

////////////////////////////////////////////////////////////////////

#endif // MAPPINGSSEDFAMILY_HPP
