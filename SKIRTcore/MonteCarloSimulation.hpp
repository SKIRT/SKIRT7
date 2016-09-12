/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef MONTECARLOSIMULATION_HPP
#define MONTECARLOSIMULATION_HPP

#include "Simulation.hpp"
#include <QTime>
#include <atomic>
class DustSystem;
class InstrumentSystem;
class PhotonPackage;
class ProcessAssigner;
class StellarSystem;
class WavelengthGrid;

//////////////////////////////////////////////////////////////////////

/** The MonteCarloSimulation class is the general abstract base class describing Monte Carlo
    simulations. Running a Monte Carlo simulation with SKIRT essentially comes down to constructing
    an instance of one of the subclasses of the MonteCarloSimulation base class and invoking the
    setupAndRun() function on it. The general MonteCarloSimulation class manages the instrument
    system and the number of photon packages that will be launched during the simulation run for
    each wavelength. It also provides pointers to a wavelength grid, a stellar system, and a dust
    system which must be setup in a subclass (so that their type can be subclass-dependent). */
class MonteCarloSimulation : public Simulation
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a Monte Carlo simulation")

    Q_CLASSINFO("Property", "instrumentSystem")
    Q_CLASSINFO("Title", "the instrument system")
    Q_CLASSINFO("Default", "InstrumentSystem")

    Q_CLASSINFO("Property", "packages")
    Q_CLASSINFO("Title", "the number of photon packages per wavelength")
    Q_CLASSINFO("MinValue", "0")
    Q_CLASSINFO("MaxValue", "1e15")
    Q_CLASSINFO("Default", "1e6")

    Q_CLASSINFO("Property", "minWeightReduction")
    Q_CLASSINFO("Title", "the minimum weight reduction factor of a photon package")
    Q_CLASSINFO("MinValue", "1e3")
    Q_CLASSINFO("Default", "1e4")
    Q_CLASSINFO("Silent", "true")

    Q_CLASSINFO("Property", "minScattEvents")
    Q_CLASSINFO("Title", "the minimum number of forced scattering events")
    Q_CLASSINFO("MinValue", "0")
    Q_CLASSINFO("MaxValue", "1000")
    Q_CLASSINFO("Default", "0")
    Q_CLASSINFO("Silent", "true")

    Q_CLASSINFO("Property", "scattBias")
    Q_CLASSINFO("Title", "the scattering bias")
    Q_CLASSINFO("MinValue", "0")
    Q_CLASSINFO("MaxValue", "1")
    Q_CLASSINFO("Default", "0.5")
    Q_CLASSINFO("Silent", "true")

    Q_CLASSINFO("Property", "continuousScattering")
    Q_CLASSINFO("Title", "use continuous scattering")
    Q_CLASSINFO("Default", "no")
    Q_CLASSINFO("Silent", "true")

    //============= Construction - Setup - Destruction =============

