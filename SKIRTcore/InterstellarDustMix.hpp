/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef INTERSTELLARDUSTMIX_HPP
#define INTERSTELLARDUSTMIX_HPP

#include "DustMix.hpp"

////////////////////////////////////////////////////////////////////

/** The InterstellarDustMix class represents the optical properties of an average dust mixture that
    is appropriate for the typical interstellar dust medium. The data are read from a file called
    <tt>kext_albedo_WD_MW_3.1_60_D03.all</tt> on Bruce Draine's web page. The file contains data
    over a large wavelength range, from 0.0001 \f$\mu\f$m to 10 mm. The model consists of a mixture
    of carbonaceous grains and amorphous silicate grains. Carbonaceous grains are PAH-like when
    small, and graphite-like when large. (see Li & Draine 2001, ApJ, 554, 778). Size distributions
    are taken from Weingartner & Draine (2001, ApJ, 548, 296), more specifically, they refer to the
    case A model for \f$R_V=3.1\f$, renormalized following Draine (2003, ApJ, 598, 1017).
    Specifically, the grain abundances relative to H have been reduced by a factor 0.93 relative to
    the \f$R_V=3.1\f$ size distribution with \f$\text{[C/H]}_{\text{PAH}} = 60~{\text{ppm}}\f$ in
    Weingartner & Draine (2001, ApJ, 548, 296). The PAH C abundance relative to H is assumed to be
    \f$\text{[C/H]}_{\text{PAH}} = 0.93 \times 60~{\text{ppm}} = 55.8~{\text{ppm}}\f$. */
class InterstellarDustMix : public DustMix
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a dust mix with average interstellar properties")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE InterstellarDustMix();

protected:
    /** This function reads the optical properties from a resource file called
        <tt>InterstellarDustMix.dat</tt>, which contains a copy of the data taken from Bruce
        Draine's web page. The function directly uses the provided extinction coefficients rather
        than the cross sections to achieve exact backwards compatibility with earlier code for
        oligochromatic simulations. The function adds a single dust population with these
        properties to the dust mix, providing a unit dust mass since the values are already scaled.
        */
    void setupSelfBefore();
};

////////////////////////////////////////////////////////////////////

#endif // INTERSTELLARDUSTMIX_HPP
