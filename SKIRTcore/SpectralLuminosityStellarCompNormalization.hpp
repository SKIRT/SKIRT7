/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef SPECTRALLUMINOSITYSTELLARCOMPNORMALIZATION_HPP
#define SPECTRALLUMINOSITYSTELLARCOMPNORMALIZATION_HPP

#include "StellarCompNormalization.hpp"

////////////////////////////////////////////////////////////////////

/** SpectralLuminosityStellarCompNormalization is a class that sets the normalization of a stellar
    component by defining the spectral luminosity (radiative power per wavelength) at a certain
    wavelength. */
class SpectralLuminosityStellarCompNormalization : public StellarCompNormalization
{
    Q_OBJECT
    Q_CLASSINFO("Title", "stellar component normalization through the spectral luminosity at a given wavelength")

    Q_CLASSINFO("Property", "wavelength")
    Q_CLASSINFO("Title", "the wavelength at which to specify the spectral luminosity")
    Q_CLASSINFO("Quantity", "wavelength")
    Q_CLASSINFO("MinValue", "1 A")
    Q_CLASSINFO("MaxValue", "1 m")

    Q_CLASSINFO("Property", "luminosity")
    Q_CLASSINFO("Title", "this component's spectral luminosity at the specified wavelength")
    Q_CLASSINFO("Quantity", "monluminosity")
    Q_CLASSINFO("MinValue", "0")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE SpectralLuminosityStellarCompNormalization();

protected:
    /** This function verifies the validity of the luminosity value. */
    void setupSelfBefore();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** This function sets the wavelength at which the spectral luminosity is defined for
        normalizing the stellar component. */
    Q_INVOKABLE void setWavelength(double value);

    /** This function returns the wavelength the spectral luminosity is defined for normalizing the
        stellar component. */
    Q_INVOKABLE double wavelength() const;

    /** Sets the spectral luminosity (radiative power per wavelength) at the specified wavelength.
        */
    Q_INVOKABLE void setLuminosity(double value);

    /** Returns spectral luminosity (radiative power per wavelength) at the specified wavelength.
        */
    Q_INVOKABLE double luminosity() const;

    //======================== Other Functions =======================

public:
    /** This function returns the total, bolometric luminosity of a (virtual) stellar component
        that would have a given %SED. For the present type of normalization, the bolometric
        luminosity is \f[ L_{\text{bol}} = \frac{L_{\lambda,\ell}\Delta\lambda_\ell}{S_\ell} \f]
        with \f$\ell\f$ the wavelength bin corresponding to the specified wavelength,
        \f$\Delta\lambda_\ell\f$ the width of that wavelength bin, \f$L_{\lambda,\ell}\f$ the
        specified spectral luminosity (for the wavelength corresponding to the bin), and
        \f$S_\ell\f$ the value of the (normalized) %SED for the same wavelength bin. */
    double totluminosity(SED* sed) const;

    //======================== Data Members ========================

private:
    // discoverable properties
    double _lambda;     // m
    double _Llambda;    // (W/m)

    // initialized during setup
    double _dlambda;
    int _ell;
};

////////////////////////////////////////////////////////////////////

#endif // SPECTRALLUMINOSITYSTELLARCOMPNORMALIZATION_HPP
