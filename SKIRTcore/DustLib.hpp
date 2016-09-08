/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef DUSTLIB_HPP
#define DUSTLIB_HPP

#include "ParallelTable.hpp"
#include "SimulationItem.hpp"

class ProcessAssigner;

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

    /** This function creates a default assigner if no assigner has been set during construction
        (which can happen since the assigner property is silent and optional). */
    void setupSelfBefore();

    //======================== Other Functions =======================

public:
    /** This function (re-)calculates the relevant dust emission spectra for the dust system, based on
        the absorption data currently stored in the dust cells, and internally caches the results. The
        function first calls the entries() and mapping() functions, implemented in each subclass. The
        first gives the total number of library entries. The second function is called to obtain the
        mapping from each dust cell \f$m\f$ to the corresponding library entry \f$n\f$. Next, the
        assign function of the ProcessAssigner class is called, with the number of library entries
        provided as an argument. The appropriate ProcessAssigner subclass will then, based on this
        number, assign different library entries to different processes. The subsequent calculations
        are then performed in parallel by the different processes, which in turn use an instance of the
        Parallel class to distribute the work amongst different threads. The smallest unit of
        parallelization here is obviously the calculation of the %SED for one particular library entry.
        The calculation itself is implemented in a helper class, called EmissionCalculator. For a
        particular library entry, the EmissionCalculator object first determines the mean %ISRF by
        averaging the ISRFs of all the dust cells that map onto it. Then, it calls on the
        DustEmissivity object held by the dust system to actually calculate the emissivities
        corresponding to the library entry. If the dust system contains multiple dust components
        \f$h\f$, each with its own dust mix, the emissivity \f$\varepsilon_{n,h,\ell}\f$ is calculated
        for each dust component \f$h\f$ seperately, and the results are combined into the complete
        emission spectrum for a dust cell \f$m\f$ through \f[ j_{m,\ell} =
        \sum_{h=0}^{N_{\text{comp}}-1} \rho_{m,h} \, \varepsilon_{n,h,\ell} \f] where \f$\ell\f$ is the
        wavelength index. Finally, this spectrum is normalized to unity and stored for later retrieval
        by luminosity(). Since the densities \f$\rho_{m,h}\f$ differ for each dust cell, the result
        must be calculated and stored for each dust cell separately. If the dust system has only a
        single dust component, the above formula reduces to \f$j_{m,\ell} =\rho_m\,
        \varepsilon_{n,\ell}\f$, so that the normalized emission spectrum is identical for all dust
        cells that map to a certain library entry. In this case, it is sufficient to just normalize and
        store the library templates and have luminosity() perform the mapping from dust cell to library
        entry. For each dust cell (or library entry in the latter case), the emissivities are converted
        to luminosities, yielding an emission spectrum or emission SED. After each process has
        calculated these SEDs for the library entries (and mapped dust cells) it was assigned to, the
        necessary communications are performed by calling the sync function on the table containing the
        results.*/
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
        that the cell produces no emission. When data parallelization is enabled, only the cells
        for this process are treated. The cell number \f$m\f$ is in that case a relative cell
        index. The function must be implemented by each subclass to
        provide this information to the base class. */
    virtual std::vector<int> mapping() const = 0;

    //======================== Data Members ========================

private:
    std::vector<int> _nv;           // library index for each cell or -1, indexed on m
    ParallelTable _Lvv;             // results of calculate(), used by luminosity()
    ProcessAssigner* _libAssigner;  // an assigner to parallelize the calculation of the library entries
    bool _nIndexed;                 // indicates whether the output is stored per library index n or per cell index m
};

////////////////////////////////////////////////////////////////////

#endif // DUSTLIB_HPP
