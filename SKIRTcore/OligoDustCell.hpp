/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef OLIGODUSTCELL_HPP
#define OLIGODUSTCELL_HPP

#include "DustCell.hpp"

//////////////////////////////////////////////////////////////////////

/** The OligoDustCell class is a subclass of the general DustCell class and describes dust cells
    that are to be used in oligochromatic Monte Carlo simulations. It does not contain more
    information than what is already contained in the general DustCell. */
class OligoDustCell : public DustCell
{
public:

    /** This constructor takes the number \f$N_{\text{comp}}\f$ of dust components and calls the
        DustCell constructor with this information. */
    explicit OligoDustCell(int Ncomp);

    /** The function returns the absorbed stellar luminosity at wavelength index \f$\ell\f$. For
        oligochromatic dust cells, this function should never be used; when it is called, a
        FatalError is thrown. */
    double Labsstellar(int ell) const;

    /** The function returns the absorbed dust luminosity at wavelength index \f$\ell\f$. For
        oligochromatic dust cells, this function should never be used; when it is called, a
        FatalError is thrown. */
    double Labsdust(int ell) const;

    /** This function resets the absorbed dust luminosity to zero at all wavelength indices. For
        oligochromatic dust cells, this function should never be used; when it is called, a
        FatalError is thrown. */
    void rebootLabsdust();

    /** The function simulates the absorption of a monochromatic luminosity package in the cell. For
        oligochromatic dust cells, this function should never be used; when it is called, a
        FatalError is thrown. */
    void absorb(int ell, double DeltaL, bool ynstellar);
};

//////////////////////////////////////////////////////////////////////

#endif // OLIGODUSTCELL_HPP
