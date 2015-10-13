/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef LYASTELLARCOMP_HPP
#define LYASTELLARCOMP_HPP

#include "GeometricStellarComp.hpp"
#include "LyaSpectrum.hpp"

//////////////////////////////////////////////////////////////////////

/** The LyaStellarComp class is a subclass of the GeometricStellarComp class and represents a Lyα
    emitting component that uses a built-in geometry in a Lyα simulation. The velocity spectrum of
    the emission can be chosen to be either line emission at a specified velocity, or a flat
    continuum spectrum over the entire velocity range. */
class LyaStellarComp : public GeometricStellarComp
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a Lyα component with a built-in geometry")
    Q_CLASSINFO("AllowedIf", "LyaMonteCarloSimulation")

    Q_CLASSINFO("Property", "spectrum")
    Q_CLASSINFO("Title", "the emission spectrum of the component")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE LyaStellarComp();

protected:
    /** This function verifies the attribute values. */
    void setupSelfBefore();

    /** This function calculates the luminosity vector maintained by the GeometricStellarComp base
        class using the emission spectrum provided as attribute. */
    void setupSelfAfter();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the Lyα emission spectrum for the stellar component. */
    Q_INVOKABLE void setSpectrum(LyaSpectrum* value);

    /** Returns the Lyα emission spectrum for the stellar component. */
    Q_INVOKABLE LyaSpectrum* spectrum() const;

    //======================== Data Members ========================

private:
    LyaSpectrum* _spectrum;
};

//////////////////////////////////////////////////////////////////////

#endif // LYASTELLARCOMP_HPP
