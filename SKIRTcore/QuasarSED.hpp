/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef QUASARSED_HPP
#define QUASARSED_HPP

#include "StellarSED.hpp"

////////////////////////////////////////////////////////////////////

/** QuasarSED is a class that represents a simple model for the spectral energy distributions of a
    quasar; see Stalevski et al. (2012, MNRAS, 420, 2756–2772) and Schartmann et al. (2005, A&A,
    437, 861–881). It is defined in the wavelength range between 0.001 \f$\mu\f$m and 1000
    \f$\mu\f$m and is characterized by \f[ S_\lambda \propto \begin{cases} \; \lambda^{1/5} &
    \text{if $0.001~\mu{\text{m}}<\lambda<0.01~\mu{\text{m}}$} \\ \; \lambda^{-1} & \text{if
    $0.01~\mu{\text{m}}<\lambda<0.1~\mu{\text{m}}$} \\ \; \lambda^{-3/2} & \text{if
    $0.1~\mu{\text{m}}<\lambda<5~\mu{\text{m}}$} \\ \; \lambda^{-4} & \text{if
    $5~\mu{\text{m}}<\lambda<1000~\mu{\text{m}}$.} \end{cases} \f] */
class QuasarSED : public StellarSED
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a Quasar SED")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE QuasarSED();

protected:
    /** This function calculates a vector with the emissivity sampled at all grid points
        \f$\lambda_\ell\f$ of the global wavelength grid. */
    void setupSelfBefore();
};

////////////////////////////////////////////////////////////////////

#endif // QUASARSED_HPP
