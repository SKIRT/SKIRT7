/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef STARBURSTSED_HPP
#define STARBURSTSED_HPP

#include "StellarSED.hpp"

////////////////////////////////////////////////////////////////////

/** StarburstSED is a class that represents spectral energy distributions of starbursting stellar
    populations. The SEDs are generated from data of the Starburst99 library; see Leitherer et al.
    (1999, ApJS, 123, 3–40). More specifically, they represent stellar populations with a constant,
    continuous star formation rate that have evolved for 100 Myr. The initial mass function has is
    a simple Salpeter IMF (a power law with \f$\alpha=2.35\f$) with \f$1~M_\odot\f$ and
    \f$100~M_\odot\f$ as lower and upper masses. Populations with different metallicities can be
    chosen (with \f$Z\f$ ranging between 0.001 and 0.040). */
class StarburstSED : public StellarSED
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a Starburst stellar population SED")

    Q_CLASSINFO("Property", "metallicity")
    Q_CLASSINFO("Title", "the metallicity of the population")
    Q_CLASSINFO("MinValue", "0.001")
    Q_CLASSINFO("MaxValue", "0.04")
    Q_CLASSINFO("Default", "0.02")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE StarburstSED();

protected:
    /** This function reads fluxes from a resource file. It calculates a vector with the
        SED by interpolating between the two populations with metallicities that bracket the
        desired metallicity \f$Z\f$. This vector is regridded on the global wavelength grid. */
    void setupSelfBefore();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the metallicity of the population. */
    Q_INVOKABLE void setMetallicity(double value);

    /** Returns the metallicity of the population. */
    Q_INVOKABLE double metallicity() const;

    //======================== Data Members ========================

private:
    double _Z;
};

////////////////////////////////////////////////////////////////////

#endif // STARBURSTSED_HPP
