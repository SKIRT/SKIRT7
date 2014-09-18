/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef MEANZUBKODUSTMIX_HPP
#define MEANZUBKODUSTMIX_HPP

#include "DustMix.hpp"

////////////////////////////////////////////////////////////////////

/** The MeanZubkoDustMix class represents a dust mixture that formally consists of a single dust
    grain population, but that in reality represents a mixture of bare (i.e. non-composite)
    graphite, silicate and PAH dust grains. The size distribution of each of these dust grain
    populations is finetuned in such a way that the global dust properties accurately reproduce the
    extinction, emission and abundance constraints on the Milky Way. The size distributions are
    taken from Zubko, Dwek & Arendt (2004, ApJS, 152, 211) and correspond to model BARE_GR_S. */
class MeanZubkoDustMix : public DustMix
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a dust mix with mean Zubko properties")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE MeanZubkoDustMix();

protected:
    /** This function reads the raw data from a resource file called <tt>MeanZubkoDustMix.dat</tt>
        which contains the optical properties of the dust mixture for 1201 wavelengths ranging from
        0.05 micron to 100 mm. The file provides the extinction cross section per hydrogen nucleon
        \f$\tau^\text{ext} = C^\text{ext}/\text{H}\f$ (and the albedo), which means we must provide
        the dust mass per hydrogen nucleon \f$M_\text{dust}/\text{H} = 1.44\times
        10^{-26}\,\text{g}\,\text{H}^{-1}\f$ as listed for the relevant model in table 6 of the
        Zubko paper. The function finally adds a single dust population with these properties to
        the dust mix. */
    void setupSelfBefore();
};

////////////////////////////////////////////////////////////////////

#endif // MEANZUBKODUSTMIX_HPP
