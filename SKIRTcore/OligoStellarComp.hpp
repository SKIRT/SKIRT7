/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef OLIGOSTELLARCOMP_HPP
#define OLIGOSTELLARCOMP_HPP

#include "StellarComp.hpp"

//////////////////////////////////////////////////////////////////////

/** The OligoStellarComp class represents a stellar component in an oligochromatic simulation.
    Specifically this means that the spectral energy distribution over the small number of
    wavelengths is managed internally rather than through the SED class. */
class OligoStellarComp : public StellarComp
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a stellar component in an oligochromatic simulation")
    Q_CLASSINFO("AllowedIf", "OligoMonteCarloSimulation")

    Q_CLASSINFO("Property", "luminosities")
    Q_CLASSINFO("Title", "the luminosities, one for each wavelength, in solar units")
    Q_CLASSINFO("MinValue", "0")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE OligoStellarComp();

protected:
    /** This function converts the luminosity input values to program units using the Sun's SED,
        and copies the result into the vector maintained by the StellarComp base class. */
    void setupSelfBefore();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the list of luminosities, one for each wavelength in the simulation's wavelength grid.
        The list should have the same length as the simulation's wavelength grid. */
    Q_INVOKABLE void setLuminosities(QList<double> value);

    /** Returns the list of luminosities, one for each wavelength in the simulation's wavelength
        grid. */
    Q_INVOKABLE QList<double> luminosities() const;

    //======================== Data Members ========================

private:
    QList<double> _luminosities;
};

//////////////////////////////////////////////////////////////////////

#endif // OLIGOSTELLARCOMP_HPP
