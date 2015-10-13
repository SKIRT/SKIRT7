/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef LINELYASPECTRUM_HPP
#define LINELYASPECTRUM_HPP

#include "LyaSpectrum.hpp"

////////////////////////////////////////////////////////////////////

/** LineLyaSpectrum is a subclass of the LyaSpectrum class that describes the emission spectrum
    corresponding to a single Lyα line. Free parameters in class are the total luminosity in the
    line and the velocity shift of the line with respect to the rest-frame position of the Lyα
    line. */
class LineLyaSpectrum : public LyaSpectrum
{
    Q_OBJECT
    Q_CLASSINFO("Title", "an emission line spectrum")

    Q_CLASSINFO("Property", "velocityShift")
    Q_CLASSINFO("Title", "the velocity shift with respect to the rest-frame position of the Lyα line")
    Q_CLASSINFO("Quantity", "velocity")
    Q_CLASSINFO("Default", "0")

    Q_CLASSINFO("Property", "lineLuminosity")
    Q_CLASSINFO("Title", "the line luminosity")
    Q_CLASSINFO("Quantity", "bolluminosity")
    Q_CLASSINFO("MinValue", "0")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE LineLyaSpectrum();

protected:
    /** This function verifies the validity of the attribues and fills a vector with the luminosity
        at each of the grid points in the global wavelength grid. */
    void setupSelfBefore();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the velocity shift of the line. */
    Q_INVOKABLE void setVelocityShift(double value);

    /** Returns the velocity shift of the line. */
    Q_INVOKABLE double velocityShift() const;

    /** Sets the line luminosity. */
    Q_INVOKABLE void setLineLuminosity(double value);

    /** Returns the line luminosity. */
    Q_INVOKABLE double lineLuminosity() const;

    //======================== Data Members ========================

private:
    double _v;
    double _L;
};

////////////////////////////////////////////////////////////////////

#endif // LINELYASPECTRUM_HPP
