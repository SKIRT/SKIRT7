/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef SED_HPP
#define SED_HPP

#include "Array.hpp"
#include "SimulationItem.hpp"

////////////////////////////////////////////////////////////////////

/** SED is the general abstract class that describes spectral energy distributions. An SED object
    is essentially a vector \f$L_\ell\f$ that contains the fraction of the total luminosity emitted
    in each of the wavelength bins of the simulation's wavelength grid. By definition, the
    \f$L_\ell\f$ vector is always normalized as \f[ \sum_{\ell=0}^{N_\lambda-1} L_\ell = 1. \f]
    Note that this is different from the emissivity \f$j(\lambda)\f$, which is the power emitted
    per unit wavelength. Each SED subclass is responsible for initializing the luminosity vector
    during setup by calling one of the functions provided for this purpose by this base class. */
class SED : public SimulationItem
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a spectral energy distribution")

    //============= Construction - Setup - Destruction =============

protected:
    /** The default constructor; it is protected since this is an abstract class. */
    SED();

    /** This function verifies that the luminosity vector has been set by the subclass. */
    void setupSelfAfter();

    //======================== Other Functions =======================

public:
    /** This function returns the luminosity \f$L_\ell\f$ at the wavelength index \f$\ell\f$. */
    double luminosity(int ell) const;

    /** This function returns the luminosity vector \f$L_\ell\f$ for all wavelengths in the
        simulation's wavelength grid. */
    const Array& luminosities() const;

    /** This function writes the %SED to a file such that it can easily be plotted. Note that it
        applies a conversion: it plots the normalized (dimensionless) wavelength-bin-averaged
        emissivity \f$\lambda\,j(\lambda)\f$, which relates to the internally stored luminosity
        vector \f$L_\lambda\f$ through \f[ (\lambda\,j(\lambda))_\ell =
        \frac{\lambda_\ell\,L_\ell}{(\Delta \lambda)_\ell} \f] with \f$(\Delta\lambda)_\ell\f$ the
        width of the \f$\ell\f$'th wavelength bin. */
    void write(const QString &filename) const;

protected:
    /** This function initializes the luminosity vector \f$L_\ell\f$; it should be called during
        setup from a subclass. The argument specifies a vector of (not necessarily normalized)
        luminosities \f$L_\ell\f$ defined on the simulation's wavelength grid. Hence the vector
        must have the same length as the simulation's wavelength grid. The function normalizes the
        luminosities so that \f[ \sum_{\ell=\ell_{\text{min}}}^{\ell_{\text{max}}} L_\ell = 1. \f]
        */
    void setluminosities(const Array& Lv);

    /** This function initializes the luminosity vector \f$L_\ell\f$ from a vector of emissivities
        \f$j_\ell\f$ (the power emitted per unit wavelength) defined on the simulation's wavelength
        grid. Hence the vector must have the same length as the simulation's wavelength grid.
        Specifically the element \f$j_\ell\f$ should contain the mean of the emissivity over the
        \f$\ell\f$'th wavelength bin: \f[ j_\ell = \dfrac{ \int_{\Delta_\ell} j(\lambda)\,
        {\text{d}}\lambda }{ \int_{\Delta_\ell} {\text{d}}\lambda } = \frac{1}{\Delta_\ell}
        \int_{\Delta_\ell} j(\lambda)\, {\text{d}}\lambda. \f] The function calculates the
        luminosities as \f[ L_\ell = j_\ell\,\Delta_\ell \f] and then calls the setluminosities()
        function. */
    void setemissivities(const Array& jv);

    /** This function initializes the luminosity vector \f$L_\ell\f$ from a vector of emissivities
        \f$j_{\lambda}\f$ defined on some arbitrary specified wavelength grid. The function first
        resamples the emissivities on the simulation's wavelength grid, and then calls the other
        setemissivities() function with a single argument to actually initialize the luminosity
        vector. The value of the %SED at each of the grid points is determined by log-log
        interpolation. For grid points that fall beyond the original grid on which the %SED is
        sampled, the value is set to zero. */
    void setemissivities(const Array& lambdav, const Array& jv);

    //======================== Data Members ========================

private:
    Array _Lv;
};

////////////////////////////////////////////////////////////////////

#endif // SED_HPP
