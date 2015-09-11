/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef DUSTMIX_HPP
#define DUSTMIX_HPP

#include <vector>
#include "ArrayTable.hpp"
#include "Direction.hpp"
#include "SimulationItem.hpp"
#include "Table.hpp"
class PhotonPackage;
class Random;
class StokesVector;
class WavelengthGrid;

////////////////////////////////////////////////////////////////////

/** The DustMix class is an abstract class that represents a dust mixture with its associated set
    of optical properties. Dust mixtures can consist of several individual dust populations; these
    dust populations can represent for example dust grains with different chemical compositions
    and/or with different sizes. We consider only a discrete number of dust populations, so if one
    wants to represent a continuous range of dust populations (e.g. a size distribution), these
    have to be discretized in a finite number of populations.

    The DustMix class is the base class for all practical realizations of dust mixtures. It
    provides a public interface for retrieving the optical properties of the various dust
    populations in the mixture, and offers a number of facilities to assist subclasses. Subclasses
    are required to use the services offered by the DustMix class to setup the optical properties
    for each dust population in the mix.

    The DustMix class manages two categories of optical properties. Fundamental properties must be
    communicated by each concrete subclass to the DustMix class during setup; derived properties
    are calculated by the DustMix class without further assistance from the subclass. The DustMix
    class stores the fundamental properties and the frequently accessed derived properties in
    private member variables (i.e. not accessable to subclasses). Some derived properties are
    calculated on the fly.

    The fundamental properties stored and published by a DustMix object are:
     - the number of dust populations \f$N_{\text{pop}}\f$,
     - for each dust population \f$c\f$, defined at the grid points \f$\lambda_\ell\f$ of
       the simulation's wavelength grid:
       - the absorption cross section per hydrogen atom \f$\varsigma_{\ell,c}^{\text{abs}}\f$,
       - the scattering cross section per hydrogen atom \f$\varsigma_{\ell,c}^{\text{sca}}\f$,
       - the scattering phase function asymmetry parameter \f$g_{\ell,c}\f$,
     - for each dust population \f$c\f$:
       - the dust mass per hydrogen atom \f$\mu_{c}\f$.

    The derived properties foremost include properties that apply to the dust mixture as a
    whole, i.e. the total absorption, scattering and extinction cross sections per hydrogen atom \f[ \begin{split}
    \varsigma_\ell^{\text{abs}} &= \sum_{c=1}^{N_{\text{pop}}} \varsigma_{\ell,c}^{\text{abs}} \\
    \varsigma_\ell^{\text{sca}} &= \sum_{c=1}^{N_{\text{pop}}} \varsigma_{\ell,c}^{\text{sca}} \\
    \varsigma_\ell^{\text{ext}} &= \sum_{c=1}^{N_{\text{pop}}} \left(\varsigma_{\ell,c}^{\text{abs}} +
    \varsigma_{\ell,c}^{\text{sca}} \right), \end{split} \f] the global albedo \f[\varpi_\ell =
    \frac{\varsigma_\ell^{\text{sca}}}{\varsigma_\ell^{\text{ext}}}, \f] the average scattering phase
    function asymmetry parameter \f[ g_\ell = \frac{1}{\varsigma_\ell^{\text{sca}}}
    \sum_{c=1}^{N_{\text{pop}}} g_{\ell,c}\, \varsigma_{\ell,c}^{\text{sca}}, \f]
    and the total dust mass per hydrogen atom \f[\mu = \sum_{c=1}^{N_{\text{pop}}} \mu_{c}. \f]

    Furthermore the derived properties include the total absorption, scattering and extinction opacities for
    the dust mixture as a whole (note that the opacities are \em not provided for individual dust populations)
    \f[ \begin{split}
    \kappa_\ell^{\text{abs}} &= \frac{\varsigma_\ell^{\text{abs}}}{\mu} \\
    \kappa_\ell^{\text{sca}} &= \frac{\varsigma_\ell^{\text{sca}}}{\mu} \\
    \kappa_\ell^{\text{ext}} &= \frac{\varsigma_\ell^{\text{ext}}}{\mu}. \end{split} \f]

    If the simulation's wavelength grid represents a sampled wavelength range (as it would for
    panchromatic simulations), derived properties also include the Planck-integrated absorption
    cross sections per hydrogen atom \f$\varsigma_{\text{P},c}^{\text{abs}}(T)\f$, defined as \f[
    \varsigma_{\text{P},c}^{\text{abs}}(T) = \int_0^\infty \varsigma_{\lambda,c}^{\text{abs}}\,
    B_\lambda(T)\, {\text{d}}\lambda. \f]  These values are precalculated on a large grid of temperatures,
    so that the equation can also be solved for \f$T\f$ given a value for \f$\varsigma_{\text{P},c}^{\text{abs}}\f$.
*/
class DustMix : public SimulationItem
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a dust mix")

    Q_CLASSINFO("Property", "writeMix")
    Q_CLASSINFO("Title", "output a data file with the optical properties of the dust mix")
    Q_CLASSINFO("Default", "yes")

    Q_CLASSINFO("Property", "writeMeanMix")
    Q_CLASSINFO("Title", "output a data file with the mean optical properties of the dust mix")
    Q_CLASSINFO("Default", "yes")

    //============= Construction - Setup - Destruction =============

