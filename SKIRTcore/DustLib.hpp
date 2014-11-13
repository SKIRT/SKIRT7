/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef DUSTLIB_HPP
#define DUSTLIB_HPP

#include "ArrayTable.hpp"
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

    Q_CLASSINFO("Property", "assigner")
    Q_CLASSINFO("Title", "the parallel process assignment scheme")
    Q_CLASSINFO("Default", "SequentialAssigner")
    Q_CLASSINFO("Optional", "true")
    Q_CLASSINFO("Silent", "true")

    //============= Construction - Setup - Destruction =============

protected:
    /** Default constructor. */
    DustLib();

    /** This function creates a default assigner if no assigner has been set during construction
        (which can happen since the assigner property is silent and optional). */
    void setupSelfBefore();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** This function sets the process assigner for this dust library. The process assigner is the
         object that assigns different library entries to different processes, to parallelize the
         calculation of the dust emission SEDs. The ProcessAssigner class is the abstract class that
         represents different types of assigners; different subclass implement the assignment in
         different ways. While the most straightforward types of process assigners are used to assign
         each library entry to a different process to speed up the calculation, one ProcessAssigner
         subclass lets all processes calculate the dust SEDs for all dust cells. This can be useful if
         the calculation itsself, perhaps by using a lower-dimensional library mechanism, is not CPU
         expensive but the subsequent communication between processes proves to be time consuming. */
    Q_INVOKABLE void setAssigner(ProcessAssigner* value);

    /** Returns the process assigner for this dust library. */
    Q_INVOKABLE ProcessAssigner* assigner() const;

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
        calculated these SEDs for the library entries (and mapped dust cells) it was assigned to, each
        process must assemble the SEDs for each of these entries (or dust cells) at one place,
        requiring the information stored at all other processes. This is done by calling the assemble
        function. Of course, this procedure only has to be performed when the processes have indeed
        performed the calculation in parallel. This depends on which ProcessAssigner subclass has been
        chosen by the user. If a 'parallel' ProcessAssigner has been chosen, assembly and thus
        communication between the processes is required. Another ProcessAssigner can be chosen which
        assigns each process to the same library entries, avoiding the need for communication
        afterwards. Whether or not the assemble function has to be called is determined by the parallel
        function of the ProcessAssigner. */
    void calculate();

    /** This function returns the luminosity fraction \f$L_\ell\f$ at the wavelength index
        \f$\ell\f$ in the normalized dust emission spectrum corresponding to the dust cell with
        dust cell number \f$m\f$. The function simply looks up the appropriate value in the cached
        results produced by calculate(). */
    double luminosity(int m, int ell) const;

private:
    /** This function is used to assemble the list of luminosities with the information contained at
        different processes. When a ProcessAssigner subclass is used which distributes the calculation
        of the dust emission spectra amongst the different parallel processes, each process contains
        only the emission luminosities for a particular set of library entries or dust cells (depending
        on which DustLib subclass is used and whether or not multiple dust components are present). In
        order to perform the simulation of thermal photon packages, each process needs the emission %SED
        for all dust cells (or all library entries). This function uses the PeerToPeerCommunicator to
        broadcast the SEDs of the dust cells (or library entries) assigned to a particular process to
        all other processes and storing them in the appropriate place in the list. This is implemented
        as a loop over all dust cells (or all library entries), where for each dust cell (library
        entry) the ProcessAssigner is called to determine which process was assigned to it. Next, the
        broadcast function of the PeerToPeerCommunicator class is called, passing the %SED of that dust
        cell (library entry) and rank of that process as arguments. Only for that process, the %SED
        contains actual luminosities, whereas for the other processes the %SED is zero. When the
        broadcast function returns, the luminosities from the former process have been copied in the
        memory of the other processes. When this is done for each dust cell (library entry), the
        assembly is complete. */
    void assemble();

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
    std::vector<int> _nv;        // library index for each cell or -1, indexed on m
    ArrayTable<2> _Lvv;          // luminosities indexed on m or n and ell
    ProcessAssigner* _assigner;  // the process assigner; determines which library entries are assigned to this process
};

////////////////////////////////////////////////////////////////////

#endif // DUSTLIB_HPP