protected:
    /** The default constructor; it is protected since this is an abstract class. */
    MonteCarloSimulation();

    /** This function verifies that all attribute values have been appropriately set. The dust
        system is optional and thus it may have a null value. */
    void setupSelfBefore();

    /** This function determines how the specified number of photon packages should be split over
        chunks, and stores the resulting parameters in protected data members. It should be called
        at the start of each photon shooting phase. A chunk is the unit of parallelization in the
        simulation, i.e. multiple chunks may be performed simultaneously in different execution
        threads. The number of photons launched in a chunk, called the chunk size, is the same for
        all chunks in the simulation. All photon packages in a chunk have the same wavelength.
        Increasing the number of chunks per wavelength artificially increases the number of work
        units, giving a better load balancing between threads. If no photon packages must be
        launched for the simulation, the number of chunks is trivially zero. If the number of
        photon packages is nonzero, we will determine the number of chunks in two steps. The first
        one is based on whether multithreading is active, and the second one looks at the way the
        work is divided between the processes, which is different when data parallelization is
        active. \n\n

        -# Number of threads:

            -# When there is only one thread per process, each process will just run through
            the wavelengths serially, and hence no load balancing improvements can be gained by
            increasing the number of chunks. The number of chunks per process is set to 1.\n\n

            -# When multithreading is used, we want the total number of work units per process to
            be larger than 10 times the amount of threads. This condition can be written as:
            \f[\boxed{ \frac{N_\text{chunks} \times N_\lambda}{N_{procs}} > 10\times
            N_\text{threads} }\f] To further enhance the load balancing, we additionally enforce
            the chunks to not consist of more than \f$S_\text{max}=10^7\f$ photon packages:
            \f[\boxed{N_\text{chunks} > \frac{N_\text{pp}}{S_\text{max}}}\f] Combining these, the total number
            of chunks per wavelength becomes
            \f[ \boxed{ N_\text{chunks} = \text{max} \left( \frac{10 \times
            N_\text{threads} \times N_\text{procs}}{N_\lambda}, \frac{N_\text{pp}}{S_\text{max}}
            \right)} \f]

        -# Work division:

            -# When data parallelization is not active, each process will do all the
            wavelengths for every chunk it gets. Therefore we distribute the work units by
            letting each process do a subset of the chunks. The total amount of chunks per
            wavelength is first rounded up to a multiple of \f$N_\text{procs}\f$, and the
            number of chunks per wavelength per process then becomes
            \f[\boxed{N_\text{chunks, per proc} = \frac{N_\text{chunks}}{N_\text{procs}}}\f]

            -# When data parallelization is used, each process will only handle a particular
            subset of about \f$\frac{1}{N_\text{procs}}\f$ of the wavelengths. Each process
            will also be the only one to shoot the photons for those specific wavelengths.
            Therefore, the processes need to do all the chunks for their own wavelengths, and
            the number of chunks per wavelength per process is set equal to the total number of
            chunks per wavelength.
            \f[\boxed{N_\text{chunks, per proc} = N_\text{chunks}}\f] */
    void setChunkParams(double packages);

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the instrument system for this simulation. */
    Q_INVOKABLE void setInstrumentSystem(InstrumentSystem* value);

    /** Returns the instrument system for this simulation. */
    Q_INVOKABLE InstrumentSystem* instrumentSystem() const;

    /** Sets the number of photon packages to be launched per wavelength for this simulation.
        Photon packages are launched in chunks of the same size. Unless the specified number of
        photon packages is exactly equal to zero, a simulation always launches at least one chunk.
        The chunk size is determined automatically during setup as described for the function
        setupSelfAfter(). The number of photon packages per wavelength actually launched is always
        an integer multiple of the chunk size, and thus may be slightly more than the specified
        number. The maximum number of photon packages per wavelength is somewhat arbitrarily set to
        1e15. This function throws an error if a larger number is specified. The argument is of
        type double (which can exactly represent integers up to 9e15) rather than 64-bit integer to
        avoid implementing yet another discoverable property type. As a side benefit, one can use
        exponential notation to specify a large number of photon packages. */
    Q_INVOKABLE void setPackages(double value);

    /** Returns the number of photon packages to be launched per wavelength for this simulation
        exactly as specified by the setPackages() function (i.e. the value is not yet adjusted). */
    Q_INVOKABLE double packages() const;

    /** Sets the minimum weight reduction factor of a photon package before its life cycle is
        terminated. */
    Q_INVOKABLE void setMinWeightReduction(double value);

    /** Returns the minimum weight reduction factor of a photon package before its life cycle is
        terminated. */
    Q_INVOKABLE double minWeightReduction() const;

    /** Sets the minimum number of scattering events that a photon package should experience before
        its life cycle is terminated. */
    Q_INVOKABLE void setMinScattEvents(double value);

    /** Returns the minimum number of scattering events that a photon package should experience
        before its life cycle is terminated. */
    Q_INVOKABLE double minScattEvents() const;

    /** Sets the scattering bias, i.e. the fraction of the probability function for the
        optial depth distribution after a scattering event that is a constant function
        of \f$\tau\f$ rather than an exponentially declining function. */
    Q_INVOKABLE void setScattBias(double value);

    /** Returns the scattering bias. */
    Q_INVOKABLE double scattBias() const;

    /** Sets the flag that indicates whether continuous scattering should be used. The default
        value is false. */
    Q_INVOKABLE void setContinuousScattering(bool value);

    /** Returns the flag that indicates whether continuous scattering should be used. */
    Q_INVOKABLE bool continuousScattering() const;


    //======================== Other Functions =======================

