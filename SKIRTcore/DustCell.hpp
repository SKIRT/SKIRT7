/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef DUSTCELL_HPP
#define DUSTCELL_HPP

#include "Array.hpp"

//////////////////////////////////////////////////////////////////////

/** The DustCell class is an abstract class that gathers all the relevant data that should be
    stored in a single dust cell and some functions to read and update these quantities. The
    DustCell class is conceived as containing only the minimum of information that is absolutely
    necessary for running Monte Carlo simulations. Indeed, dust components can contain many dust
    cells and the memory usage of a Monte Carlo simulation is usually determined by the memory
    needed to build up the grid of dust cells. The information stored in each dust cell is the
    volume of the cell and the dust mass density corresponding to each dust component. Depending on
    the particular subclass of the abstract DustCell class, other properties can be stored. */
class DustCell
{
protected:
    /** This constructor takes the number \f$N_{\text{comp}}\f$ of dust components and creates the
        vector needed to store the density data. It is protected since this is an abstract class. */
    explicit DustCell(int Ncomp);

public:
    /** Destructs the dust cell. This virtual destructor is declared here because
        DustCell is the top-level class in the hierarchy of dust cells.*/
    virtual ~DustCell();

    /** This function returns the volume of the cell. */
    double volume() const;

    /** This function sets the volume of the cell. */
    void setvolume(double volume);

    /** This function returns the mass density of \f$i\f$'th component the cell. */
    double density(int i) const;

    /** This function returns the total mass density of the cell, which is just a sum of the
        density of the different components. */
    double density() const;

    /** This function sets the mass density of the \f$i\f$'th component of the cell. */
    void setdensity(int i, double rho);

    /** This function returns the absorbed luminosity at wavelength index \f$\ell\f$. It just sums
        the absorbed stellar and dust contributions. */
    double Labs(int ell) const;

    /** This pure virtual function returns the absorbed stellar luminosity at wavelength index \f$\ell\f$.
        */
    virtual double Labsstellar(int ell) const = 0;

    /** This pure virtual function returns the absorbed dust luminosity at wavelength index \f$\ell\f$.
        */
    virtual double Labsdust(int ell) const = 0;

    /** This pure virtual function resets the absorbed dust luminosity to zero at all wavelength indices. */
    virtual void rebootLabsdust() = 0;

    /** This pure virtual function simulates the absorption of a monochromatic luminosity package
        in the cell. The flag ynstellar indicates whether the luminosity package has a stellar origin or not. */
    virtual void absorb(int ell, double DeltaL, bool ynstellar) = 0;

private:
    double _volume;
    Array _rhov;
};

//////////////////////////////////////////////////////////////////////

#endif // DUSTCELL_HPP
