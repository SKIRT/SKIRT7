/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef PANDUSTSYSTEM_HPP
#define PANDUSTSYSTEM_HPP

#include "ParallelTable.hpp"
#include "DustSystem.hpp"

class DustEmissivity;
class DustLib;
class ProcessAssigner;

//////////////////////////////////////////////////////////////////////

/** A PanDustSystem class object represents a complete dust system for use with panchromatic
    simulations. This class relies on the functionality implemented in the DustSystem base class,
    and additionaly supports dust emission. It maintains information on the absorbed energy for
    each cell at each wavelength in a (potentially very large) table. It also holds a
    DustEmissivity object and a DustLib object used to calculate the dust emission spectrum for
    dust cells. */
class PanDustSystem : public DustSystem
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a dust system for use with panchromatic simulations")

    Q_CLASSINFO("Property", "dustEmissivity")
    Q_CLASSINFO("Title", "the dust emissivity type")
    Q_CLASSINFO("Optional", "true")
    Q_CLASSINFO("Default", "GreyBodyDustEmissivity")

    Q_CLASSINFO("Property", "dustLib")
    Q_CLASSINFO("Title", "the dust library mechanism")
    Q_CLASSINFO("Default", "AllCellsDustLib")
    Q_CLASSINFO("RelevantIf", "dustEmissivity")

    Q_CLASSINFO("Property", "emissionBias")
    Q_CLASSINFO("Title", "the dust emission bias")
    Q_CLASSINFO("MinValue", "0")
    Q_CLASSINFO("MaxValue", "1")
    Q_CLASSINFO("Default", "0.5")
    Q_CLASSINFO("Silent", "true")
    Q_CLASSINFO("RelevantIf", "dustEmissivity")

    Q_CLASSINFO("Property", "emissionBoost")
    Q_CLASSINFO("Title", "the factor by which to boost the number of dust emission photon packages")
    Q_CLASSINFO("MinValue", "0")
    Q_CLASSINFO("MaxValue", "1000")
    Q_CLASSINFO("Default", "1")
    Q_CLASSINFO("Silent", "true")
    Q_CLASSINFO("RelevantIf", "dustEmissivity")

    Q_CLASSINFO("Property", "selfAbsorption")
    Q_CLASSINFO("Title", "include dust self-absorption")
    Q_CLASSINFO("Default", "no")
    Q_CLASSINFO("RelevantIf", "dustEmissivity")

    Q_CLASSINFO("Property", "cycles")
    Q_CLASSINFO("Title", "the number of cycles in each dust self-absorption stage")
    Q_CLASSINFO("Optional", "true")
    Q_CLASSINFO("Silent", "true")
    Q_CLASSINFO("Minimum", "0")
    Q_CLASSINFO("Default", "0")
    Q_CLASSINFO("RelevantIf", "selfAbsorption")

    Q_CLASSINFO("Property", "writeEmissivity")
    Q_CLASSINFO("Title", "output a file with the dust mix emissivities in the local ISRF")
    Q_CLASSINFO("Default", "no")
    Q_CLASSINFO("Silent", "true")
    Q_CLASSINFO("RelevantIf", "dustEmissivity")

    Q_CLASSINFO("Property", "writeTemperature")
    Q_CLASSINFO("Title", "output FITS files displaying the dust temperature distribution")
    Q_CLASSINFO("Default", "yes")
    Q_CLASSINFO("RelevantIf", "dustEmissivity")

    Q_CLASSINFO("Property", "writeISRF")
    Q_CLASSINFO("Title", "output a data file describing the interstellar radiation field")
    Q_CLASSINFO("Default", "no")
    Q_CLASSINFO("RelevantIf", "dustEmissivity")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE PanDustSystem();