public:
    /** This function returns the dimension of the simulation, which depends on the (lack of)
        symmetry in the stellar and dust geometries. A value of 1 means spherical symmetry, 2 means
        axial symmetry and 3 means none of these symmetries. The stellar or dust component with the
        least symmetry (i.e. the highest dimension) determines the result for the whole simulation.
        */
    int dimension() const;

protected:
    /** This function initializes the progress counter used in logprogress() for the specified
        phase and logs the number of photon packages and wavelengths to be processed. */
    void initprogress(QString phase);

    /** This function logs a progress message for the phase specified in the initprogress()
        function, assuming the previous message was issued at least 3 seconds ago. The function
        must be called regularly while processing photon packages. The argument specifies the
        number of photon packages processed since the most recent invocation in the same thread. */
    void logprogress(quint64 extraDone);

    /** This function drives the stellar emission phase in a Monte Carlo simulation. It consists of
        a parallelized loop that iterates over \f$N_{\text{pp}}\times N_\lambda\f$ monochromatic
        photons packages. Within this loop, the function simulates the life cycle of a single
        stellar photon package. Each photon package is born when it is emitted by the stellar
        system. Immediately after birth, peel-off photon packages are created and launched towards
        the instruments (one for each instrument). If there is dust in the system (there usually
        is...), the photon package now enters a cycle which consists of different steps. First, all
        details of the path of photon package through the dust system are calculated and stored.
        Based on this information, the escape and absorption of a fraction of the luminosity of the
        photon package is simulated. When the photon package is still luminous enough, it is
        propagated to a new scattering position, peel-off photon packages are created and launched
        towards each instrument, and the actual scattering is simulated. Then again, the details of
        the path of photon package through the dust system are calculated and stored, and the loop
        repeats itself. It is terminated only when the photon package has lost a very substantial
        part of its original luminosity (and hence becomes irrelevant). */
    void runstellaremission();

    /** This function implements the loop body for runstellaremission(). */
    void dostellaremissionchunk(size_t index);

    /** This function simulates the peel-off of a photon package after an emission event. This
        means that we create peel-off or shadow photon packages, one for every instrument in the
        instrument system, that we force to propagate in the direction of the observer(s) instead
        of in the propagation direction \f${\bf{k}}\f$ determined randomly by the emission process.
        Each peel-off photon package has the same characteristics as the original peel-off photon
        package, except that the propagation direction is altered to the direction
        \f${\bf{k}}_{\text{obs}}\f$ of the observer. For anistropic emission, a weight factor is
        applied to the luminosity to compensate for the fact that the probability that a photon
        package would have been emitted towards the observer is not the same as the probability
        that it is emitted in any other direction. For each instrument in the instrument system,
        the function creates such a peel-off photon package and feeds it to the instrument. The
        first argument specifies the photon package that was just emitted; the second argument
        provides a placeholder peel off photon package for use by the function. */
    void peeloffemission(PhotonPackage* pp, PhotonPackage* ppp);

    /** This function simulates the peel-off of a photon package before a scattering event. This
        means that, just before a scattering event, we create peel-off or shadow photon packages,
        one for every instrument in the instrument system, that we force to propagate in the
        direction of the observer(s) instead of in the propagation direction \f${\bf{k}}\f$
        determined randomly by the scattering process. Each peel-off photon package has the same
        characteristics as the original photon package, apart from three differences. The first one
        is, obviously, that the propagation direction is altered to the direction
        \f${\bf{k}}_{\text{obs}}\f$ towards the observer. The second difference is that we have to
        alter the luminosity of the photon package to compensate for this change in propagation
        direction. This compensation is necessary because the scattering process is anisotropic.
        Since we force the peel-off photon package to be scattered from the direction
        \f${\bf{k}}\f$ into the direction \f${\bf{k}}_{\text{obs}}\f$, we give it as additional
        weight factor the probability that a photon package would be scattered into the direction
        \f${\bf{k}}_{\text{obs}}\f$ if its original propagation direction was \f${\bf{k}}\f$. If
        there is only one dust component in the dust system, this weight factor is equal to the
        scattering phase function \f$w= \Phi_\ell({\bf{k}},{\bf{k}}_{\text{obs}})\f$ of the dust
        mixture at the wavelength index of the photon package. If there are different dust
        components, each with their own density and dust mixture, the appropriate weight factor is
        a weighted mean of the scattering phase functions, \f[ w = \frac{ \sum_h
        \kappa_{\ell,h}^{\text{sca}}\, \rho_{m,h}\, \Phi_{\ell,h}({\bf{k}}, {\bf{k}}_{\text{obs}})
        }{ \sum_h \kappa_{\ell,h}^{\text{sca}}\, \rho_{m,h} }, \f] where \f$\rho_{m,h}\f$ is the
        density of the dust corresponding to the \f$h\f$'th dust component in the dust cell where
        the scattering event takes place, and \f$\kappa_{\ell,h}^{\text{sca}}\f$ and
        \f$\Phi_{\ell,h}\f$ are the scattering coefficient and phase function corresponding to the
        \f$h\f$'th dust component respectively (both evaluated at the wavelength index \f$\ell\f$
        of the photon package). The third difference is that the polarization state of the peel off
        photon package is adjusted. If there are multiple dust components, the weight factors
        described above are used not just for the luminosity but also for the components of the
        Stokes vector. For each instrument in the instrument system, the function creates such a
        peel-off photon package and feeds it to the instrument. The first argument specifies the
        photon package that was just emitted; the second argument provides a placeholder peel off
        photon package for use by the function. */
    void peeloffscattering(PhotonPackage* pp, PhotonPackage* ppp);

    /** This function simulates the continuous peel-off of a series of photon packages along the
        path of the original photon package. It should be called before the
        simulateescapeandabsorption() function, because it assumes that the photon package still
        has its original luminosity. Peel-off photon packages are created for each non-empty dust
        cell in the original photon package's path, and for every instrument in the instrument
        system. Each peel-off package starts at a random location \f${\bf{r}}_{\text{cell}}\f$
        within the originating dust cell and along the original photon package's path, and is
        forced to propagate in the direction towards the observer \f${\bf{k}}_{\text{obs}}\f$. The
        luminosity of the peel-off photon package is derived from the original package's luminosity
        through \f$L_\text{ppp}=L_\text{pp}\,w_\text{cell}\,w_\text{obs}\f$. The first weight
        factor represents the luminosity fraction scattered in this cell, which is calculated as
        \f[ w_\text{cell} = \varpi_\ell \left( {\text{e}}^{-\tau_{\ell,n-1}} -
        {\text{e}}^{-\tau_{\ell,n}} \right), \f] with \f$\varpi_\ell\f$ the scattering albedo of
        the dust and \f$\tau_{\ell,n}\f$ the optical depth measured from the initial position of
        the path until the exit point of the \f$n\f$'th dust cell along the path. The second weight
        factor \f$w_{\text{obs}}\f$ compensates for the change in propagation direction, and is
        determined as explained for the function peeloffscattering(). */
    void continuouspeeloffscattering(PhotonPackage* pp, PhotonPackage* ppp);

    /** This function simulates the escape from the system and the absorption by dust of a fraction
        of the luminosity of a photon package. It actually splits the luminosity \f$L_\ell\f$ of
        the photon package in \f$N+2\f$ different parts, with \f$N\f$ the number of dust cells
        along its path through the dust system: a part \f$L_\ell^{\text{esc}}\f$ that escapes from
        the system, a part \f$L_\ell^{\text{sca}}\f$ that is scattered before it leaves the system,
        and \f$N\f$ parts \f$L_{\ell,n}^{\text{abs}}\f$, each of them corresponding to the fraction
        that is absorbed in a dust cell along the path. The part that scatters is the actual part
        of the photon package that survives and continues in the photon package's life cycle. So we
        alter the luminosity of the photon package from \f$L_\ell\f$ to \f$L_\ell^{\text{sca}}\f$.
        If the absorption rates are not considered in the simulation, that is also the only part
        that matters.

        On the other hand, if the absorption rates need to be taken into account in the simulation
        (if dust emission is included in the simulation, or the mean radiation field is
        calculated), we have to simulate the absorption of the fractions
        \f$L_{\ell,n}^{\text{abs}}\f$ by the appropriate dust cells. It is obviously crucial to
        have expressions for \f$L_\ell^{\text{sca}}\f$ and \f$L_{\ell,n}^{\text{abs}}\f$ (and
        \f$L_\ell^{\text{esc}}\f$, but this does not really matter...). If we denote the total
        optical depth along the path of the photon package as \f$\tau_{\ell,\text{path}}\f$ (this
        quantity is stored in the PhotonPackage object provided as an input parameter of this
        function), the fraction of the luminosity that escapes from the system without any
        interaction is \f[ L_\ell^{\text{esc}} = L_\ell\, {\text{e}}^{-\tau_{\ell,\text{path}}}.
        \f] Remains to subdivide the remainder of the initial luminosity, \f[ L_\ell \left( 1 -
        {\text{e}}^{-\tau_{\ell,\text{path}}} \right), \f] between the scattered fraction
        \f$L_\ell^{\text{sca}}\f$ and the \f$N\f$ absorbed fractions \f$L_{\ell,n}^{\text{abs}}\f$.
        When there is only one single dust component, the optical properties of the dust are the
        same everywhere along the path, and we easily have \f[ L_\ell^{\text{sca}} = \varpi_\ell\,
        L_\ell \left( 1 - {\text{e}}^{-\tau_{\ell,\text{path}}} \right) \f] with \f[ \varpi_\ell =
        \frac{ \kappa_\ell^{\text{sca}} }{ \kappa_\ell^{\text{ext}} } \f] the scattering albedo of
        the dust, and \f[ L_{\ell,n}^{\text{abs}} = (1-\varpi_\ell)\, L_\ell \left(
        {\text{e}}^{-\tau_{\ell,n-1}} - {\text{e}}^{-\tau_{\ell,n}} \right), \f] with
        \f$\tau_{\ell,n}\f$ the optical depth measured from the initial position of the path until
        the exit point of the \f$n\f$'th dust cell along the path (this quantity is also stored in
        the PhotonPackage object). It is straightforward to check that the sum of escaped,
        scattered and absorbed luminosities is \f[ L_\ell^{\text{esc}} + L_\ell^{\text{sca}} +
        \sum_{n=0}^{N-1} L_{\ell,n}^{\text{abs}} = L_\ell, \f] as desired. When there is more than
        one dust component, the dust properties are no longer uniform in every cell and things
        become a bit more messy. The expression for \f$L_{\ell,n}^{\text{abs}}\f$ can be readily
        expanded from the corresponding expression in the case of a single dust component, \f[
        L_{\ell,n}^{\text{abs}} = (1-\varpi_{\ell,n})\, L_\ell \left( {\text{e}}^{-\tau_{\ell,n-1}}
        - {\text{e}}^{-\tau_{\ell,n}} \right). \f] The only change is that the global albedo
        \f$\varpi_\ell\f$ is now replaced by a local albedo \f$\varpi_{\ell,n}\f$. It is found as
        the weighted mean of the albedo of the different dust components in the \f$n\f$'th dust
        cell along the path, \f[ \varpi_{\ell,n} = \frac{ \sum_h \kappa_{\ell,h}^{\text{sca}}\,
        \rho_{h,n} }{ \sum_h \kappa_{\ell,h}^{\text{ext}}\, \rho_{h,n} } \f] with \f$\rho_{h,n}\f$
        the density of the \f$h\f$'th dust component in the \f$n\f$'th cell. To calculate
        \f$L_\ell^{\text{sca}}\f$, we also have to take into account that the albedo varies from
        cell to cell, \f[ L_\ell^{\text{sca}} = L_\ell\, \sum_{n=0}^{N-1} \varpi_{\ell,n} \left(
        {\text{e}}^{-\tau_{\ell,n-1}} - {\text{e}}^{-\tau_{\ell,n}} \right). \f] Also in this case,
        it is easy to see that \f[ L_\ell^{\text{esc}} + L_\ell^{\text{sca}} + \sum_{n=0}^{N-1}
        L_{\ell,n}^{\text{abs}} = L_\ell. \f] */
    void simulateescapeandabsorption(PhotonPackage* pp, bool storeabsorptionrates);

    /** This function determines the next scattering location of a photon package and the simulates
        the propagation to this position. Given the total optical depth along the path of the
        photon package \f$\tau_{\ell,\text{path}}\f$ (this quantity is stored in the PhotonPackage
        object provided as an input parameter of this function), the appropriate probability
        distribution for the covered optical depth is an exponential probability
        distribution cut off at \f$\tau_{\ell,\text{path}}\f$. Properly normalized, it reads as
        \f[ p(\tau_\ell) = \frac{{\text{e}}^{-\tau_\ell}}
        {1-{\text{e}}^{-\tau_{\ell,\text{path}}}} \f] where the range of \f$\tau_\ell\f$ is limited
        to the interval \f$[0,\tau_{\ell,\text{path}}]\f$. Instead of generating a
        random optical depth \f$\tau_\ell\f$ directly from this distribution, we use the biasing
        technique in order to cover the entire allowed optical depth range \f$[0,\tau_{\ell,
        \text{path}}]\f$ more uniformly. As the biased probability distribution, we use a linear
        combination between an exponential distribution and a uniform distribution, with a parameter
        \f$\xi\f$ setting the relative importance of the uniform part. In formula form, \f[
        q(\tau_\ell) = (1-\xi)\, \frac{ {\text{e}}^{-\tau_\ell} }
        { 1-{\text{e}}^{-\tau_{\ell,\text{path}}} } + \frac{\xi}{\tau_{\ell,\text{path}}}. \f] A
        random optical depth from this distribution is readily determined. Since we use biasing, the
        weight, or correspondingly the luminosity, of the photon package needs to be adjusted with
        a bias factor \f$p(\tau_\ell)/q(\tau_\ell)\f$. Finally, the randomly determined optical
        depth is converted to a physical path length \f$s\f$, and the photon package is
        propagated over this distance. */
    void simulatepropagation(PhotonPackage* pp);

    /** This function simulates a scattering event of a photon package. Most of the properties of
        the photon package remain unaltered, including the position and the luminosity. The
        properties that change are the number of scattering events experienced by the photon
        package (this is obviously increased by one) the propagation direction, which is generated
        randomly, and the polarization state. If there is only one dust component, the propagation
        direction and polarization state are obtained from the scattering phase function. If there
        are several components, the function first generates a random dust component (where the
        relative weight of each dust component is equal to \f[ w_h = \frac{
        \kappa_{\ell,h}^{\text{sca}}\, \rho_{h,m} }{ \sum_{h'} \kappa_{\ell,h'}^{\text{sca}}\,
        \rho_{h',m} }, \f] where \f$\rho_{m,h}\f$ is the density of the dust corresponding to the
        \f$h\f$'th dust component in the dust cell where the scattering event takes place, and
        \f$\kappa_{\ell,h}^{\text{sca}}\f$ is the scattering coefficient corresponding to the
        \f$h\f$'th dust component respectively. When a random dust component is generated, a random
        propagation direction and polarization state are obtained from the corresponding scattering
        phase function. */
    void simulatescattering(PhotonPackage* pp);

    /** This function performs the final step in a Monte Carlo simulation. It writes out the useful
        information in the instrument system and in the dust system so that the results of the
        simulation can be analyzed. */
    void write();

    //======================== Data Members ========================

