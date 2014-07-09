/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef MONTECARLOSIMULATION_HPP
#define MONTECARLOSIMULATION_HPP

#include "Simulation.hpp"
class DustSystem;
class DustSystemPath;
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
    Q_CLASSINFO("MaxValue", "2e13")
    Q_CLASSINFO("Default", "1e6")

    //============= Construction - Setup - Destruction =============

protected:
    /** The default constructor; it is protected since this is an abstract class. */
    MonteCarloSimulation();

    /** This function verifies that all attribute values have been appropriately set. The dust system
        is optional and thus it may have a null value. */
    void setupSelfBefore();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the instrument system for this simulation. */
    Q_INVOKABLE void setInstrumentSystem(InstrumentSystem* value);

    /** Returns the instrument system for this simulation. */
    Q_INVOKABLE InstrumentSystem* instrumentSystem() const;

    /** Sets the number of photon packages to be launched for this simulation per wavelength. The
        current implementation launches photon packages in chunks of 10000, thus the specified
        number is automatically rounded to an integer multiple of 10000. A simulation always
        launches at least one chunk (i.e. 10000 photon packages) per wavelength, unless the
        specified number of photon packages is exactly equal to zero. Also, the current
        implementation represents the number of chunks in a signed 32-bit integer (because the
        iteration count of a loop in the Parallel class is a 32-bit signed integer). This means
        that the number of photon packages per wavelength is limited to a maximum of 2e9 x 10000 =
        2e13. This function throws an error if a larger number is specified. The argument is of type
        double (which can exactly represent integers up to 9e15) to avoid 64-bit integers: they are
        difficult to declare consistently across 32 & 64 bit platforms, and we would need to
        implement yet another discoverable property type. As a side benefit, one can use
        exponential notation to specify a large number of photon packages. */
    Q_INVOKABLE void setPackages(double value);

    /** Returns the number of photon packages to be launched for this simulation per wavelength.
        The returned value is always a multiple of the chunk size, as discussed for setPackages().
        */
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
    /** This function is provided for use in subclasses to perform the final step in a Monte Carlo
        simulation. It writes out the useful information in the instrument system and in the dust
        system so that the results of the simulation can be analyzed. */
    void write();

    /** This function is provided for use in subclasses. It calculates the information on the path of
        a photon package through the dust system and stores it in a DustSystemPath object. In
        essence, the function calls the DustSystem::opticaldepth() function. */
    void fillDustSystemPath(PhotonPackage* pp, DustSystemPath* dsp) const;

    /** This function is provided for use in subclasses.
        It simulates the peel-off of a photon package after an emission event. This
        means that we create peel-off or shadow photon packages, one for every instrument in the
        instrument system, that we force to propagate in the direction of the observer(s) instead
        of in the propagation direction \f${\bf{k}}\f$ determined randomly by the emission process.
        Each peel-off photon package has the same
        characteristics as the original peel-off photon package, except that the propagation
        direction is altered to the direction \f${\bf{k}}_{\text{obs}}\f$ of the observer (there is
        no extra weight factor or compensation because the emission is considered to be isotropic,
        and so the probability that a photon package would have been emitted towards the observer
        is the same as the probability that it is emitted in any other direction). For each
        instrument in the instrument system, the function creates such a peel-off photon package
        and feeds it to the instrument. */
    void peeloffemission(PhotonPackage* pp);

    /** This function is provided for use in subclasses.
        It simulates the peel-off of a photon package before a scattering event. This
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
        such a peel-off photon package and feeds it to the instrument. */
    void peeloffscattering(PhotonPackage* pp);

    /** This function is provided for use in subclasses.
        It simulates the escape from the system and the absorption by dust of a fraction
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
        DustSystemPath object that provided as an input parameter of this function), the fraction
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
        the exit point of the \f$n\f$'th dust cell along the path (this quantity is stored in the
        DustSystemPath object). It is straightforward to check that the sum of escaped, scattered
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
    void simulateescapeandabsorption(PhotonPackage* pp, DustSystemPath* dsp, bool dustemission);

    /** This function is provided for use in subclasses.
        It determines the next scattering location of a photon package and the simulates
        the propagation to this position. Given the total optical depth along the path of the
        photon package \f$\tau_{\ell,\text{path}}\f$ (this quantity is stored in the DustSystemPath
        object that is provided as an input parameter of this function), a random optical depth
        \f$\tau_\ell\f$ is generated from an exponential probability distribution cut off at
        \f$\tau_{\ell,\text{path}}\f$, distribution \f[ p(\tau_\ell)\,{\text{d}}\tau_\ell \propto
        \begin{cases} \;{\text{e}}^{-\tau_\ell}\,{\text{d}}\tau_\ell &\qquad \text{for
        }0<\tau_\ell<\tau_{\ell,\text{path}}, \\ \;0 &\qquad \text{for
        }\tau_\ell>\tau_{\ell,\text{path}}. \end{cases} \f] Once this random optical depth is
        determined, it is converted to a physical path length \f$s\f$ and the photon package is
        propagated over this distance. */
    void simulatepropagation(PhotonPackage* pp, DustSystemPath* dsp);

    /** This function is provided for use in subclasses.
        It simulates a scattering event of a photon package. Most of the properties of
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

    //======================== Data Members ========================

protected:
    // *** discoverable attributes to be setup by a subclass ***

    WavelengthGrid* _lambdagrid;
    StellarSystem* _ss;
    DustSystem* _ds;

    // *** discoverable attributes managed by this class ***

    // the instrument system
    InstrumentSystem* _is;

    // the number of photon packages in one chunk
    enum ChunkSize { CHUNKSIZE = 10000 };

    // the number of photon package chunks to be launched per wavelength
    // this value is managed by setPackages()
    int _Nchunks;
};

////////////////////////////////////////////////////////////////////
#endif // MONTECARLOSIMULATION_HPP

