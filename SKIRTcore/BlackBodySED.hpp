/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef BLACKBODYSED_HPP
#define BLACKBODYSED_HPP

#include "StellarSED.hpp"

////////////////////////////////////////////////////////////////////

/** BlackBodySED is a class that describes black-body spectral energy distributions, i.e. the
    emission spectra of perfect absorbers which are in thermal equilibrium. Such an %SED is
    characterized by the temperature of the object, and its spectrum is the Planck spectrum. */
class BlackBodySED : public StellarSED
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a black body SED")

    Q_CLASSINFO("Property", "temperature")
    Q_CLASSINFO("Title", "the black body temperature")
    Q_CLASSINFO("Quantity", "temperature")
    Q_CLASSINFO("MinValue", "0")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE BlackBodySED();

protected:
    /** This function verifies the validity of the temperature and calculates a vector with the
        Planck function \f$ B_\lambda(T) \f$ sampled at all grid points of the global wavelength
        grid. */
    void setupSelfBefore();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the black body temperature. */
    Q_INVOKABLE void setTemperature(double value);

    /** Returns the black body temperature. */
    Q_INVOKABLE double temperature() const;

    //======================== Data Members ========================

private:
    double _T;
};

////////////////////////////////////////////////////////////////////

#endif // BLACKBODYSED_HPP
