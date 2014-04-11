/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef PEGASESED_HPP
#define PEGASESED_HPP

#include "StellarSED.hpp"

////////////////////////////////////////////////////////////////////

/** PegaseSED is a class that represents template galaxy %SEDs from the Pegase library. The library
    offers templates for elliptical, lenticular and spiral galaxies. */
class PegaseSED : public StellarSED
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a Pegase galaxy SED")

    Q_CLASSINFO("Property", "type")
    Q_CLASSINFO("Title", "the spectral type of the SED")
    Q_CLASSINFO("E",  "elliptical galaxy (E)")
    Q_CLASSINFO("S0", "lenticular galaxy (S0)")
    Q_CLASSINFO("Sa", "early-type spiral galaxy (Sa)")
    Q_CLASSINFO("Sb", "intermediate-type spiral galaxy (Sb)")
    Q_CLASSINFO("Sc", "late-type spiral galaxy (Sc)")
    Q_CLASSINFO("Default", "E")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE PegaseSED();

protected:
    /** This function reads fluxes from a resource file corresponding to the type of the desired
        Pegase template. The flux vector is regridded on the global wvelength grid. */
    void setupSelfBefore();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** The enumeration type indicating the spectral type of the SED. */
    Q_ENUMS(SpectralType)
    enum SpectralType { E, S0, Sa, Sb, Sc };

    /** Sets the spectral type of the SED. */
    Q_INVOKABLE void setType(SpectralType value);

    /** Returns the spectral type of the SED. */
    Q_INVOKABLE SpectralType type() const;

    //======================== Data Members ========================

private:
    SpectralType _type;
};

////////////////////////////////////////////////////////////////////

#endif // PEGASESED_HPP
