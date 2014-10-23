/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef GREYBODYDUSTEMISSIVITY_HPP
#define GREYBODYDUSTEMISSIVITY_HPP

#include "DustEmissivity.hpp"

//////////////////////////////////////////////////////////////////////

/** The GreyBodyDustEmissivity class calculates the emissivity of a particular dust mix in a given
    radiation field, assuming that the dust grains are in local thermal equilibrium. Under this
    assumption (which is valid for large grains) the dust emits as a modified blackbody, with a
    different equilibrium temperature for every population in the mixture if it is a multi-grain
    mixture. The emissivity in an interstellar radiation field \f$J_\lambda\f$ can then immediately
    be calculated as \f[ \varepsilon_\lambda = \frac{1}{\mu} \sum_{c=0}^{N_{\text{pop}}-1}
    \varsigma_{\lambda,c}^{\text{abs}}\, B_\lambda(T_c) \f] with \f$\mu\f$ the total dust mass of
    the dust mix, \f$\varsigma_{\lambda,c}^{\text{abs}}\f$ the absorption cross section of the
    \f$c\f$'th dust population, and \f$T_c\f$ the equilibrium temperature of that population,
    defined by the balance equation \f[ \int_0^\infty \varsigma_{\lambda,c}^{\text{abs}}\,
    J_\lambda\, {\text{d}}\lambda = \int_0^\infty \varsigma_{\lambda,c}^{\text{abs}}\,
    B_\lambda(T_c)\, {\text{d}}\lambda. \f] */
class GreyBodyDustEmissivity : public DustEmissivity
{
    Q_OBJECT
    Q_CLASSINFO("Title", "modified blackbody dust emissivity (assumes LTE conditions)")

    //============= Construction - Setup - Destruction =============

public:
    /** Default constructor. */
    Q_INVOKABLE GreyBodyDustEmissivity();

    //======================== Other Functions =======================

public:
    /** This function returns the dust emissivity \f$\varepsilon_\ell\f$ at all wavelength indices
        \f$\ell\f$ for a dust mix of the specified type residing in the specified mean radiation
        field \f$J_\ell\f$, assuming the simulation's wavelength grid. */
    Array emissivity(const DustMix* mix, const Array& Jv) const;

};

////////////////////////////////////////////////////////////////////

#endif // GREYBODYDUSTEMISSIVITY_HPP
