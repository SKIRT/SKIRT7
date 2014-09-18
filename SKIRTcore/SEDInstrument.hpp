/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef SEDINSTRUMENT_HPP
#define SEDINSTRUMENT_HPP

#include "DistantInstrument.hpp"

////////////////////////////////////////////////////////////////////

/** An SEDInstrument object represents a basic instrument that just records the total integrated
    flux, and outputs this as an SED file. Internally, the class contains a single 1D vector (the
    F-vector) that stores the total integrated flux at every wavelength index. */
class SEDInstrument : public DistantInstrument
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a basic instrument that outputs the total integrated flux as an SED")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor */
    Q_INVOKABLE SEDInstrument();

protected:
    /** This function completes setup for this instrument. */
    void setupSelfBefore();

    //======================== Other Functions =======================

protected:
    /** This function simulates the detection of a photon package by the instrument.
        See SimpleInstrument::detect() for more information. */
    void detect(PhotonPackage* pp);

    /** This function calibrates and outputs the instrument data.
        See SimpleInstrument::write() for more information. */
    void write();

    //======================== Data Members ========================

private:
    Array _Ftotv;
};

////////////////////////////////////////////////////////////////////

#endif // SEDINSTRUMENT_HPP
