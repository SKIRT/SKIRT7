/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef PANMONTECARLOSIMULATION_HPP
#define PANMONTECARLOSIMULATION_HPP

#include "Array.hpp"
#include "MonteCarloSimulation.hpp"
class PanDustSystem;
class PanWavelengthGrid;

//////////////////////////////////////////////////////////////////////

/** This is a subclass of the general MonteCarloSimulation class representing a panchromatic Monte
    Carlo simulation, i.e. operating at a range of wavelengths. In such simulations, there can be
    absorption, scattering and thermal emission by dust grains. */
class PanMonteCarloSimulation : public MonteCarloSimulation
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a panchromatic Monte Carlo simulation")

    Q_CLASSINFO("Property", "wavelengthGrid")
    Q_CLASSINFO("Title", "the wavelength grid")
    Q_CLASSINFO("Default", "PanWavelengthGrid")

    Q_CLASSINFO("Property", "stellarSystem")
    Q_CLASSINFO("Title", "the stellar system")
    Q_CLASSINFO("Default", "CompStellarSystem")

    Q_CLASSINFO("Property", "dustSystem")
    Q_CLASSINFO("Title", "the dust system")
    Q_CLASSINFO("Optional", "true")
    Q_CLASSINFO("Default", "PanDustSystem")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE PanMonteCarloSimulation();

protected:
    /** This function performs some basic initialization. */
    void setupSelfAfter();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the wavelength grid for this simulation. */
    Q_INVOKABLE void setWavelengthGrid(PanWavelengthGrid* value);

    /** Returns the wavelength grid for this simulation. */
    Q_INVOKABLE PanWavelengthGrid* wavelengthGrid() const;

    /** Sets the stellar system for this simulation. */
    Q_INVOKABLE void setStellarSystem(StellarSystem* value);

    /** Returns the stellar system for this simulation. */
    Q_INVOKABLE StellarSystem* stellarSystem() const;

    /** Sets the optional dust system for this simulation. */
    Q_INVOKABLE void setDustSystem(PanDustSystem* value);

    /** Returns the dust system for this simulation, or null if there is no dust. */
    Q_INVOKABLE PanDustSystem* dustSystem() const;

    //======================== Other Functions =======================

protected:
    /** This function actually runs the simulation. For a panchromatic simulation, this includes
        the stellar emission phase, the dust self-absorption phase, and the dust emission phase
        (plus writing the results). */
    void runSelf();

private:
    /** This function drives the dust self-absorption phase in a panchromatic Monte Carlo
        simulation. This function consists of a big loop, which represents the different cycles of
        the dust self-absorption phase. This outer loop, and the function, terminates when either
        the maximum number of dust self-absorption iterations has been reached (the hardcoded value
        is 100), or when the total luminosity absorbed by the dust is stable and does not change by
        more than 0.5% compared to the previous cycle. Within every cycle, the first task is to
        construct the dust emission library that describes the spectral properties of the dust
        emission. Subsequently the dust self-absorption phase implements a parallelized loop that
        iterates over \f$N_{\text{pp}}\times N_\lambda\f$ monochromatic photon packages. Within
        this loop, the function simulates the life cycle of a single dust photon package. Before we
        can start the life cycle of the photon packages at a given wavelength index \f$\ell\f$, we
        first have to determine the total luminosity that is emitted from every dust cell at that
        wavelength index. This is just the product of the total luminosity absorbed in the
        \f$m\f$'th dust cell, \f$L^{\text{abs}}_m\f$, and the normalized SED at wavelength index
        \f$\ell\f$ corresponding to that cell, as obtained from the dust emission library. Once we
        know the luminosity \f$L_{\ell,m}\f$ emitted by each dust cell, we calculate the total dust
        luminosity, \f[ L_\ell = \sum_{m=0}^{N_{\text{cells}}-1} L_{\ell,m}, \f] and create a
        vector \f$X_m\f$ that describes the normalized cumulative luminosity distribution as a
        function of the cell number \f$m\f$, \f[ X_m = \frac{ \sum_{m'=0}^m L_{\ell,m'} }{ L_\ell
        }. \f] This vector is used to generate random dust cells from which photon packages can be
        launched. Now the actual dust self-absorption can start, i.e. we launch \f$N_{\text{pp}}\f$
        different photon packages at wavelength index \f$\ell\f$, with the original position chosen
        as a random position in the cell \f$m\f$ chosen randomly from the cumulative luminosity
        distribution \f$X_m\f$. The remaining life cycle of a photon package in the dust emission
        phase is very similar to the life cycle described in
        MonteCarloSimulation::runstellaremission(). */
    void rundustselfabsorption();

    /** This function implements the loop body for rundustselfabsorption(). */
    void dodustselfabsorptionchunk(size_t index);

    /** This function drives the dust emission phase in a panchromatic Monte Carlo simulation. The
        first task is to construct the dust emission library that describes the spectral properties
        of the dust emission. Subsequently the dust emission phase implements a parallelized loop
        that iterates over \f$N_{\text{pp}}\times N_\lambda\f$ monochromatic photon packages.
        Within this loop, the function simulates the life cycle of a single dust photon
        package. Before we can start the life cycle of the photon packages at a given wavelength
        index \f$\ell\f$, we first have to determine the total luminosity that is emitted from
        every dust cell at that wavelength index. This is just the product of the total luminosity
        absorbed in the \f$m\f$'th dust cell, \f$L^{\text{abs}}_m\f$, and the normalized SED at
        wavelength index \f$\ell\f$ corresponding to that cell, as obtained from the dust emission
        library. Once we know the luminosity \f$L_{\ell,m}\f$ emitted by each dust cell, we
        calculate the total dust luminosity, \f[ L_\ell = \sum_{m=0}^{N_{\text{cells}}-1}
        L_{\ell,m}, \f] and create a vector \f$X_m\f$ that describes the normalized cumulative
        luminosity distribution as a function of the cell number \f$m\f$, \f[ X_m = \frac{
        \sum_{m'=0}^m L_{\ell,m'} }{ L_\ell }. \f] This vector is used to generate random dust
        cells from which photon packages can be launched. Now the actual dust emission can start,
        i.e. we launch \f$N_{\text{pp}}\f$ different photon packages at wavelength index
        \f$\ell\f$, with the original position chosen as a random position in the cell \f$m\f$
        chosen randomly from the cumulative luminosity distribution \f$X_m\f$. The remaining life
        cycle of a photon package in the dust emission phase is very similar to the life cycle
        described in MonteCarloSimulation::runstellaremission(). */
    void rundustemission();

    /** This function implements the loop body for rundustemission(). */
    void dodustemissionchunk(size_t index);

    //======================== Data Members ========================

private:
    PanDustSystem* _pds;   // copy of _ds (pointer to the dust system) with the "pan" subtype

    // data members used to communicate between rundustXXX() and the corresponding parallel loop
    int _Ncells;           // number of dust cells
    Array _Labsbolv;       // vector that contains the bolometric absorbed luminosity in each cell
};

////////////////////////////////////////////////////////////////////

#endif // PANMONTECARLOSIMULATION_HPP
