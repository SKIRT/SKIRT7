/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef BRUZUALCHARLOTSEDFAMILY_HPP
#define BRUZUALCHARLOTSEDFAMILY_HPP

#include "ArrayTable.hpp"
#include "SEDFamily.hpp"
class WavelengthGrid;

//////////////////////////////////////////////////////////////////////

/** An instance of the BruzualCharlotSEDFamily class represents the family of Bruzual & Charlot
    SEDs for single stellar populations, parameterized on metallicity and age (Bruzual & Charlot
    2003, RAS 344, 1000-1026). The data was downloaded from
    http://www2.iap.fr/users/charlot/bc2003/. We use the low resolution version of the
    Padova1994/chabrier model, which is one of the two recommended models. The Bruzual & Charlot
    library data is read from the appropriate resource files during setup, and it is subsequently
    interpolated to the desired parameters and wavelength grid points by calling the luminosities()
    function as often as needed. */
class BruzualCharlotSEDFamily : public SEDFamily
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a Bruzual-Charlot SED family for single stellar populations")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE BruzualCharlotSEDFamily();

    /** This constructor can be invoked programmatically by classes that use a hard-coded SED
        family (as opposed to selected through the ski file). Before the constructor returns, the
        newly created object is hooked up as a child to the specified parent in the simulation
        hierarchy (so it will automatically be deleted), and its setup() function has been called.
        */
    explicit BruzualCharlotSEDFamily(SimulationItem* parent);

protected:
    /** This function reads the Bruzual & Charlot library data from the appropriate resource files
        and stores all relevant information internally. */
    void setupSelfBefore();

    //====================== Retrieving an SED =====================

public:
    /** This function returns the luminosity \f$L_\ell\f$ at each wavelength in the simulation's
        wavelength grid for a stellar population with given initial mass \em M (in \f$M_\odot\f$
        at \f$t=0\f$), metallicity \em Z (as a dimensionless fraction), and age \em t (in years).
        The luminosity is defined as the emissivity multiplied by the width of the wavelength bin.
        */
    Array luminosities(double M, double Z, double t) const;

    /** This function returns the number of parameters used by this particular %SED family, in
        other words the number of arguments of the luminosities() function. */
    int nparams_generic() const;

    /** This function returns the luminosity \f$L_\ell\f$ at each wavelength in the simulation's
        wavelength grid for the specified parameter values, which should be in the same order and
        using the same units as the arguments described for the luminosities() function. The first
        \em skipvals values in the \em params array are ignored. */
    Array luminosities_generic(const Array& params, int skipvals=0) const;

    //====================== Data members =====================

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
