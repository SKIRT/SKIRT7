/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

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

    /** This function performs some basic initialization. */
    void setupSelfBefore();

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

private:
    /** This function drives the stellar emission phase in an oligochromatic Monte Carlo simulation.
        It consists of a parallelized loop that iterates over \f$N_{\text{pp}}\f$ different
        monochromatic photons packages. Within this loop, the function simulates the life cycle of a
        single stellar photon package. Each photon package is born when it is emitted by the
        stellar system. Immediately after birth, peel-off photon packages are created and launched
        towards the instruments (one for each instrument). If there is dust in the system (there
        usually is...), the photon package now enters a cycle which consists of different steps.
        First, all details of the path of photon package through the dust system are calculated and
        stored. Based on this information, the escape and absorption of a fraction of the
        luminosity of the photon package is simulated. When the photon package is still luminous
        enough, it is propagated to a new scattering position, peel-off photon packages are created
        and launched towards each instrument, and the actual scattering is simulated. Then again,
        the details of the path of photon package through the dust system are calculated and
        stored, and the loop repeats itself. It is terminated only when the photon package has lost
        a very substantial part of its original luminosity (and hence becomes irrelevant). */
    void runstellaremission();

    /** This function implements the loop body for runstellaremission(). */
    void dostellaremissionchunk(int index);

    //======================== Data Members ========================

private:
    // a message is logged when this number of CHUNKS of photon packages was processed
    int _Nlog;

    // variables shared between runstellaremission() and dostellaremissionchunk()
    int _ell;
    double _L;
    double _Lmin;
};

////////////////////////////////////////////////////////////////////

#endif // OLIGOMONTECARLOSIMULATION_HPP
