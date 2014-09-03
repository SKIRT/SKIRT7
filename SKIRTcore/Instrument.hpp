/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef INSTRUMENT_HPP
#define INSTRUMENT_HPP

#include <cfloat>
#include <vector>
#include "Direction.hpp"
#include "Position.hpp"
#include "SimulationItem.hpp"
class DustSystem;
class PhotonPackage;

////////////////////////////////////////////////////////////////////

/** Instrument is an abstract class representing instruments to collect the photon packages during
    a Monte Carlo simulation. Various subclasses implement different instrument types, including
    spectrometers, simple CCD frames or full integral field spectographs. Each subclass is also
    responsible for the transformation from world coordinates to instrument coordinates, allowing
    various perspective schemes in different subclasses. This top-level abstract class offers a
    generic interface for receiving photon packages from the simulation, and for appropriately
    locking the instrument's data structure when photon packages may arrive in parallel. */
class Instrument : public SimulationItem
{
    Q_OBJECT
    Q_CLASSINFO("Title", "an instrument")

    Q_CLASSINFO("Property", "instrumentName")
    Q_CLASSINFO("Title", "the name for this instrument")

    //============= Construction - Setup - Destruction =============

protected:
    /** The default constructor; it is protected since this is an abstract class. */
    Instrument();

    /** This function performs setup for the instrument. */
    void setupSelfBefore();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the instrument name used as part of output filenames to differentiate between
        instruments. */
    Q_INVOKABLE void setInstrumentName(QString value);

    /** Returns the instrument name. */
    Q_INVOKABLE QString instrumentName() const;

    //======================== Other Functions =======================

public:
    /** Returns the direction towards the observer, given the photon package's launching position.
        The implementation must be provided in a subclass. */
    virtual Direction bfkobs(const Position& bfr) const = 0;

    /** This function simulates the detection of a photon package by the instrument. Its
        implementation must be provided in a subclass. The implementation must call the record()
        function to actually update the instrument's data structure, so that appropriate locking
        can be provided. */
    virtual void detect(PhotonPackage* pp) = 0;

    /** This function calibrates the instrument and writes down the entire contents to a set of
        files. Its implementation must be provided in a subclass. */
    virtual void write() = 0;

    /** This function is provided for use in subclasses. It calculates and returns the optical
        depth over the specified distance along the current path of the specified photon package,
        at the photon package's wavelength. If the distance is not specified, the complete path is
        taken into account. */
    double opticalDepth(PhotonPackage* pp, double distance=DBL_MAX) const;

    //======================== Data Members ========================

protected:
    // discoverable attributes of a generic instrument
    QString _instrumentname;

private:
    // other data members
    DustSystem* _ds;   // cached pointer to dust system to call opticalDepth() function
};

////////////////////////////////////////////////////////////////////

#endif // INSTRUMENT_HPP
