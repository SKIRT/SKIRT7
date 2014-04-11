/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef DRAINELIDUSTMIX_HPP
#define DRAINELIDUSTMIX_HPP

#include "DustMix.hpp"

////////////////////////////////////////////////////////////////////

/** The DraineLiDustMix class represents a dust mixture that formally consists of a single dust
    grain population, but that in reality represents a mixture of graphite, silicate and PAH dust
    grains. The size distribution of each of these dust grain populations is finetuned in such a
    way that the global dust properties accurately reproduce the extinction curve of the Milky Way.
    The optical properties of this dust mixture are generated using the DustEm code using the
    following input data:

    \verbatim
    # grain type, nsize, type keywords, Mdust/MH, rho, amin, amax, alpha/a0 [, at, ac, gamma (ED)] [, au, zeta, eta (CV)]
    # (cgs units)
    PAH0_DL07 10 mix-logn   5.40E-4 2.24E+0 3.10E-08 1.20E-7  3.50E-8 4.00E-1
    PAH1_DL07 10 mix-logn   5.40E-4 2.24E+0 3.10E-08 1.20E-7  3.50E-8 4.00E-1
    Gra       30 logn       1.80E-4 2.24E+0 3.10E-08 4.00E-6  3.00E-7 4.00E-1
    Gra       70 plaw-ed-cv 2.33E-3 2.24E+0 3.10E-08 2.00E-4 -2.54E+0 1.07E-6 4.28E-5 3.00E+0 1.07E-6 -1.65E-1 1.00E+0
    aSil      70 plaw-ed-cv 8.27E-3 3.50E+0 3.10E-08 2.00E-4 -3.21E+0 1.64E-5 1.00E-5 3.00E+0 1.64E-5  3.00E-1 1.00E+0
    \endverbatim
*/
class DraineLiDustMix : public DustMix
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a Draine & Li (2007) dust mix")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE DraineLiDustMix();

protected:
    /** This function reads the raw data from a resource file called <tt>DraineLiDustMix.dat</tt>,
        which was generated using the DustEm code. The file provides cross sections per hydrogen
        nucleon \f$C^\text{abs}/\text{H}\f$ and \f$C^\text{sca}/\text{H}\f$, which means we must
        provide the dust mass per hydrogen nucleon: \f[\frac{M_\text{dust}}{\text{H}} =
        \frac{M_\text{dust}}{M_\text{H}}\,\frac{M_\text{H}}{\text{H}} =
        \frac{M_\text{dust}}{M_\text{H}}\,m_\text{p}\f] where \f$M_\text{dust}/M_\text{H}\f$ is
        obtained by summing the values in the fourth column of the DustEM input data, and
        \f$m_\text{p}\f$ is the proton mass. Finally the function adds a single dust population
        with these properties to the dust mix. */
    void setupSelfBefore();
};

////////////////////////////////////////////////////////////////////

#endif // DRAINELIDUSTMIX_HPP
