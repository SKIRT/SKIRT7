/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef FRAMEINSTRUMENT_HPP
#define FRAMEINSTRUMENT_HPP

#include "ParallelDataCube.hpp"
#include "SingleFrameInstrument.hpp"

////////////////////////////////////////////////////////////////////

/** A FrameInstrument object represents a basic instrument that just records the total flux in
    every pixel, and outputs this as a data cube in a FITS file. Internally, the class contains a
    single 3D vector (the f-vector) corresponding to the surface brightness in every pixel, at
    every wavelength index. */
class FrameInstrument : public SingleFrameInstrument
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a basic instrument that outputs the total flux in every pixel as a data cube")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor */
    Q_INVOKABLE FrameInstrument();

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
    Array _ftotv;
    ParallelDataCube _distftotv;
};

////////////////////////////////////////////////////////////////////

#endif // FRAMEINSTRUMENT_HPP
