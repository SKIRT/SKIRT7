/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef PANDUSTSYSTEM_HPP
#define PANDUSTSYSTEM_HPP

#include "DustSystem.hpp"
class DustEmissivity;
class DustLib;

//////////////////////////////////////////////////////////////////////

/** A PanDustSystem class object represents a complete dust system for use with panchromatic
    simulations. This class relies on the functionality implemented in the DustSystem base class,
    providing a factory method for creating a dust cell of the appropriate PanDustCell type. In
    addition to the DustSystem functionality, this class supports dust emission. Most importantly,
    it holds a DustEmissivity object and a DustLib object used to calculate the dust emission
    spectrum for dust cells. */
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

    Q_CLASSINFO("Property", "selfAbsorption")
    Q_CLASSINFO("Title", "include dust self-absorption")
    Q_CLASSINFO("Default", "yes")
    Q_CLASSINFO("RelevantIf", "dustEmissivity")

    Q_CLASSINFO("Property", "writeEmissivity")
    Q_CLASSINFO("Title", "output a file with the dust mix emissivities in the local ISRF")
    Q_CLASSINFO("Default", "no")
    Q_CLASSINFO("RelevantIf", "dustEmissivity")

    Q_CLASSINFO("Property", "writeTemperature")
    Q_CLASSINFO("Title", "output FITS files displaying the dust temperature distribution")
    Q_CLASSINFO("Default", "yes")
    Q_CLASSINFO("RelevantIf", "dustEmissivity")

    Q_CLASSINFO("Property", "writeISRF")
    Q_CLASSINFO("Title", "output a data file describing the interstellar radiation field")
    Q_CLASSINFO("Default", "yes")
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

    /** This factory function creates and returns a pointer to a new dust cell of the appropriate
        PanDustCell type. */
    virtual DustCell* createDustCell();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the optional dust emissivity object for this dust system. */
    Q_INVOKABLE void setDustEmissivity(DustEmissivity* value);

    /** Returns the dust emissivity object for this dust system, or null if there is no dust emission. */
    Q_INVOKABLE DustEmissivity* dustEmissivity() const;

    /** Sets the dust library object for this dust system. */
    Q_INVOKABLE void setDustLib(DustLib* value);

    /** Returns the dust library object for this dust system, or null if there is no dust emission. */
    Q_INVOKABLE DustLib* dustLib() const;

    /** Sets the flag indicating whether to include dust self-absorption. The default value is
        true. If dust emission is turned off, the value of this flag is irrelevant. */
    Q_INVOKABLE void setSelfAbsorption(bool value);

    /** Returns the flag indicating whether to include dust self-absorption. If dust emission is
        turned off, this function returns false. */
    Q_INVOKABLE bool selfAbsorption() const;

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

    /** This function (re-)calculates the relevant dust emission spectra for the dust system, based
        on the absorption data currently stored in the dust cells, and internally caches the
        results. If dust emission is turned off, this function does nothing. */
    void calculatedustemission();

    /** This function returns the luminosity \f$L_\ell\f$ at the wavelength index \f$\ell\f$ in the
        normalized dust emission SED corresponding to the dust cell with dust cell number \f$m\f$.
        It just looks up the appropriate value in the cached results produced by calculate(). If
        dust emission is turned off, the function returns zero. */
    double dustluminosity(int m, int ell) const;

    /** If the writeISRF attribute is true, this function writes out a data file (named
        <tt>prefix_ds_isrf.dat</tt>) describing the interstellar radiation field for each wavelength.
        The first line contains a list of all wavelengths in the simulation's wavelength grid, and
        followed by a blank line. Subsequently there is a line for each dust cell in the dust grid.
        Each line has the following columns: cell identifier \f$m\f$, x, y and z positions, mean
        radiation field \f$J_{\lambda,m}\f$ for every wavelength \f$\lambda\f$.

        If the writeTemperature attribute is true, this function writes out FITS files (named
        <tt>prefix_ds_tempXX.fits</tt>) with the mean dust temperatures in the coordinate planes.
        Each of these maps contains 1024 x 1024 pixels, and covers as a field of view the total
        extension of the grid. The number of data files written depends on the dimension of the
        dust system's geometry: for spherical symmetry only the intersection with the xy plane is
        written, for axial symmetry the intersections with the xy and xz planes are written, and
        for general geometries all three intersections are written. Each FITS file is a data cube,
        where a map is created for each dust population of each dust component. The mean dust
        temperature \f${\bar{T}}_c({\boldsymbol{r}})\f$ corresponding to the \f$c\f$'th population
        at the position \f${\boldsymbol{r}}\f$ is calculated from the mean intensity
        \f$J_\lambda({\boldsymbol{r}})\f$ of the radiation field using the balance equation \f[
        \int_0^\infty \kappa_{\lambda,c}^{\text{abs}}\, J_\lambda({\boldsymbol{r}})\,
        {\text{d}}\lambda = \int_0^\infty \kappa_{\lambda,c}^{\text{abs}}\,
        B_\lambda({\bar{T}}_c({\boldsymbol{r}}))\, {\text{d}}\lambda, \f] where
        \f$\kappa_{\lambda,c}^{\text{abs}}\f$ is the absorption coefficient of the \f$c\f$'th dust
        population. Note that this mean dust temperature is only illustrative: it does not imply
        that the dust emits as a modified blackbody at an equibrium temperature. */
    void write() const;

    //======================== Data Members ========================

private:
    // data members to be set before setup is invoked
    DustEmissivity* _dustemissivity;
    DustLib* _dustlib;
    bool _selfabsorption;
    bool _writeEmissivity;
    bool _writeTemp;
    bool _writeISRF;

    // data members initialized during setup
    int _Nlambda;
};

//////////////////////////////////////////////////////////////////////

#endif // PANDUSTSYSTEM_HPP
