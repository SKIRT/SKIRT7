/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef BENCHMARK1DDUSTMIX_HPP
#define BENCHMARK1DDUSTMIX_HPP

#include "DustMix.hpp"

////////////////////////////////////////////////////////////////////

/** The Benchmark1DDustMix class represents the idealized dust mixture used in the 1D radiative
    transfer benchmark calculations of Ivezic et al. (1997, MNRAS, 291, 121). Scattering is assumed
    to be isotropic and the absorption and scattering coefficients are approximated by simple
    analytical functions. With \f$\lambda\f$ representing the wavelength expressed in \f$\mu\f$m,
    the absorption coefficient is given by \f[ \frac{\kappa_\lambda^{\text{abs}}}
    {\kappa_1^{\text{abs}}} = \begin{cases} \; 1 & \text{if $\lambda<1$} \\ \; \dfrac{1}{\lambda} &
    \text{else}, \end{cases} \f] and the scattering coefficient by \f[
    \frac{\kappa_\lambda^{\text{sca}}} {\kappa_1^{\text{sca}}} = \begin{cases} \; 1 & \text{if
    $\lambda<1$} \\ \; \dfrac{1}{\lambda^4} & \text{else}. \end{cases} \f]
    The extinction coefficients in the benchmark data are scale-free; we arbitrarily
    scale them to a reasonable order of magnitude. */
class Benchmark1DDustMix : public DustMix
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a dust mix from the 1D benchmark model (Ivezic et al. 1997)")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE Benchmark1DDustMix();

protected:
    /** This function directly calculates all dust mix properties on the simulation's wavelength
        grid. It then adds a single dust population to the dust mix, scaling the coefficients to a
        reasonable order of magnitude. */
    void setupSelfBefore();
};

////////////////////////////////////////////////////////////////////

#endif // BENCHMARK1DDUSTMIX_HPP