protected:
    /** This function does some basic initialization. */
    void setupSelfBefore();

    /** If the relevant flag is turned on, this function outputs a data file tabulating the
        emissivity for each dust component's dust mix, assuming the dust would be embedded in the
        local (i.e. solar neighborhood) interstellar radiation field as defined by Mathis et al.
        (1983, A&A, 128, 212). */
    void setupSelfAfter();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the optional dust emissivity object for this dust system. */
    Q_INVOKABLE void setDustEmissivity(DustEmissivity* value);

    /** Returns the dust emissivity object for this dust system, or null if there is no dust
        emission. */
    Q_INVOKABLE DustEmissivity* dustEmissivity() const;

    /** Sets the dust library object for this dust system. */
    Q_INVOKABLE void setDustLib(DustLib* value);

    /** Returns the dust library object for this dust system, or null if there is no dust emission.
        */
    Q_INVOKABLE DustLib* dustLib() const;

    /** Sets the emission bias, i.e. the fraction of dust cells selected for emission from a
        uniform distribution rather than a distribution weighted according to the total dust
        luminosity of the cells. */
    Q_INVOKABLE void setEmissionBias(double value);

    /** Returns the emission bias. */
    Q_INVOKABLE double emissionBias() const;

    /** Sets the multiplication factor by which to boost the number of photon packages sent during
        the dust emission phase. The default value is 1, i.e. use the same number of photon
        packages as during the stellar emission phase. A higher value increases the resolution of
        infrared images at the cost of extra run-time. If dust emission is turned off, the boost
        value is irrelevant. */
    Q_INVOKABLE void setEmissionBoost(double value);

    /** Returns the multiplication factor by which to boost the number of photon packages sent
        during the dust emission phase. If dust emission is turned off, this function returns 1. */
    Q_INVOKABLE double emissionBoost() const;

    /** Sets the flag indicating whether to include dust self-absorption. The default value is
        true. If dust emission is turned off, the value of this flag is irrelevant. */
    Q_INVOKABLE void setSelfAbsorption(bool value);

    /** Returns the flag indicating whether to include dust self-absorption. If dust emission is
        turned off, this function returns false. */
    Q_INVOKABLE bool selfAbsorption() const;

    /** Sets the the number of cycles in each dust self-absorption stage. */
    Q_INVOKABLE void setCycles(int value);

    /** Returns the number of cycles in each dust self-absorption stage. */
    Q_INVOKABLE int cycles() const;

    /** Sets the flag that indicates whether or not to output a file with the dust mix emissivities
        in the local ISRF. The default value is true. If dust emission is turned off, the value of
        this flag is irrelevant. */
    Q_INVOKABLE void setWriteEmissivity(bool value);

    /** Returns the flag that indicates whether or not to output a file with the dust mix
        emissivities in the local ISRF. If dust emission is turned off, this function returns
        false. */
    Q_INVOKABLE bool writeEmissivity() const;

    /** Sets the flag that indicates whether or not to output FITS files displaying the dust
        temperature distribution for further analysis. The default value is true. If dust emission
        is turned off, the value of this flag is irrelevant. */
    Q_INVOKABLE void setWriteTemperature(bool value);

    /** Returns the flag that indicates whether or not to output FITS files displaying the dust
        temperature distribution for further analysis. If dust emission is turned off, this
        function returns false. */
    Q_INVOKABLE bool writeTemperature() const;

    /** Sets the flag indicating whether to output a data file describing the interstellar
        radiation field. The default value is true. If dust emission is turned off, the value of
        this flag is irrelevant. */
    Q_INVOKABLE void setWriteISRF(bool value);

    /** Returns the flag indicating whether to output a data file describing the interstellar
        radiation field. If dust emission is turned off, this function returns false. */
    Q_INVOKABLE bool writeISRF() const;

    //======================== Other Functions =======================

