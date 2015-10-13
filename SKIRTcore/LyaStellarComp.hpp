/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef LYASTELLARCOMP_HPP
#define LYASTELLARCOMP_HPP

#include "GeometricStellarComp.hpp"
#include "LyaSpectrum.hpp"

//////////////////////////////////////////////////////////////////////

/** The LyaStellarComp class is a subclass of the GeometricStellarComp class and represents a
    component that radiates the primary emission that can be used in a Lyα simulation. The geometry
    (i.e. the spatial location of the sources) and the spectral distribution (the amount of
    radiation emitted as a function of the velocity or wavelength) of the component can be set
    independently. */
class LyaStellarComp : public GeometricStellarComp
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a component with a built-in geometry (to be used in a Lyα simulation)")
    Q_CLASSINFO("AllowedIf", "LyaMonteCarloSimulation")

    Q_CLASSINFO("Property", "spectrum")
    Q_CLASSINFO("Title", "the emission spectrum")

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
