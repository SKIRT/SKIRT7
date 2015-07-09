/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef MAPPINGSSEDFAMILY_HPP
#define MAPPINGSSEDFAMILY_HPP

#include "ArrayTable.hpp"
#include "SEDFamily.hpp"
class WavelengthGrid;

//////////////////////////////////////////////////////////////////////

/** An instance of the MappingsSEDFamily class represents the family of MAPPINGS III
    starburst template SEDs, parameterized on metallicity, compactness, ISM pressure and PDR
    covering factor, as described in Groves et al. (2008) ApJS,176,438. The data was downloaded
    from http://www.mpia-hd.mpg.de/~brent/starburst.html -> Cparam_models.save and converted to
    plain text files using a simple IDL script. The MAPPINGS III library data is read from the
    appropriate resource files during setup, and it is subsequently interpolated to the
    desired parameters and wavelength grid points by calling the luminosities() function as often
    as needed. */
class MappingsSEDFamily : public SEDFamily
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a MAPPINGS III SED family for starbursting regions")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE MappingsSEDFamily();

    /** This constructor can be invoked programmatically by classes that use a hard-coded SED
        family (as opposed to selected through the ski file). Before the constructor returns, the
        newly created object is hooked up as a child to the specified parent in the simulation
        hierarchy (so it will automatically be deleted), and its setup() function has been called.
        */
    explicit MappingsSEDFamily(SimulationItem* parent);

protected:
    /** This function reads the MAPPINGS III library data from the appropriate resource files and
        stores all relevant information internally. */
    void setupSelfBefore();

    //====================== Retrieving an SED =====================

public:
    /** This function returns the luminosity \f$L_\ell\f$ at each wavelength in the simulation's
        wavelength grid for a starbursting population, given the star formation rate \f$\dot{M}\f$
        (assumed to be constant over the past 10 Myr, in \f$M_\odot\,{\text{yr}}^{-1}\f$),
        metallicity \f$Z\f$, the logarithm of the compactness \f$\log C\f$, the ISM pressure
        \f$p\f$ (in Pa), and the dimensionless PDR covering factor \f$f_{\text{PDR}}\f$. */
    Array luminosities(double SFR, double Z, double logC, double pressure, double fPDR) const;

    /** This function returns the number of parameters used by this particular %SED family, in
        other words the number of arguments of the luminosities() function. */
    int nparams() const;

    /** This function returns the luminosity \f$L_\ell\f$ at each wavelength in the simulation's
        wavelength grid for the specified parameter values, which should be in the same order and
        using the same units as the arguments described for the luminosities() function. The first
        \em skipvals values in the \em params array are ignored. */
    Array luminosities_generic(const Array& params, int skipvals=0) const;

    /** This function returns the mass (in \f$M_\odot\f$) of the source represented by the
        specified set of parameter values. The \em params array must contain the appropriate number
        of parameter values in the order specified by the particular %SED family subclass. The
        first \em skipvals values in the array are ignored. */
    double mass_generic(const Array& params, int skipvals=0) const;

    /** This function returns a short name for the type of sources typically assigned to this
        particular %SED family. */
     QString sourceName() const;

     /** This function returns a description for the type of sources typically assigned to this
         particular %SED family. */
      QString sourceDescription() const;

    //====================== Data members =====================

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
