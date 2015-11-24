/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef TRUSTPOLARIZEDMEANDUSTMIX_HPP
#define TRUSTPOLARIZEDMEANDUSTMIX_HPP

#include "DustMix.hpp"

////////////////////////////////////////////////////////////////////

/** The TrustPolarizedMeanDustMix class represents a dust mixture that formally consists of a
    single dust grain population, but that in reality represents a mixture of bare (i.e.
    non-composite) graphite, silicate and PAH dust grains. The size distribution of each of these
    dust grain populations is finetuned in such a way that the global dust properties accurately
    reproduce the extinction, emission and abundance constraints on the Milky Way. The size
    distributions are taken from Zubko, Dwek & Arendt (2004, ApJS, 152, 211) and correspond to
    model BARE_GR_S. The class supports scattering polarization assuming spherical grains, loading
    the Mueller matrix coefficients from the appropriate resource data files. For more information,
    see Camps et al. 2015, AA 580, A87. */
class TrustPolarizedMeanDustMix : public DustMix
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a dust mix from the TRUST benchmark (mean properties), includes polarization")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE TrustPolarizedMeanDustMix();

protected:
    /** This function reads the optical properties from the appropriate resource files, and adds a
        single dust population with these properties to the dust mix. */
    void setupSelfBefore();
};

////////////////////////////////////////////////////////////////////

#endif // TRUSTPOLARIZEDMEANDUSTMIX_HPP