protected:
    /** The default constructor; it is protected since this is an abstract class. */
    DustMix();

    /** This function performs general setup for the abstract dust mix class. In their own
        setupSelfBefore() implementation, subclasses must communicate the fundamental optical
        properties for each dust population in the mix to the DustMix base class, using the
        protected functions provided by this class for this purpose. */
    void setupSelfBefore();

    /** This function calculates the cached derived optical properties and outputs some optical
        properties to text files, if so requested. Specifically this function performs the
        following tasks:
          - Verify that the subclass communicated information for at least one dust population,
            if not a fatal error is thrown.
          - Calculate the total cross sections per hydrogen atom \f$\varsigma_{\ell}^{\text{abs}}\f$,
            \f$\varsigma_{\ell}^{\text{sca}}\f$, and \f$\varsigma_{\ell}^{\text{ext}}\f$; the albedo
            \f$\varpi_{\ell}\f$; and the
            mean asymmetry parameter \f$g_{\ell}\f$ of the dust mixture, for each wavelength index.
          - Calculate the total dust mass per hydrogen atom \f$\mu\f$ of the dust mixture.
          - Calculate the total opacities \f$\kappa_{\ell}^{\text{abs}}\f$, \f$\kappa_{\ell}^{\text{sca}}\f$
            and \f$\kappa_{\ell}^{\text{ext}}\f$ of the dust mixture.
          - Assuming the corresponding write flags are turned on, write the optical properties of each
            dust population in the dust mixture to files called <tt>prefix_ds_mix_h_opti.dat</tt>
            and <tt>prefix_ds_mix_h_mass.dat</tt>, and write the combined optical properties of
            the dust mixture to a file called <tt>prefix_ds_mix_h_mean.dat</tt>, where h is the index
            of the dust component that uses this dust mixture.
          - If the simulation's wavelength grid represents a sampled wavelength range (as it would
            for panchromatic simulations), construct a temperature grid and calculate the corresponding
            Planck-integrated absorption cross sections per hydrogen atom
            \f$\kappa_{\text{P},c}^{\text{abs}}(T)\f$ for the different dust populations in the
            dust mixture. The grid is chosen to be semi-logarithmic in \f$T\f$ with a maximum
            temperature \f$T_{\text{max}}\f$ of 10000 K. Whereas this is obviously too large a
            temperature for dust grains (they sublimate at typically 1000 to 1500 K), such a large
            maximum temperature is useful to calculate the temperature of empty grid cells in the
            optically thin limit. The number of temperature points in grid is equal to 500 and the
            ratio between the innermost and outermost bin in the temperature distribution is set to
            1000, which results in the grid \f[ \begin{split} T_0 &= 0~{\text{K}}, \\ T_1 &=
            0.1376~{\text{K}}, \\ T_2 &= 0.2774~{\text{K}}, \\ &\vdots \\ T_{499} &=
            9862.4~{\text{K}}, \\ T_{500} &= 10000~{\text{K}}. \end{split} \f]
        */
    void setupSelfAfter();

    //============= Functions for Use in Subclasses during Setup =============

