/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef MONTECARLOSIMULATION_HPP
#define MONTECARLOSIMULATION_HPP

#include "Simulation.hpp"
#include <QTime>
#include <atomic>
class DustSystem;
class InstrumentSystem;
class PhotonPackage;
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
    Q_CLASSINFO("Title", "the number of photon (γ) packages per wavelength")
    Q_CLASSINFO("MinValue", "0")
    Q_CLASSINFO("MaxValue", "1e15")
    Q_CLASSINFO("Default", "1e6")

    //============= Construction - Setup - Destruction =============

protected:
    /** The default constructor; it is protected since this is an abstract class. */
    MonteCarloSimulation();

    /** This function verifies that all attribute values have been appropriately set. The dust
        system is optional and thus it may have a null value. */
    void setupSelfBefore();

    /** This function determines how the photon packages should be split over chunks. A chunk is
        the unit of parallelization in the simulation, i.e. multiple chunks may be performed
        simultaneously in different execution threads. The number of photons launched in a chunk,
        called the chunk size, is the same for all chunks in the simulation. All photon packages in
        a chunk have the same wavelength. If no photon packages must be launched for the
        simulation, the number of chunks is trivially zero. If there is only a single execution
        thread, there is no reason to split the photon packages into chunks, so the number of
        chunks per wavelength is set to one, and the chunk size is equal to the number of photon
        packages per wavelength. Otherwise the number of chunks is determined using a heuristic
        that balances the following objectives: (1) the total number of chunks for all wavelengths
        should be substantially larger than the number of parallel threads, to minimize the load
        imbalance at the end of each phase; (2) for the same reason, the chunk size should not be
        overly large; (3) the chunk size should not be too small, since each chunk incurs a
        nontrivial overhead. This leads to the following formula for the number of chunks per
        wavelength \f$N_\text{chunks}\f$ in function of the specified number of photon packages per
        wavelength \f$N_\text{pp}\f$, the number of wavelengths in the simulation's wavelength grid
        \f$N_\lambda\f$ and the number of parallel execution threads \f$N_\text{threads}\f$
        \f[N_\text{chunks} = {\text{ceil}}\left[ \min(\frac{N_\text{pp}}{2\,S_\text{min}},
        \max(\frac{N_\text{pp}}{S_\text{max}}, \frac{b\,N_\text{threads}}{N_\lambda})) \right]\f]
        where \f$S_\text{min}=10^4\f$ is the minimum chunk size, \f$S_\text{max}=10^7\f$ is the
        maximum chunk size, and \f$b=10\f$ is the load balancing safety factor. */
    void setupSelfAfter();

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

    /** This constant specifies the suggested number of photon packages to be processed between
        invocations of the logprogress() function. */
    const quint64 LOG_CHUNK_SIZE = 50000;

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
        characteristics as the original photon package, apart from two differences. The first one
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
        of the photon package). For each instrument in the instrument system, the function creates
        such a peel-off photon package and feeds it to the instrument. The first argument specifies
        the photon package that was just emitted; the second argument provides a placeholder peel
        off photon package for use by the function. */
    void peeloffscattering(PhotonPackage* pp, PhotonPackage* ppp);

    /** This function simulates the escape from the system and the absorption by dust of a fraction
        of the luminosity of a photon package. It actually splits the luminosity \f$L_\ell\f$ of
        the photon package in \f$N+2\f$ different parts, with \f$N\f$ the number of dust cells
        along its path through the dust system: a part \f$L_\ell^{\text{esc}}\f$ that escapes from
        the system, a part \f$L_\ell^{\text{sca}}\f$ that is scattered before it leaves the system,
        and \f$N\f$ parts \f$L_{\ell,n}^{\text{abs}}\f$, each of them corresponding to the fraction
        that is absorbed in a dust cell along the path. The part that scatters is the actual part
        of the photon package that survives and continues in the photon package's life cycle. So we
        alter the luminosity of the photon package from \f$L_\ell\f$ to \f$L_\ell^{\text{sca}}\f$.
        If dust emission is not considered in the simulation, that is also the only part that
        matters. On the other hand, if dust emission is taken into account in the simulation, we
        have to simulate the absorption of the fractions \f$L_{\ell,n}^{\text{abs}}\f$ by the
        appropriate dust cells. It is obviously crucial to have expressions for
        \f$L_\ell^{\text{sca}}\f$ and \f$L_{\ell,n}^{\text{abs}}\f$ (and \f$L_\ell^{\text{esc}}\f$,
        but this does not really matter...). If we denote the total optical depth along the path of
        the photon package as \f$\tau_{\ell,\text{path}}\f$ (this quantity is stored in the
        PhotonPackage object provided as an input parameter of this function), the fraction
        of the luminosity that escapes from the system without any interaction is \f[
        L_\ell^{\text{esc}} = L_\ell\, {\text{e}}^{-\tau_{\ell,\text{path}}}. \f] Remains to
        subdivide the remainder of the initial luminosity, \f[ L_\ell \left( 1 -
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
        the PhotonPackage object). It is straightforward to check that the sum of escaped, scattered
        and absorbed luminosities is \f[ L_\ell^{\text{esc}} + L_\ell^{\text{sca}} +
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
    void simulateescapeandabsorption(PhotonPackage* pp, bool dustemission);

    /** This function determines the next scattering location of a photon package and the simulates
        the propagation to this position. Given the total optical depth along the path of the
        photon package \f$\tau_{\ell,\text{path}}\f$ (this quantity is stored in the PhotonPackage
        object provided as an input parameter of this function), a random optical depth
        \f$\tau_\ell\f$ is generated from an exponential probability distribution cut off at
        \f$\tau_{\ell,\text{path}}\f$, distribution \f[ p(\tau_\ell)\,{\text{d}}\tau_\ell \propto
        \begin{cases} \;{\text{e}}^{-\tau_\ell}\,{\text{d}}\tau_\ell &\qquad \text{for
        }0<\tau_\ell<\tau_{\ell,\text{path}}, \\ \;0 &\qquad \text{for
        }\tau_\ell>\tau_{\ell,\text{path}}. \end{cases} \f] Once this random optical depth is
        determined, it is converted to a physical path length \f$s\f$ and the photon package is
        propagated over this distance. */
    void simulatepropagation(PhotonPackage* pp);

    /** This function simulates a scattering event of a photon package. Most of the properties of
        the photon package remain unaltered, including the position and the luminosity. The
        properties that change are the number of scattering events experienced by the photon
        package (this is obviously increased by one) and the propagation direction, which is
        generated randomly. If there is only one dust component, the propagation direction is
        sampled from from the scattering phase function. If there are several components, the
        function first generates a random dust component (where the relative weight of each dust
        component is equal to \f[ w_h = \frac{ \kappa_{\ell,h}^{\text{sca}}\, \rho_{h,m} }{
        \sum_{h'} \kappa_{\ell,h'}^{\text{sca}}\, \rho_{h',m} }, \f] where \f$\rho_{m,h}\f$ is the
        density of the dust corresponding to the \f$h\f$'th dust component in the dust cell where
        the scattering event takes place, and \f$\kappa_{\ell,h}^{\text{sca}}\f$ is the scattering
        coefficient corresponding to the \f$h\f$'th dust component respectively. When a random dust
        component is generated, a random propagation direction is generated randomly from the
        corresponding scattering phase function. */
    void simulatescattering(PhotonPackage* pp);

    /** This function performs the final step in a Monte Carlo simulation. It writes out the useful
        information in the instrument system and in the dust system so that the results of the
        simulation can be analyzed. */
    void write();

    //======================== Data Members ========================

protected:
    // *** discoverable attributes to be setup by a subclass ***
    WavelengthGrid* _lambdagrid;
    StellarSystem* _ss;
    DustSystem* _ds;

    // *** discoverable attributes managed by this class ***
    InstrumentSystem* _is;
    double _packages;       // the specified number of photon packages to be launched per wavelength

    // *** data members initialized by this class during setup ***
    quint64 _Nlambda;       // the number of wavelengths in the simulation's wavelength grid
    quint64 _Nchunks;       // the number of chunks to be launched per wavelength
    quint64 _chunksize;     // the number of photon packages in one chunk
    quint64 _Npp;           // the precise number of photon packages to be launched per wavelength

private:
    // *** data members used by the XXXprogress() functions in this class ***
    QString _phase;         // a string identifying the photon shooting phase for use in the log message
    std::atomic<quint64> _Ndone;  // the number of photon packages processed so far (for all wavelengths)
    QTime _timer;           // measures the time elapsed since the most recent log message
};

////////////////////////////////////////////////////////////////////

#endif // MONTECARLOSIMULATION_HPP
