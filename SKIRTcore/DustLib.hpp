/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef DUSTLIB_HPP
#define DUSTLIB_HPP

#include "ArrayTable.hpp"
#include "SimulationItem.hpp"

//////////////////////////////////////////////////////////////////////

/** The DustLib class manages the calculation of dust cell emission spectra based on the absorption
    data accumulated in the dust system. The calculation of the dust-mix-dependent emissivities is
    outsourced to the DustEmissivity object held by the dust system. The DustLib class implements a
    dust library mechanism as described in Baes et al. (2011, ApJS, 196, 22). Instead of
    calculating the dust %SED individually for every dust cell in the dust system, a library is
    constructed and template SEDs from this library are used. Obviously, the SEDs in the library
    should be chosen/constructed in such a way that they can represent the whole range of actual
    dust SEDs encountered in the simulation. In other words, the library should span the entire
    parameter space of interstellar radiation fields. Different subclasses of the DustLib class
    achieve this goal to different degrees of sophistication (with a better coverage of the
    parameter space typically at the cost of a more CPU expensive library construction). */
class DustLib : public SimulationItem
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a dust library")

    //============= Construction - Setup - Destruction =============

protected:
    /** Default constructor. */
    DustLib();

    //======================== Other Functions =======================

public:
    /** This function (re-)calculates the relevant dust emission spectra for the dust system, based
        on the absorption data currently stored in the dust cells, and internally caches the
        results. The function first calls the mapping() function, implemented by each subclass, to
        obtain the mapping from each dust cell \f$m\f$ to the corresponding library entry \f$n\f$.
        Subsequently, for every library entry,
        the function determines the mean %ISRF by averaging the ISRFs of all the dust cells that
        map onto it, and then it calls on the DustEmissivity object held by the dust system to
        actually calculate the emissivities corresponding to the library entry. If the dust system
        contains multiple dust components \f$h\f$, each with its own dust mix, the emissivity
        \f$\varepsilon_{n,h,\ell}\f$ is calculated for each dust component \f$h\f$ seperately, and
        the results are combined into the complete emission spectrum for a dust cell \f$m\f$ through
        \f[ j_{m,\ell} =
        \sum_{h=0}^{N_{\text{comp}}-1} \rho_{m,h} \, \varepsilon_{n,h,\ell} \f] where \f$\ell\f$ is the
        wavelength index. Finally, this spectrum is normalized to unity and stored for later retrieval by
        luminosity(). Since the densities \f$\rho_{m,h}\f$ differ for each dust cell, the result must
        be calculated and stored for each dust cell separately. If the dust system has only a
        single dust component, the above formula reduces to \f$j_{m,\ell} =\rho_m\, \varepsilon_{n,\ell}\f$,
        so that the normalized emission spectrum is identical for all dust cells that map to a
        certain library entry. In this case, it is sufficient to just normalize and store the
        library templates and have luminosity() perform the mapping from dust cell to library
        entry. */
    void calculate();

    /** This function returns the luminosity fraction \f$L_\ell\f$ at the wavelength index
        \f$\ell\f$ in the normalized dust emission spectrum corresponding to the dust cell with
        dust cell number \f$m\f$. The function simply looks up the appropriate value in the cached
        results produced by calculate(). */
    double luminosity(int m, int ell) const;

protected:
    /** This function returns the number of entries in the library. It must be implemented by each
        subclass to provide this information to the base class. */
    virtual int entries() const = 0;

    /** This function returns a vector \em nv with length \f$N_{\text{cells}}\f$ that maps each
        cell \f$m\f$ to the corresponding library entry \f$n_m\f$. A index value of -1 indicates
        that the cell produces no emission. The function must be implemented by each subclass to
        provide this information to the base class. */
    virtual std::vector<int> mapping() const = 0;

    //======================== Data Members ========================

private:
    // results of calculate(), used by luminosity()
    std::vector<int> _nv;   // library index for each cell or -1, indexed on m
    ArrayTable<2> _Lvv;     // luminosities indexed on m or n and ell
};

////////////////////////////////////////////////////////////////////

#endif // DUSTLIB_HPP