protected:
    /** This function returns the simulation's wavelength grid points \f$\lambda_\ell\f$, as a
        convenience for subclasses that wish to sample the fundamental properties on the
        appropriate grid on their own accord (for example, because the properties are defined by
        analytical functions). */
    const Array& simlambdav() const;

    /** This function, for use in a subclass, adds a new dust population with the specified
        fundamental properties to the dust mix. The arguments provide the dust mass per hydrogen atom
        \f$\mu\f$, the absorption and scattering cross sections per hydrogen atom
        \f$\varsigma_{\ell}^{\text{abs}}\f$ and \f$\varsigma_{\ell}^{\text{sca}}\f$, and the scattering phase
        function asymmetry parameter \f$g_{\ell}\f$ for the new dust population. The optical
        properties are sampled at the grid points \f$\lambda_\ell\f$ of the simulation's wavelength
        grid, thus the specified vectors must have the same length as the simulation's wavelength
        grid. This function should be used by subclasses that wish to sample the fundamental
        properties on the appropriate grid on their own accord, for example, because the properties
        are defined by analytical functions. */
    void addpopulation(double mu, const Array& sigmaabsv, const Array& sigmascav, const Array& asymmparv);

    /** This function, for use in a subclass, adds a new dust population with the specified
        fundamental properties to the dust mix. The arguments provide the dust mass per hydrogen atom
        \f$\mu\f$, the absorption and scattering cross sections per hydrogen atom
        \f$varsigma_k^{\text{abs}}\f$ and \f$varsigma_k^{\text{sca}}\f$, and the scattering phase
        function asymmetry parameter \f$g_k\f$ for the new dust population. The optical
        properties are defined at the specified grid points \f$\lambda_k\f$. All vectors must
        have the same length as the specified wavelength grid. If the extent of the specified
        wavelength grid is smaller than the extent of the simulation's wavelength grid, a fatal
        error message is thrown. Otherwise, the value of the optical properties at each of the grid
        points \f$\lambda_\ell\f$ is determined by a simple interpolation. For the absorption and
        scattering coefficients we use a log-log interpolation, for the asymmetry parameter we use
        a log-linear interpolation. This function can be used by subclasses that have the values of
        the fundamental properties available on some externally defined wavelength grid. */
    void addpopulation(double mu, const Array& lambdav,
                       const Array& sigmaabsv, const Array& sigmascav, const Array& asymmparv);

    /** This function, for use in a subclass, adds polarization properties for a single dust
        population to the dust mix. The arguments provide the four Mueller matrix coefficients
        \f$S_{11}(\lambda,\theta), S_{12}(\lambda,\theta), S_{33}(\lambda,\theta),
        S_{34}(\lambda,\theta)\f$ of dust grains in the population, sampled at the simulation's
        wavelength grid \f$\lambda_\ell\f$ and for a set of scattering angles
        \f$\theta_\mathrm{t}\f$. The four tables must have the same size. The size of the first
        dimension (\f$\lambda\f$) must match the number of wavelengths in the simulation's
        wavelength grid. The size of the second dimension (\f$\theta\f$) can be chosen by the
        caller but must be the same for all invocations of this function on a particular dust mix
        instance. */
    void addpolarization(const Table<2>& S11vv, const Table<2>& S12vv, const Table<2>& S33vv, const Table<2>& S34vv);

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the flag that indicates whether or not to output a data file with the optical
        properties of the dust mixture. The default value is true. */
    Q_INVOKABLE void setWriteMix(bool value);

    /** Returns the flag that indicates whether or not to output a data file with the optical
        properties of the dust mixture. */
    Q_INVOKABLE bool writeMix() const;

    /** Sets the flag that indicates whether or not to output a data file with the mean optical
        properties of the dust mixture. The default value is true. */
    Q_INVOKABLE void setWriteMeanMix(bool value);

    /** Returns the flag that indicates whether or not to output a data file with the mean optical
        properties of the dust mixture. */
    Q_INVOKABLE bool writeMeanMix() const;

    //======== Getters for Fundamental and Derived Properties =======

