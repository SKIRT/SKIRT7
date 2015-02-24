/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef LUMINOSITYSTELLARCOMPNORMALIZATION_HPP
#define LUMINOSITYSTELLARCOMPNORMALIZATION_HPP

#include "StellarCompNormalization.hpp"

////////////////////////////////////////////////////////////////////

/** LuminosityStellarCompNormalization is a class that sets the normalization of a stellar
    component by defining the total luminosity in a given wavelength band. */
class LuminosityStellarCompNormalization : public StellarCompNormalization
{
    Q_OBJECT
    Q_CLASSINFO("Title", "stellar component normalization through the luminosity in a given band")

    Q_CLASSINFO("Property", "band")
    Q_CLASSINFO("Title", "the band in which to set this component's luminosity")
    Q_CLASSINFO("FUV", "the FUV-band")
    Q_CLASSINFO("NUV", "the NUV-band")
    Q_CLASSINFO("U", "the U-band")
    Q_CLASSINFO("B", "the B-band")
    Q_CLASSINFO("V", "the V-band")
    Q_CLASSINFO("R", "the R-band")
    Q_CLASSINFO("I", "the I-band")
    Q_CLASSINFO("J", "the J-band")
    Q_CLASSINFO("H", "the H-band")
    Q_CLASSINFO("K", "the K-band")
    Q_CLASSINFO("SDSSu", "the u-band")
    Q_CLASSINFO("SDSSg", "the g-band")
    Q_CLASSINFO("SDSSr", "the r-band")
    Q_CLASSINFO("SDSSi", "the i-band")
    Q_CLASSINFO("SDSSz", "the z-band")
    Q_CLASSINFO("Default", "V")

    Q_CLASSINFO("Property", "luminosity")
    Q_CLASSINFO("Title", "this component's luminosity in the specified band, in solar units")
    Q_CLASSINFO("MinValue", "0")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE LuminosityStellarCompNormalization();

protected:
    /** This function verifies the validity of the luminosity value and converts it to program
        units. */
    void setupSelfBefore();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** The enumeration type indicating a wavelength band. */
    Q_ENUMS(Band)
    enum Band { FUV, NUV, U, B, V, R, I, J, H, K, SDSSu, SDSSg, SDSSr, SDSSi, SDSSz };

    /** Sets the band in which to set this component's luminosity. */
    Q_INVOKABLE void setBand(Band value);

    /** Returns the band in which to set this component's luminosity. */
    Q_INVOKABLE Band band() const;

    /** Sets the X-band luminosity \f$L_{\text{X}}\f$, given in solar units (i.e. multiples of the
        monochromatic luminosity of the sun in that particular band). */
    Q_INVOKABLE void setLuminosity(double value);

    /** Returns the X-band luminosity \f$L_{\text{X}}\f$, in solar units. */
    Q_INVOKABLE double luminosity() const;

    //======================== Other Functions =======================

public:
    /** This function returns the total, bolometric luminosity of a (virtual) stellar component
        that would have a given %SED. For the present type of normalization, the bolometric
        luminosity is \f[ L_{\text{bol}} = \frac{ L_{\text{X}} }{ S_{\text{X}} } \f] with
        \f$L_{\text{X}}\f$ the X-band luminosity (a data member of the class) and
        \f$S_{\text{X}}\f$ the value of the (normalized) %SED at the X-band filter. */
    double totluminosity(SED* sed) const;

private:
    /** This function returns the effective wavelength for the specified band. */
    static double effectivewavelength(Band X);

    //======================== Data Members ========================

private:
    Band _X;
    int _ell;
    double _LX_Lsun;    // solar units
    double _LX_Wm;      // program units (W/m)
};

////////////////////////////////////////////////////////////////////

#endif // LUMINOSITYSTELLARCOMPNORMALIZATION_HPP
