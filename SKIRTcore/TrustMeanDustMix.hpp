/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef TRUSTMEANDUSTMIX_HPP
#define TRUSTMEANDUSTMIX_HPP

#include "DustMix.hpp"

////////////////////////////////////////////////////////////////////

/** The TrustMeanDustMix class represents a dust mixture that formally consists of a single dust
    grain population, but that in reality represents a mixture of bare (i.e. non-composite)
    graphite, silicate and PAH dust grains. The size distribution of each of these dust grain
    populations is finetuned in such a way that the global dust properties accurately reproduce the
    extinction, emission and abundance constraints on the Milky Way. The size distributions are
    taken from Zubko, Dwek & Arendt (2004, ApJS, 152, 211) and correspond to model BARE_GR_S. It is used
    as a mean grain population for the TRUST benchmark simulations. */
class TrustMeanDustMix : public DustMix
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a dust mix from the TRUST benchmark (mean properties)")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE TrustMeanDustMix();

protected:
    /** This function reads the raw data from a resource file called <tt>TrustMeanDustMix/ZDA_BARE_GR_S_Effective.dat</tt>
        which contains the optical properties of the dust mixture for 1201 wavelengths ranging from
        0.001 micron to 10 mm. The function finally adds a single dust population with these properties to
        the dust mix. */
    void setupSelfBefore();
};

////////////////////////////////////////////////////////////////////

#endif // TRUSTMEANDUSTMIX_HPP