public:
    /** This function returns the number of dust populations in the mixture. */
    int Npop() const;

    /** This function returns the dust mass per hydrogen atom \f$\mu_{c}\f$ of the \f$c\f$'th dust
        population. */
    double mu(int c) const;

    /** This function returns the total dust mass per hydrogen atom \f$\mu_{c}\f$ of the dust
        mixture. */
    double mu() const;

    /** This function returns the absorption cross section per hydrogen atom
        \f$\varsigma^{\text{abs}}_{\ell,c}\f$ of the \f$c\f$'th dust population at wavelength index
        \f$\ell\f$. */
    double sigmaabs(int ell, int c) const;

    /** This function returns a vector with the absorption cross sections per hydrogen atom
        \f$\varsigma^{\text{abs}}_{\ell,c}\f$ of the \f$c\f$'th dust population, at all wavelengths
        in the simulation's wavelength grid. */
    const Array& sigmaabsv(int c) const;

    /** This function returns the total absorption cross section per hydrogen atom
        \f$\varsigma^{\text{abs}}_{\ell}\f$ of the dust mixture at wavelength index \f$\ell\f$. */
    double sigmaabs(int ell) const;

    /** This function returns the scattering cross section per hydrogen atom
        \f$\varsigma^{\text{sca}}_{\ell,c}\f$ of the \f$c\f$'th dust population at wavelength index
        \f$\ell\f$. */
    double sigmasca(int ell, int c) const;

    /** This function returns the total scattering cross section per hydrogen atom
        \f$\varsigma^{\text{sca}}_{\ell}\f$ of the dust mixture at wavelength index \f$\ell\f$. */
    double sigmasca(int ell) const;

    /** This function returns the total extinction cross section per hydrogen atom
        \f$\varsigma^{\text{ext}}_{\ell}\f$ of the dust mixture at wavelength index \f$\ell\f$. */
    double sigmaext(int ell) const;

    /** This function returns the total absorption coefficient \f$\kappa^{\text{abs}}_\ell\f$ of
        the dust mixture at wavelength index \f$\ell\f$. */
    double kappaabs(int ell) const;

    /** This function returns a vector with the total absorption coefficients
        \f$\kappa^{\text{abs}}_\ell\f$ of the dust mixture, at all wavelengths in the simulation's
        wavelength grid. */
    const Array& kappaabsv() const;

    /** This function returns the total scattering coefficient \f$\kappa^{\text{sca}}_\ell\f$ of
        the dust mixture at wavelength index \f$\ell\f$. */
    double kappasca(int ell) const;

    /** This function returns the total extinction coefficient \f$\kappa^{\text{ext}}_\ell\f$ of
        the dust mixture at wavelength index \f$\ell\f$. */
    double kappaext(int ell) const;

    /** This function returns the total extinction coefficient \f$\kappa^{\text{ext}}_\lambda\f$ of
        the dust mixture at the wavelength \f$\lambda\f$. If the simulation's wavelength grid
        represents a sampled wavelength range (as it would for panchromatic simulations), the
        requested value is determined by log-log interpolation of the values at the grid points
        bracketing the wavelength \f$\lambda\f$. If the simulation's wavelength grid contains
        individual distinct wavelengths (as used by oligochromatic simulations), the specified
        wavelength must be equal to one of the wavelengths in the simulation's wavelength grid
        (within a small margin), and the extinction coefficient for that wavelength is returned.
        If the specified wavelength is not in the grid, a fatal error is thrown.
        */
    double kappaext(double lambda) const;

    /** This function returns the global albedo \f$\varpi_\ell\f$ of the dust mixture at wavelength
        index \f$\ell\f$. */
    double albedo(int ell) const;

    /** This function returns true if this dust mix supports polarization; false otherwise. */
    bool polarization() const;

    /** This function generates a new direction \f${\bf{k}}_{\text{new}}\f$ in case the specified
        photon package scatters, and calculates the new polarization state of the scattered photon
        package. The function passes the new direction to the caller as its return value, and
        stores the new polarization state in the provided Stokes vector. It is permitted for the
        provided Stokes vector to actually reside in the specified photon package. For a dustmix
        that doesn't support polarization, the function generates the new direction from the
        normalized two-dimensional probability distribution \f[ p({\bf{k}}_{\text{new}})\,
        {\text{d}}{\bf{k}}_{\text{new}} = \Phi_\ell({\bf{k}}_{\text{new}}, {\bf{k}}_{\text{pp}})\,
        {\text{d}}{\bf{k}}_{\text{new}} \f] at wavelength index \f$\ell\f$ of the photon package.
        Also, in that case, the provided polarization state is not modified. For a dustmix that
        does support polarization, the function generates a new direction
        \f${\bf{k}}_{\text{new}}\f$ after a scattering event, given that the original direction
        before the scattering event is \f${\bf{k}}\f$ and taking into account the polarization
        state of the photon. First, the polarization degree and angle are computed from the Stokes
        parameters. Then, scattering angles \f$\theta\f$ and \f$\phi\f$ are sampled from the phase
        function, and the Stokes vector is rotated into the scattering plane and transformed by
        applying the Mueller matrix. Finally, the new direction is computed from the previously
        sampled \f$\theta\f$ and \f$\phi\f$ angles. */
    Direction scatteringDirectionAndPolarization(StokesVector* out, const PhotonPackage* pp) const;

    /** This function calculates the polarization state appropriate for a peel off photon package
        generated by a scattering event for the specified photon package, and stores the result in
        the provided Stokes vector. For a dustmix that doesn't support polarization, the function
        does nothing (i.e. it is assumed that the provided Stokes vector has been initialized to an
        unpolarized state). For a dustmix that does support polarization, the function rotates the
        Stokes vector from the reference direction in the previous scattering plane into the
        peel-off scattering plane, applies the Mueller matrix on the Stokes vector, and further
        rotates the Stokes vector from the reference direction in the peel-off scattering plane
        to the x-axis of the instrument to which the peel-off photon package is headed. */
    void scatteringPeelOffPolarization(StokesVector* out, const PhotonPackage* pp, Direction bfknew,
                                       Direction bfkx, Direction bfky);

    /** This function returns the value of the scattering phase function in case the specified
        photon package is scattered to the specified new direction, where the phase function is
        normalized as \f[\int\Phi_\ell(\Omega)\,\mathrm{d}\Omega=4\pi.\f]

        For a dustmix that doesn't support polarization, the function returns
        \f$\Phi_\ell({\bf{k}}_{\text{pp}}, {\bf{k}}_{\text{new}})\f$ for the current propagation
        direction of the photon package \f${\bf{k}}_{\text{pp}}\f$ and the specified new direction
        \f${\bf{k}}_{\text{new}}\f$, at wavelength index \f$\ell\f$ of the photon package, as
        described by the Henyey-Greenstein phase function.

        For a dustmix that does support polarization, the function returns the phase function for
        polarized radiation given by \f[\Phi_\ell(\Omega) = N \left( S_{11,\ell}(\theta) +
        P_\text{L} S_{12,\ell}(\theta) \cos 2(\varphi-\gamma) \right)\f] where \f$\theta\f$ is the
        angle between the photon package's propagation direction and the new scattering direction;
        \f$\phi\f$ is the angle between the previous and current scattering plane of the photon
        package; \f$\gamma\f$ is the polarization angle of the photon package, \f$P_\text{L}\f$ is
        the linear polarization degree of the photon package; and \f$N\f$ is a normalization factor
        to ensure that the integral over the unit sphere is equal to \f$4\pi\f$. */
    double phaseFunctionValue(const PhotonPackage* pp, Direction bfknew) const;

    /** This function returns the Planck-integrated absorption cross section per hydrogen atom
        \f$\varsigma_{\text{P},c}^{\text{abs}}(T)\f$ of the \f$c\f$'th dust population for the
        temperature \f$T\f$. It is defined as \f[ \varsigma_{\text{P},c}^{\text{abs}}(T) =
        \int_0^\infty \varsigma_{\lambda,c}^{\text{abs}}\, B_\lambda(T)\, {\text{d}}\lambda \f]
        where \f$\varsigma_{\lambda,c}^{\text{abs}}\f$ is the absorption cross section per hydrogen
        atom of the \f$c\f$'th dust population. The result is determined by linear interpolation of
        internally stored values. */
    double planckabs(double T, int c) const;

    /** This function returns the Planck-integrated absorption cross section per hydrogen atom
        \f$\varsigma_{\text{P}}^{\text{abs}}(T)\f$ of the complete dust mix for the
        temperature \f$T\f$. */
    double planckabs(double T) const;

    /** This function returns the temperature \f$T\f$ corresponding to a given value
        \f$\varsigma^{\text{abs}}_{\text{P},c}\f$ for the Planck-integrated absorption coefficient
        of the \f$c\f$'th dust population. Given \f$\varsigma^{\text{abs}}_{\text{P},c}\f$, it
        solves the equation \f[ \varsigma^{\text{abs}}_{\text{P},c} = \int_0^\infty
        \varsigma_{\lambda,c}^{\text{abs}}\, B_\lambda(T)\, {\text{d}}\lambda, \f] for \f$T\f$,
        where \f$\varsigma_{\lambda,c}^{\text{abs}}\f$ is the absorption cross section per hydrogen
        atom of the \f$c\f$'th dust population. The result is determined by linear interpolation of
        internally stored values. */
    double invplanckabs(double planckabs, int c) const;

    /** This function returns the temperature \f$T\f$ corresponding to a given value
        \f$\varsigma^{\text{abs}}_{\text{P}}\f$ for the Planck-integrated absorption coefficient
        of the complete dust mix. */
    double invplanckabs(double planckabs) const;

    /** This function returns the equilibrium temperature \f$T_{\text{eq}}\f$ of the \f$c\f$'th
        dust population when it would be embedded in the specified radiation field. */
    double equilibrium(const Array& Jv, int c) const;

