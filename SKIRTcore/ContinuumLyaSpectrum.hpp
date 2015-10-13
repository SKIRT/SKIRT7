/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef CONTINUUMLYASPECTRUM_HPP
#define CONTINUUMLYASPECTRUM_HPP

#include "LyaSpectrum.hpp"

////////////////////////////////////////////////////////////////////

/** ContinuumLyaSpectrum is a subclass of the LyaSpectrum class that describes an emission spectrum
    that is flat in luminosity per wavelength units. In other words, the spectrum is such that
    \f$S_\lambda = {\text{constant}}\f$ over the entire wavelength or velocity grid. */
class ContinuumLyaSpectrum : public LyaSpectrum
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a continuum emission spectrum")

    Q_CLASSINFO("Property", "level")
    Q_CLASSINFO("Title", "the continuum level")
    Q_CLASSINFO("Quantity", "monluminosity")
    Q_CLASSINFO("MinValue", "0")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE ContinuumLyaSpectrum();

protected:
    /** This function verifies the validity of the luminosity level. It fills the luminosity vector
        by converting this monochromatic luminosity level to a bolometric luminosity at each of the
        grid points in the global wavelength grid. */
    void setupSelfBefore();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the continuum level. */
    Q_INVOKABLE void setLevel(double value);

    /** Returns the continuum level. */
    Q_INVOKABLE double level() const;

    //======================== Data Members ========================

private:
    double _Llambda;
};

////////////////////////////////////////////////////////////////////

#endif // CONTINUUMLYASPECTRUM_HPP
