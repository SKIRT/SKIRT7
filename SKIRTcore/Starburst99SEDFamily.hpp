/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef STARBURST99SEDFAMILY_HPP
#define STARBURST99SEDFAMILY_HPP

#include "ArrayTable.hpp"
#include "SEDFamily.hpp"
class WavelengthGrid;

//////////////////////////////////////////////////////////////////////

/** An instance of the Starburst99SEDFamily class represents a family of Starburst99 SEDs for
    single stellar populations (Leitherer et al. 1999, ApJS, 123, 3), assuming the Kroupa initial
    mass function (Kroupa 2001, MNRAS, 322, 231), and parameterized on metallicity and age. The
    library data was prepared and bundled into a FITS file by Patrik Jonsson for use by the \c
    Sunrise code (<tt>2013ascl.soft03030J</tt>). It was downloaded from
    <tt>https://bitbucket.org/lutorm/sunrise/downloads/Patrik-imfKroupa-Zmulti-ml.fits.gz</tt> for
    inclusion in \c SKIRT as a resource file. The library data is read from this resource file
    during setup, and it is subsequently interpolated to the desired parameters and wavelength grid
    points by calling the luminosities() function as often as needed. */
class Starburst99SEDFamily : public SEDFamily
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a Starburst99 SED family for single stellar populations")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE Starburst99SEDFamily();

    /** This constructor can be invoked programmatically by classes that use a hard-coded SED
        family (as opposed to selected through the ski file). Before the constructor returns, the
        newly created object is hooked up as a child to the specified parent in the simulation
        hierarchy (so it will automatically be deleted), and its setup() function has been called.
        */
    explicit Starburst99SEDFamily(SimulationItem* parent);

protected:
    /** This function reads the library data from the appropriate resource files
        and stores all relevant information internally. */
    void setupSelfBefore();

    //====================== Retrieving an SED =====================

public:
    /** This function returns the luminosity \f$L_\ell\f$ at each wavelength in the simulation's
        wavelength grid for a stellar population with given initial mass \em M (in \f$M_\odot\f$
        at \f$t=0\f$), metallicity \em Z (as a dimensionless fraction), and age \em t (in years).
        The luminosity is defined as the emissivity multiplied by the width of the wavelength bin.

        If the redshift argument \f$z\f$ is present and nonzero, the spectrum is redshifted
        according to the specified value. In function of the rest wavelength \f$\lambda_0\f$, the
        redshifted wavelength \f$\lambda_z\f$ is given by \f$\lambda_z=(1+z)\,\lambda_0\f$. For
        \f$z<<1\f$, the observed luminosity at each wavelength in the simulation's wavelength grid
        can then be written as \f[L_z[\lambda_\ell] = L_0[(1-z)\,\lambda_\ell]\f] */
    Array luminosities(double M, double Z, double t, double z=0) const;

    /** This function returns the number of parameters used by this particular %SED family, in
        other words the number of arguments of the luminosities() function. */
    int nparams() const;

    /** This function returns the luminosity \f$L_\ell\f$ at each wavelength in the simulation's
        wavelength grid for the specified parameter values, which should be in the same order and
        using the same units as the arguments described for the luminosities() function. The first
        \em skipvals values in the \em params array are ignored. */
    Array luminosities_generic(const Array& params, int skipvals=0, double z=0) const;

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
    Array _Zv;
    Array _tv;
    ArrayTable<3> _jvv;
};

////////////////////////////////////////////////////////////////////

#endif // STARBURST99SEDFAMILY_HPP