public:
    /** This function returns true if dust emission is turned on for this dust system, and false
        otherwise. */
    bool dustemission() const;

    /** This function returns a flag that indicate whether the absorption rates in each cell need
        to be stored for this dust system. For a panchromatic simulation, absorption rates are only
        calculated if dust emission is turned on. */
    bool storeabsorptionrates() const;

    /** The function simulates the absorption of a monochromatic luminosity package in the dust
        cell with cell number \f$m\f$, i.e. it adds a fraction \f$\Delta L\f$ to the already
        absorbed luminosity at wavelength index \f$\ell\f$. The function adds the absorbed
        luminosity \f$\Delta L\f$ to the appropriate item in the absorption rate table for stellar
        or dust emission as indicated by the flag. The addition is performed in a thread-safe
        manner so this function may be concurrently called from multiple threads. */
    void absorb(int m, int ell, double DeltaL, bool ynstellar);

    /** This function returns the absorbed luminosity \f$L_{\ell,m}\f$ at wavelength index
        \f$\ell\f$ in the dust cell with cell number \f$m\f$. For a panchromatic dust system, it sums
        the individual absorption rate counters corresponding to the stellar and dust emission. */
    double Labs(int m, int ell) const;

    /** This function resets the absorbed dust luminosity to zero in all cells of the dust system.
        */
    void rebootLabsdust();

    /** This function returns the total (bolometric) absorbed luminosity in the dust cell with cell
        number \f$m\f$. It is calculated by summing the absorbed luminosity at all the wavelength
        indices. */
    double Labs(int m) const;

    /** This function returns a vector with the total (bolometric) absorbed luminosity in each dust cell. */
    Array Labsbolv() const;

    /** This function returns the total (bolometric) absorbed dust luminosity in the entire dust system.
        It is calculated by summing the absorbed stellar luminosity of all the cells. */
    double Labsstellartot() const;

    /** This function returns the total (bolometric) absorbed luminosity in the entire dust system.
        It is calculated by summing the absorbed dust luminosity of all the cells. */
    double Labsdusttot() const;

    /** This function (re-)calculates the relevant dust emission spectra for the dust system, based
        on the absorption data currently stored in the dust cells, and internally caches the
        results. If dust emission is turned off, this function does nothing. */
    void calculatedustemission(bool ynstellar);

    /** This function is used to sum the absorbed luminosities in the panchromatic dust system
        across the different processes in the multiprocessing environment. This function must be
        provided with a boolean argument, indicating whether the absorbed stellar luminosities (in
        _Labsstelvv) or the absorbed thermal luminosities (in _Labsdustvv) must be summed. The
        communication is performed by calling the sum_all() function of the PeerToPeerCommunicator
        object, which is found with the discovery mechanism. */
    void sumResults(bool ynstellar);

    void syncLabsTables();

    /** This function returns the luminosity \f$L_\ell\f$ at the wavelength index \f$\ell\f$ in the
        normalized dust emission SED corresponding to the dust cell with dust cell number \f$m\f$.
        It just looks up the appropriate value in the cached results produced by calculate(). If
        dust emission is turned off, the function returns zero. */
    double dustluminosity(int m, int ell) const;

    /** If the writeISRF attribute is true, this function writes out a data file (named
        <tt>prefix_ds_isrf.dat</tt>) describing the interstellar radiation field for each
        wavelength. The first line contains a list of all wavelengths in the simulation's
        wavelength grid, and followed by a blank line. Subsequently there is a line for each dust
        cell in the dust grid. Each line has the following columns: cell identifier \f$m\f$, x, y
        and z positions, mean radiation field \f$J_{\lambda,m}\f$ for every wavelength
        \f$\lambda\f$.

        If the writeTemperature attribute is true, this function writes out FITS
        files (named <tt>prefix_ds_tempXX.fits</tt>) with the mean dust temperatures in the
        coordinate planes. Each of these maps contains 1024 x 1024 pixels, and covers as a field of
        view the total extension of the grid. The number of data files written depends on the
        dimension of the dust system's geometry: for spherical symmetry only the intersection with
        the xy plane is written, for axial symmetry the intersections with the xy and xz planes are
        written, and for general geometries all three intersections are written. Each FITS file is
        a data cube, where a map is created for each dust population of each dust component. The
        mean dust temperature \f${\bar{T}}_c({\boldsymbol{r}})\f$ corresponding to the \f$c\f$'th
        population at the position \f${\boldsymbol{r}}\f$ is calculated from the mean intensity
        \f$J_\lambda({\boldsymbol{r}})\f$ of the radiation field using the balance equation \f[
        \int_0^\infty \kappa_{\lambda,c}^{\text{abs}}\, J_\lambda({\boldsymbol{r}})\,
        {\text{d}}\lambda = \int_0^\infty \kappa_{\lambda,c}^{\text{abs}}\,
        B_\lambda({\bar{T}}_c({\boldsymbol{r}}))\, {\text{d}}\lambda, \f] where
        \f$\kappa_{\lambda,c}^{\text{abs}}\f$ is the absorption coefficient of the \f$c\f$'th dust
        population. Note that this mean dust temperature is only illustrative: it does not imply
        that the dust emits as a modified blackbody at an equibrium temperature.

        Furthermore, if the writeTemperature attribute is true, this function writes out a text file
        containing the mass and indicative dust temperature for each cell in the dust grid. The
        indicative dust temperature is defined as the mean temperature (calculated as described above)
        averaged over all dust populations (weighed by mass in the dust mix) and all dust components
        (weighed by density in the dust cell). */
    void write() const;



    //======================== Data Members ========================

private:
    // data members to be set before setup is invoked
    DustEmissivity* _dustemissivity;
    DustLib* _dustlib;
    double _emissionBias;
    double _emissionBoost;
    bool _selfabsorption;
    bool _writeEmissivity;
    bool _writeTemp;
    bool _writeISRF;
    int _cycles;

    // data members initialized during setup
    int _Nlambda;
    const ProcessAssigner* _lambdaAssigner; // determines which wavelengths absorption will be recorded for
    const ProcessAssigner* _cellAssigner; // determines which cells will be given to the DustLib via
    ParallelTable _Labsstelvv;
    ParallelTable _Labsdustvv;
    bool _haveLabsstel;     // true if absorbed stellar emission is relevant for this simulation
    bool _haveLabsdust;     // true if absorbed dust emission is relevant for this simulation
};

//////////////////////////////////////////////////////////////////////

#endif // PANDUSTSYSTEM_HPP