private:
    /** This function returns a random scattering angle \f$\theta\f$ sampled from the phase
        function for a given wavelength index \f$\ell\f$. */
    double sampleTheta(int ell) const;

    /** This function returns a random scattering angle \f$\phi\f$ sampled from the phase function
        according to the incident linear polarization degree, the polarization angle and the
        scattering angle \f$\theta\f$, at wavelength index \f$\ell\f$. */
    double samplePhi(int ell, double theta, double polDegree, double polAngle) const;

    //======================== Data Members ========================

private:
    // discoverable attributes
    bool _writeMix;
    bool _writeMeanMix;

    // cached info initialized in setupSelfBefore()
    WavelengthGrid* _lambdagrid;
    int _Nlambda;
    Random* _random;

    // fundamental properties setup in subclass setupSelfBefore() through protected functions offered by this class
    int _Npop;                          // index c
    std::vector<double> _muv;           // indexed on c
    std::vector<Array> _sigmaabsvv;     // indexed on c and ell
    std::vector<Array> _sigmascavv;     // indexed on c and ell
    std::vector<Array> _asymmparvv;     // indexed on c and ell

    // cached derived properties calculated in setupSelfAfter()
    double _mu;
    Array _sigmaabsv;                   // indexed on ell
    Array _sigmascav;                   // indexed on ell
    Array _sigmaextv;                   // indexed on ell
    Array _kappaabsv;                   // indexed on ell
    Array _kappascav;                   // indexed on ell
    Array _kappaextv;                   // indexed on ell
    Array _albedov;                     // indexed on ell
    Array _asymmparv;                   // indexed on ell
    Array _Tv;                          // indexed on p
    ArrayTable<2> _planckabsvv;         // indexed on c and p

    // polarization-related data members
    bool _polarization;
    int _Ntheta;                    // index t
    const int _Nphi = 361;          // index f
    Table<2> _S11vv;                // indexed on ell and t
    Table<2> _S12vv;                // indexed on ell and t
    Table<2> _S33vv;                // indexed on ell and t
    Table<2> _S34vv;                // indexed on ell and t
    Array _thetav;                  // indexed on t
    ArrayTable<2> _thetaXvv;        // indexed on ell and t
    Array _pfnormv;                 // indexed on ell
    Array _phiv;                    // indexed on f
    Array _phi1v;                   // indexed on f
    Array _phisv;                   // indexed on f
    Array _phicv;                   // indexed on f
    Array _phiXv;                   // indexed on f
};

////////////////////////////////////////////////////////////////////

#endif // DUSTMIX_HPP
