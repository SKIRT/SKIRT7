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
    /** This function verifies the validity of the property values. */
    void setupSelfBefore();

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
    /** This function drives the stellar emission phase in a panchromatic Monte Carlo simulation.
        It consists of two nested loops: an outer loop that loops over all wavelengths, and an
        inner loop that iterates over different monochromatic photons packages corresponding to a
        single wavelength. The outer loop (over wavelengths) is parallellized. Within the nested
        loops, the function simulates the life cycle of a single stellar photon package. Each
        photon package is born when it is emitted by the stellar system. Immediately after birth,
        peel-off photon packages are created and launched towards the instruments (one for each
        instrument). If there is dust in the system (there usually is...), the photon package now
        enters a cycle which consists of different steps. First, all details of the path of photon
        package through the dust system are calculated and stored. Based on this information, the
        escape and absorption of a fraction of the luminosity of the photon package is simulated.
        When the photon package is still luminous enough, it is propagated to a new scattering
        position, peel-off photon packages are created and launched towards each instrument, and
        the actual scattering is simulated. Then again, the details of the path of photon package
        through the dust system are calculated and stored, and the loop repeats itself. It is
        terminated only when the photon package has lost a very substantial part of its original
        luminosity (and hence becomes irrelevant). */
    void runstellaremission();

    /** This function is used by runstellaremission() as the body of a parallel loop to perform
        stellar emission for a single wavelength. */
    void do_stellaremission_wavelength(size_t ell);

    /** This function drives the dust self-absorption phase in a panchromatic Monte Carlo
        simulation. This function consists of a big loop, which represents the different cycles of
        the dust self-absorption phase. This outer loop, and the function, terminates when either
        the maximum number of dust self-absorption iterations has been reached (the hardcoded value
        is 100), or when the total luminosity absorbed by the dust is stable and does not change by
        more than 1% compared to the previous cycle. Within every cycle, the first task is to
        construct the dust emission library (an object of the DustEmissionLibrary class) that
        describes the spectral properties of the dust emission. In principle, we should construct a
        separate dust emission library for every dust population in the dust system, but this still
        needs to be implemented. Once the dust emission library is constructed, the dust
        self-absorption phase consists of two nested loops: an outer loop that loops over all
        wavelengths, and an inner loop that iterates over different monochromatic photons packages
        corresponding to a single wavelength. The outer loop (over wavelengths) is parallellized.
        Within the nested loops, the function simulates the life cycle of a single dust photon
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
        cells from which photon packages can be launched. Now the actual dust self-absorption can
        start, i.e. we launch \f$N_{\text{pp}}\f$ different photon packages at wavelength index
        \f$\ell\f$, with the original position chosen as a random position in the cell \f$m\f$
        chosen randomly from the cumulative luminosity distribution \f$X_m\f$. As we assume that
        there is no scattering for the dust photon packages (an option that could be changed if
        desired) and we don't need to consider peel-off (the dust self-absorption phase is just to
        calculate the internal equilibrium, not to calculate the emergent radiation field), the
        life cycle of a photon in the dust emission phase is extremely simple. First, all details
        of the path of photon package through the dust system are calculated and stored. Based on
        this information, the escape and absorption of a fraction of the luminosity of the photon
        package (i.e. the self-absorption) is simulated. */
    void rundustselfabsorption();

    /** This function is used by rundustselfabsorption() as the body of a parallel loop to perform
        dust self absorption for a single wavelength. */
    void do_dustselfabsorption_wavelength(size_t ell);

    /** This function drives the dust emission phase in a panchromatic Monte Carlo simulation. The
        first task is to construct the dust emission library (an object of the DustEmissionLibrary
        class) that describes the spectral properties of the dust emission. In principle, we should
        construct a separate dust emission library for every dust population in the dust system,
        but this still needs to be implemented. Once the dust emission library is constructed, the
        dust emission phase consists of two nested loops: an outer loop that loops over all
        wavelengths, and an inner loop that iterates over different monochromatic photons packages
        corresponding to a single wavelength. The outer loop (over wavelengths) is parallellized.
        Within the nested loops, the function simulates the life cycle of a single dust photon
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
        chosen randomly from the cumulative luminosity distribution \f$X_m\f$. As we assume that
        there is no scattering for the dust photon packages (an option that could be changed if
        desired), the life cycle of a photon in the dust emission phase is extremely simple: it
        just consists of creating peel-off photon packages and launching them towards the
        instruments (one for each instrument). */
    void rundustemission();

    /** This function is used by rundustemission() as the body of a parallel loop to perform
        dust emission for a single wavelength. */
    void do_dustemission_wavelength(size_t ell);

    /** This function initializes the progress counters used in logprogress(). */
    void initprogress();

    /** This function logs a progress message for the specified phase, wavelength and photon
        package index. */
    void logprogress(QString phase, int ell, qint64 index);

    //======================== Data Members ========================

private:
    PanDustSystem* _pds;   // copy of _ds (pointer to the dust system) with the "pan" subtype
    qint64 _Npp;           // the number of photon packages per wavelength
    qint64 _Nlog;          // a message is logged when this number of photon packages was processed for a wavelength
    Array _Ndone;          // the number of photon packages already launched for each wavelength

    // data member used to communicate between rundustselfabsorption() and its parallel loop
    int _cycle;            // number of dust self-absorption cycles
    Array _Labsbolv;       // vector that contains the bolometric absorbed luminosity in all cells
};

////////////////////////////////////////////////////////////////////

#endif // PANMONTECARLOSIMULATION_HPP
