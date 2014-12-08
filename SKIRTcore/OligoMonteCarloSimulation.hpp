/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef OLIGOMONTECARLOSIMULATION_HPP
#define OLIGOMONTECARLOSIMULATION_HPP

#include "MonteCarloSimulation.hpp"
class OligoDustSystem;
class OligoWavelengthGrid;

//////////////////////////////////////////////////////////////////////

/** This is a subclass of the general MonteCarloSimulation class representing an oligochromatic
    Monte Carlo simulation, i.e. operating at one or more distinct wavelengths rather than a
    discretized range of wavelengths. In such simulations there can be absorption and scattering by
    dust grains, but by definition there is no thermal dust emission. */
class OligoMonteCarloSimulation : public MonteCarloSimulation
{
    Q_OBJECT
    Q_CLASSINFO("Title", "an oligochromatic Monte Carlo simulation")

    Q_CLASSINFO("Property", "wavelengthGrid")
    Q_CLASSINFO("Title", "the wavelength grid")
    Q_CLASSINFO("Default", "OligoWavelengthGrid")

    Q_CLASSINFO("Property", "stellarSystem")
    Q_CLASSINFO("Title", "the stellar system")
    Q_CLASSINFO("Default", "StellarSystem")

    Q_CLASSINFO("Property", "dustSystem")
    Q_CLASSINFO("Title", "the dust system")
    Q_CLASSINFO("Optional", "true")
    Q_CLASSINFO("Default", "OligoDustSystem")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE OligoMonteCarloSimulation();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the wavelength grid for this simulation. */
    Q_INVOKABLE void setWavelengthGrid(OligoWavelengthGrid* value);

    /** Returns the wavelength grid for this simulation. */
    Q_INVOKABLE OligoWavelengthGrid* wavelengthGrid() const;

    /** Sets the stellar system for this simulation. */
    Q_INVOKABLE void setStellarSystem(StellarSystem* value);

    /** Returns the stellar system for this simulation. */
    Q_INVOKABLE StellarSystem* stellarSystem() const;

    /** Sets the optional dust system for this simulation. */
    Q_INVOKABLE void setDustSystem(OligoDustSystem* value);

    /** Returns the dust system for this simulation, or null if there is no dust. */
    Q_INVOKABLE OligoDustSystem* dustSystem() const;

    //======================== Other Functions =======================

protected:
    /** This function actually runs the simulation. For an oligochromatic simulation, this just
        includes the stellar emission phase (plus writing the results). */
    void runSelf();
};

////////////////////////////////////////////////////////////////////

#endif // OLIGOMONTECARLOSIMULATION_HPP
