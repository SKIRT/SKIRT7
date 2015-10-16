/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef LYAMONTECARLOSIMULATION_HPP
#define LYAMONTECARLOSIMULATION_HPP

#include "MonteCarloSimulation.hpp"
class HISystem;
class DustSystem;
class LyaWavelengthGrid;

//////////////////////////////////////////////////////////////////////

/** This is a subclass of the general MonteCarloSimulation class representing a Lyα
    Monte Carlo simulation. */
class LyaMonteCarloSimulation : public MonteCarloSimulation
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a Lyα Monte Carlo simulation")

    Q_CLASSINFO("Property", "wavelengthGrid")
    Q_CLASSINFO("Title", "the wavelength grid")
    Q_CLASSINFO("Default", "LyaWavelengthGrid")

    Q_CLASSINFO("Property", "stellarSystem")
    Q_CLASSINFO("Title", "the 'stellar' system")
    Q_CLASSINFO("Default", "StellarSystem")

    Q_CLASSINFO("Property", "hISystem")
    Q_CLASSINFO("Title", "the neutral hydrogen system")
    Q_CLASSINFO("Optional", "true")
//    Q_CLASSINFO("Default", "DustSystem")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE LyaMonteCarloSimulation();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the wavelength grid for this simulation. */
    Q_INVOKABLE void setWavelengthGrid(LyaWavelengthGrid* value);

    /** Returns the wavelength grid for this simulation. */
    Q_INVOKABLE LyaWavelengthGrid* wavelengthGrid() const;

    /** Sets the stellar system for this simulation. */
    Q_INVOKABLE void setStellarSystem(StellarSystem* value);

    /** Returns the stellar system for this simulation. */
    Q_INVOKABLE StellarSystem* stellarSystem() const;

    /** Sets the optional neutral hydrogen system for this simulation. */
    Q_INVOKABLE void setHISystem(HISystem* value);

    /** Returns the neutral hydrogen system for this simulation, or null if there is no gas. */
    Q_INVOKABLE HISystem* hISystem() const;

    //======================== Other Functions =======================

protected:
    /** This function actually runs the simulation. For a Lyα simulation, this just
        includes the stellar emission phase (plus writing the results). */
    void runSelf();

    //======================== Data Members ========================

private:
    // discoverable attributes
    HISystem* _his;
};

////////////////////////////////////////////////////////////////////

#endif // LYAMONTECARLOSIMULATION_HPP
