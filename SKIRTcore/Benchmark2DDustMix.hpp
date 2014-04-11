/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef BENCHMARK2DDUSTMIX_HPP
#define BENCHMARK2DDUSTMIX_HPP

#include "DustMix.hpp"

////////////////////////////////////////////////////////////////////

/** The Benchmark2DDustMix class represents the optical properties of the dust mixture used in the
    2D radiative transfer benchmark calculations of Pascucci et al. (2004, A&A, 417, 793). It
    consists of a single dust population of spherical astronomical silicate grains with a grain
    size of 0.12 micron. The data can be downloaded from the <a
    href="http://www.mpia.de/PSF/PSFpages/RT/benchmark.html"> benchmark pages</a> at MPIA, where
    also additional information on the 2D benchmark models can be found. Scattering is assumed to
    be isotropic. The extinction coefficients in the benchmark data are scale-free; we arbitrarily
    scale them to a reasonable order of magnitude. */
class Benchmark2DDustMix : public DustMix
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a dust mix from the 2D benchmark model (Pascucci et al. 2004)")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE Benchmark2DDustMix();

protected:
    /** This function reads the raw data from a resource file called
        <tt>Benchmark2DDustMix.dat</tt>, which contains a copy of the data taken from the MPIA web
        site. It then adds a single dust population to the dust mix, scaling the coefficients to a
        reasonable order of magnitude. */
    void setupSelfBefore();
};

////////////////////////////////////////////////////////////////////

#endif // BENCHMARK2DDUSTMIX_HPP
