/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef MULTIGRAINDUSTMIX_HPP
#define MULTIGRAINDUSTMIX_HPP

#include "DustMix.hpp"
class GrainComposition;
class GrainSizeDistributionInterface;

////////////////////////////////////////////////////////////////////

/** The MultiGrainDustMix class is an abstract subclass of the general DustMix class and represents
    dust mixtures containing dust grain populations with properties calculated from the underlying
    grain composition and grain size distributions. The MultiGrainDustMix class offers facilities
    to its subclasses to perform these calculations during setup. It adds dust populations with the
    resulting properties to the dust mix using the facilities offered by the DustMix base class.

    The MultiGrainDustMix class extends the DustMix public interface with a function to calculate
    the enthalpy of a typical dust grain at a specified temperature, for each dust population. This
    capability is offered here rather than in the DustMix base class because it requires access to
    the specific enthalpy information offered by the GrainComposition class hierarchy. */
class MultiGrainDustMix : public DustMix
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a multi-grain dust mix")

    Q_CLASSINFO("Property", "writeSize")
    Q_CLASSINFO("Title", "output a data file with grain size information for the dust mix")
    Q_CLASSINFO("Default", "yes")

    //============= Construction - Setup - Destruction =============

protected:
    /** The default constructor; it is protected since this is an abstract class. */
    MultiGrainDustMix();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the flag that indicates whether or not to output a data file with grain size
        information for the dust mixture. The default value is true. */
    Q_INVOKABLE void setWriteSize(bool value);

    /** Returns the flag that indicates whether or not to output a data file with grain size
        information for the dust mixture. */
    Q_INVOKABLE bool writeSize() const;

    //============= Functions for Use in Subclasses during Setup =============

protected:
    /** This function adds a number of dust populations \f$N_\text{bins}\f$ to the dust mix, all
        with the same specified grain composition \em gc, and according to the specified grain size
        distribution \em gs.

        The function calculates the fundamental dust properties from the following dust grain
        properties obtained from the \em gc argument: the absorption efficiencies
        \f$Q^{\text{abs}}(\lambda,a)\f$, the scattering efficiencies
        \f$Q^{\text{sca}}(\lambda,a)\f$, the scattering phase function asymmetry parameter
        \f$g(\lambda,a)\f$, the bulk density \f$\rho_{\text{bulk}}\f$ of the grain material; and
        from the grain size distribution per hydrogen atom
        \f$\Omega(a)=(\frac{\text{d}n_\text{D}}{\text{d}a})/n_\text{H}\f$ obtained from the \em gs
        argument.

        The grain size distribution is split into \f$N_\text{bins}\f$ bins on a logarithmic scale,
        and a seperate dust population is added for each bin. If the total range of the size
        distribution is given by \f$[a_\text{min},a_\text{max}]\f$, we denote the ranges for the
        various bins as \f$[a_{\text{min},c},a_{\text{max},c}], \,c=1,...,N_\text{bins}\f$ where
        \f$a_{\text{min},1}=a_\text{min}\f$ and \f$a_{\text{max},N_\text{bins}}=a_\text{max}\f$.
        The properties are calculated for each of the dust populations \f$c\f$ and for each
        wavelength \f$\lambda_\ell\f$ in the simulation's wavelength grid, using the formulas
        listed below.

        The absorption and scattering cross sections per hydrogen atom
        \f$\varsigma_{\ell,c}^{\text{abs}}\f$ and \f$\varsigma_{\ell,c}^{\text{abs}}\f$ for the
        \f$\ell\f$'th wavelength and of the \f$c\f$'th dust population are calculated by an
        integration over the size distribution, \f[ \varsigma_{\ell,c}^{\text{abs}} =
        \int_{a_{\text{min},c}}^{a_{\text{max},c}} \Omega(a)\, Q^{\text{abs}}(\lambda_\ell,a)\, \pi
        a^2\, {\text{d}}a \f] and \f[ \varsigma_{\ell,c}^{\text{sca}} =
        \int_{a_{\text{min},c}}^{a_{\text{max},c}} \Omega(a)\, Q^{\text{sca}}(\lambda_\ell,a)\, \pi
        a^2\, {\text{d}}a. \f] The asymmetry parameter \f$g_{\ell,c}\f$ is similarly calculated
        using \f[ g_{\ell,c} = \frac{1}{\varsigma_{\ell,c}^{\text{sca}}}
        \int_{a_{\text{min},c}}^{a_{\text{max},c}} \Omega(a)\, g(\lambda_\ell,a)\,
        Q^{\text{sca}}(\lambda_\ell,a)\, \pi a^2\, {\text{d}}a. \f] The dust mass \f$\mu_c\f$ for
        each dust population is calculated by integrating the bulk density over the size
        distribution, \f[ \mu_c = \int_{a_{\text{min},c}}^{a_{\text{max},c}} \Omega(a)\,
        \rho_{\text{bulk}}\, \frac{4\pi}{3}\, a^3\, {\text{d}}a. \f]

        All integrations are performed using a simple trapezoidal integration rule over a
        logarithmic grain size grid with a fixed number of points for each dust population.

        Assuming the corresponding write flag is turned on, the function writes information on the
        calculated grain size distribution to a file called <tt>prefix_ds_mix_h_size.dat</tt>,
        where h is the index of the dust component that uses this dust mixture.
    */
    void addpopulations(const GrainComposition* gc, const GrainSizeDistributionInterface* gs, int Nbins);

    /** This function performs identically to the other addpopulations() function. It is provided
        as a convenience so that subclasses can provide the grain size distribution as a classic
        function rather than an interface object. The function simply wraps the size distribution
        arguments into an interface and calls the other form of the addpopulations() function. */
    void addpopulations(const GrainComposition* gc,
                        double amin, double amax, double (*dnda)(double a),
                        int Nbins);

    //======== Getters for Additional Multi-Grain Properties =======

public:
    /** This function returns a brief human-readable identifier for the type of grain composition
        in the \f$c\f$'th dust population. */
    QString gcname(int c) const;

    /** This function returns the largest temperature for which this dust mix can provide
        meaningfull enthalpy data, for any of the dust populations in the mix. */
    double uppertemperature() const;

    /** This function returns the enthalpy at temperature \f$T\f$ of a "mean" dust grain
        representative of the \f$c\f$'th dust population. The enthalpy is equivalent to the
        internal energy of the dust grain, using an arbitrary zero point. It is obtained by
        multiplying the specific enthalpy of the appropriate grain composition (at the specified
        temperature) by the mass of a representative dust grain. If the specified temperature lies
        outside of the internally defined grid, the enthalpy value at the nearest border is used
        instead. */
    double enthalpy(double T, int c) const;

    /** This function returns the mean mass of a dust grain in the \f$c\f$'th dust population. */
    double meanmass(int c) const;

    //======================== Data Members ========================

private:
    // discoverable attributes
    bool _writeSize;

    // additional multi-grain properties setup in setupSelfBefore()
    std::vector<const GrainComposition*> _gcv;   // indexed on c
    std::vector<double> _meanmassv;              // indexed on c
};

////////////////////////////////////////////////////////////////////

#endif // MULTIGRAINDUSTMIX_HPP
