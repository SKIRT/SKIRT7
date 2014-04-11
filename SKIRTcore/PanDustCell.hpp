/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef PANDUSTCELL_HPP
#define PANDUSTCELL_HPP

#include "DustCell.hpp"

//////////////////////////////////////////////////////////////////////

/** The PanDustCell class is a subclass of the general DustCell class and describes dust cells that
    are to be used in panchromatic Monte Carlo simulations. Apart from the information already
    contained the generic DustCell class, each dust cell now also has an array for recording the
    absorbed luminosity at each wavelength index of the global wavelength grid. */
class PanDustCell : public DustCell
{
public:
    /** This constructor calls the DustCell constructor with the number of dust components
        \f$N_{\text{comp}}\f$. It also constructs the absorbed luminosity vector with the number of
        wavelengths \f$N_{\lambda}\f$ and initializes all elements to zero. */
    PanDustCell(int Ncomp, int Nlambda);

    /** This function returns the absorbed stellar luminosity at wavelength index \f$\ell\f$. It just reads
        the appropriate number from the internally stored vector. */
    double Labsstellar(int ell) const;

    /** This function returns the absorbed dust luminosity at wavelength index \f$\ell\f$. It just reads
        the appropriate number from the internally stored vector. */
    double Labsdust(int ell) const;

    /** This function resets the absorbed dust luminosity to zero at all wavelength indices. */
    void rebootLabsdust();

    /** This function simulates the absorption of a monochromatic luminosity package in the cell.
        It increases the absorbed luminosity counter at the \f$\ell\f$'th wavelength index by
        \f$\Delta L\f$. The flag ynstellar indicates whether the luminosity package has a stellar
        origin or not. */
    void absorb(int ell, double DeltaL, bool ynstellar);

private:
    Array _Labsstellarv;
    Array _Labsdustv;
};

//////////////////////////////////////////////////////////////////////

#endif // PANDUSTCELL_HPP
