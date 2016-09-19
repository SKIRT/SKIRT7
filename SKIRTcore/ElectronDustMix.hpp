/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef ELECTRONDUSTMIX_HPP
#define ELECTRONDUSTMIX_HPP

#include "DustMix.hpp"

////////////////////////////////////////////////////////////////////

/** The ElectronDustMix class represents the optical properties of a population of electrons,
    including polarization. For our purposes here, we consider electrons to be a simplified version
    of dust grains. Electrons do not absorb nor emit photons. Photon scattering is wavelength
    independent and there is only one scattering cross section, the constant Thomson cross section
    \f$\sigma_\text{t}\f$. The Sxx values in the Mueller matrix depend just on \f$\cos\theta\f$ and
    are given by equation (C.7) of Wolf 2003 (Computer Physics Communications, 150, 99–115). */
class ElectronDustMix : public DustMix
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a 'dust mix' representing a population of electrons")

    Q_CLASSINFO("Property", "circularPolarization")
    Q_CLASSINFO("Title", "use a synthetic particle that adds circular polarization during scattering")
    Q_CLASSINFO("Default", "no")
    Q_CLASSINFO("Silent", "true")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE ElectronDustMix();

protected:
    /** This function directly calculates all electron dust mix properties on the simulation's
        wavelength grid. It then adds a single dust population to the dust mix. */
    void setupSelfBefore();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the flag that indicates whether to use a population of synthetic particles that add
        circular polarization during scattering; the default value is false, indicating a
        population of regular electrons. */
    Q_INVOKABLE void setCircularPolarization(bool value);

    /** Returns the flag that indicates whether to use a population of synthetic particles that add
        circular polarization. */
    Q_INVOKABLE bool circularPolarization() const;

    //============= Data Members =============

private:
    bool _circularPolarization;     // true for synthetic particle that adds circular polarization;
                                    // false for regular electron
};

////////////////////////////////////////////////////////////////////

#endif // ELECTRONDUSTMIX_HPP
