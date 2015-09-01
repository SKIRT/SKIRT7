/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef SEDFAMILY_HPP
#define SEDFAMILY_HPP

#include "Array.hpp"
#include "SimulationItem.hpp"

//////////////////////////////////////////////////////////////////////

/** SEDFamily is an abstract class for representing some family of SEDs. Each subclass implements
    an %SED family, where the exact form of the %SED depends on one or more parameters. This base
    class offers a generic interface for obtaining a particlar #SED from the family, given the
    appropriate number of parameter values. */
class SEDFamily : public SimulationItem
{
    Q_OBJECT
    Q_CLASSINFO("Title", "an SED family")

    //============= Construction - Setup - Destruction =============

protected:
    /** The default constructor. */
    SEDFamily();

    //====================== Retrieving an SED =====================

public:
    /** This function returns the number of parameters used by this particular %SED family. */
    virtual int nparams() const = 0;

    /** This function returns the luminosity \f$L_\ell\f$ (defined as emissivity multiplied by the
        width of the wavelength bin) at each wavelength in the simulation's wavelength grid for the
        specified set of parameter values. The \em params array must contain the appropriate number
        of parameter values in the order specified by the particular %SED family subclass.
        The first \em skipvals values in the array are ignored.

        If the redshift argument \f$z\f$ is present and nonzero, the spectrum is redshifted
        according to the specified value. In function of the rest wavelength \f$\lambda_0\f$, the
        redshifted wavelength \f$\lambda_z\f$ is given by \f$\lambda_z=(1+z)\,\lambda_0\f$. For
        \f$z<<1\f$, the observed luminosity at each wavelength in the simulation's wavelength grid
        can then be written as \f[L_z[\lambda_\ell] = L_0[(1-z)\,\lambda_\ell]\f] */
    virtual Array luminosities_generic(const Array& params, int skipvals=0, double z=0) const = 0;

    /** This function returns the mass (in \f$M_\odot\f$) of the source represented by the
        specified set of parameter values. The \em params array must contain the appropriate number
        of parameter values in the order specified by the particular %SED family subclass. The
        first \em skipvals values in the array are ignored. */
    virtual double mass_generic(const Array& params, int skipvals=0) const = 0;

    /** This function returns a short name for the type of sources typically assigned to this
        particular %SED family. The name is used as part of filenames; it should be lowercase only
        and it should not contain spaces or punctuation. */
    virtual QString sourceName() const = 0;

    /** This function returns a description for the type of sources typically assigned to this
        particular %SED family. The description is used in log messages. */
     virtual QString sourceDescription() const = 0;
};

////////////////////////////////////////////////////////////////////

#endif // SEDFAMILY_HPP
