/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef LYASPECTRUM_HPP
#define LYASPECTRUM_HPP

#include "Array.hpp"
#include "SimulationItem.hpp"

////////////////////////////////////////////////////////////////////

/** LyaSpectrum is an abstract class that describes the spectrum emission of a source to be
    used in Lyα radiative transfer simulations. A LyaSpectrum object
    is essentially a vector \f$L_\ell\f$ that contains the total luminosity emitted at each of
    the grid point in the wavelength grid (or equivalently, at each of the velocity grid points).
    Each LyaSpectrum subclass is responsible for initializing the luminosity vector
    during setup by calling one of the functions provided for this purpose by this base class. */
class LyaSpectrum : public SimulationItem
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a Lyα emission spectrum")

    //============= Construction - Setup - Destruction =============

protected:
    /** The default constructor; it is protected since this is an abstract class. */
    LyaSpectrum();

    /** This function verifies that the luminosity vector has been set by the subclass. */
    void setupSelfAfter();

    //======================== Other Functions =======================

public:
    /** This function returns the luminosity \f$L_\ell\f$ at the wavelength index \f$\ell\f$. */
    double luminosity(int ell) const;

    /** This function returns the luminosity vector \f$L_\ell\f$ for all wavelengths in the
        simulation's wavelength grid. */
    const Array& luminosities() const;

protected:
    /** This function initializes the luminosity vector \f$L_\ell\f$; it should be called during
        setup from a subclass. The argument specifies a vector of luminosities \f$L_\ell\f$ defined
        on the simulation's wavelength grid. */
    void setluminosities(const Array& Lv);

    //======================== Data Members ========================

private:
    Array _Lv;
};

////////////////////////////////////////////////////////////////////

#endif // LYASPECTRUM_HPP
