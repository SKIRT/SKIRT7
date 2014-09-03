/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef MULTIFRAMEINSTRUMENT_HPP
#define MULTIFRAMEINSTRUMENT_HPP

#include "DistantInstrument.hpp"
class InstrumentFrame;

////////////////////////////////////////////////////////////////////

/** MultiFrameInstrument is a specialty instrument class for use with oligochromatic simulations in
    combination with an external tool such as FitSKIRT. It is similar to the FrameInstrument class
    in the sense that each pixel stores the incoming total flux per wavelength. However,
    MultiFrameInstrument allows a different frame extent and/or pixel resolution at each
    wavelength. All frames do share the direction and position angles determined by the properties
    of the DistantInstrument base class. It is assumed that the distance to the system is
    sufficiently large so that parallel projection can be used. */

class MultiFrameInstrument : public DistantInstrument
{
    Q_OBJECT
    Q_CLASSINFO("Title", "an instrument with a different frame per wavelength (for use with FitSKIRT)")
    Q_CLASSINFO("AllowedIf", "OligoMonteCarloSimulation")

    Q_CLASSINFO("Property", "writeTotal")
    Q_CLASSINFO("Title", "output the total flux")
    Q_CLASSINFO("Default", "yes")

    Q_CLASSINFO("Property", "writeStellarComps")
    Q_CLASSINFO("Title", "output the flux emitted from each stellar component seperately")
    Q_CLASSINFO("Default", "no")

    Q_CLASSINFO("Property", "frames")
    Q_CLASSINFO("Title", "the instrument frames (one for each wavelength)")
    Q_CLASSINFO("Default", "InstrumentFrame")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE MultiFrameInstrument();

protected:
    /** This function verifies that all attribute values have been appropriately set and performs
        setup for the instrument. */
    void setupSelfBefore();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the flag that indicates whether or not to output the total flux. The default value is
        true. */
    Q_INVOKABLE void setWriteTotal(bool value);

    /** Returns the flag that indicates whether or not to output the total flux. */
    Q_INVOKABLE bool writeTotal() const;

    /** Sets the flag that indicates whether or not to output the flux emitted from each stellar
        component in a seperate file. The default value is false. */
    Q_INVOKABLE void setWriteStellarComps(bool value);

    /** Returns the flag that indicates whether or not to output the flux emitted from each stellar
        component in a seperate file. */
    Q_INVOKABLE bool writeStellarComps() const;

    /** This function adds an instrument frame to the multi-frame instrument. There must be exactly
        one frame for each wavelength in the oligochromatic simulation's wavelength grid. The frames
        are assumed to be in the same order as the wavelengths. */
    Q_INVOKABLE void addFrame(InstrumentFrame* value);

    /** This function returns the list of instruments in the instrument system. */
    Q_INVOKABLE QList<InstrumentFrame*> frames() const;

    //======================== Other Functions =======================

    /** This function simulates the detection of a photon package by the instrument. It operates
        similarly to SimpleInstrument::detect(), except that the photon packages for different
        wavelengths are handed to different instrument frames. */
    void detect(PhotonPackage* pp);

    /** This function calibrates and outputs the instrument data. It operates similarly to
        SimpleInstrument::write(), except that a separate output file is written for each
        wavelength, using filenames that include the wavelength index \f$\ell\f$. */
    void write();

    //======================== Data Members ========================

private:
    // discoverable attributes
    bool _writeTotal;
    bool _writeStellarComps;
    QList<InstrumentFrame*> _frames;
};

////////////////////////////////////////////////////////////////////

#endif // MULTIFRAMEINSTRUMENT_HPP