private:
    // *** discoverable attributes managed by this class ***
    InstrumentSystem* _is;
    double _packages;           // the specified number of photon packages to be launched per wavelength
    double _minWeightReduction; // the minimum weight reduction factor
    int _minfs;                 // the minimum number of scattering events
    double _xi;                 // the scattering bias
    bool _continuousScattering; // true if continuous scattering should be used

protected:
    // *** discoverable attributes to be setup by a subclass ***
    WavelengthGrid* _lambdagrid;
    StellarSystem* _ss;
    DustSystem* _ds;

protected:
    // *** data members initialized by this class through the setChunkParams() function ***
    quint64 _Nlambda;       // the number of wavelengths in the simulation's wavelength grid
    quint64 _Nchunks;       // the number of chunks to be launched per wavelength
    quint64 _chunksize;     // the number of photon packages in one chunk
    quint64 _Npp;           // the precise number of photon packages to be launched per wavelength
    quint64 _myTotalNpp;    // the total number of photon packages to be launched by this process
    quint64 _logchunksize;  // the number of photon packages to be processed between logprogress() invocations

private:
    // *** data members used by the XXXprogress() functions in this class ***
    QString _phase;         // a string identifying the photon shooting phase for use in the log message
    std::atomic<quint64> _Ndone;  // the number of photon packages processed so far (for all wavelengths)
    QTime _timer;           // measures the time elapsed since the most recent log message
};

////////////////////////////////////////////////////////////////////

#endif // MONTECARLOSIMULATION_HPP
