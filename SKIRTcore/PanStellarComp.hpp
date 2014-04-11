/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef PANSTELLARCOMP_HPP
#define PANSTELLARCOMP_HPP

#include "StellarComp.hpp"
class StellarCompNormalization;
class StellarSED;

//////////////////////////////////////////////////////////////////////

/** The PanStellarComp class represents a stellar component in a panchromatic simulation. It uses a
    spectral energy distribution (an instance of SED class) and a normalization method (an instance
    of the StellarCompNormalization class) to calculate the luminosity vector maintained by the
    StellarComp base class. */
class PanStellarComp : public StellarComp
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a stellar component in a panchromatic simulation")
    Q_CLASSINFO("AllowedIf", "PanMonteCarloSimulation")

    Q_CLASSINFO("Property", "sed")
    Q_CLASSINFO("Title", "the spectral energy distribution for the stellar component")

    Q_CLASSINFO("Property", "normalization")
    Q_CLASSINFO("Title", "the type of normalization for the stellar component")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE PanStellarComp();

protected:
    /** This function verifies the attribute values. */
    void setupSelfBefore();

    /** This function calculates the luminosity vector maintained by the StellarComp base class
        using the spectral energy distribution and normalization method provided as attributes. */
    void setupSelfAfter();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the spectral energy distribution for the stellar component. */
    Q_INVOKABLE void setSed(StellarSED* value);

    /** Returns the spectral energy distribution for the stellar component. */
    Q_INVOKABLE StellarSED* sed() const;

    /** Sets the type of normalization for the stellar component. */
    Q_INVOKABLE void setNormalization(StellarCompNormalization* value);

    /** Returns the type of normalization for the stellar component. */
    Q_INVOKABLE StellarCompNormalization* normalization() const;

    //======================== Data Members ========================

private:
    StellarSED* _sed;
    StellarCompNormalization* _norm;
};

//////////////////////////////////////////////////////////////////////

#endif // PANSTELLARCOMP_HPP
