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
        The first \em skipvals values in the array are ignored. */
    virtual Array luminosities_generic(const Array& params, int skipvals=0) const = 0;

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
