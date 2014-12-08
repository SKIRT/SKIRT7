/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef STELLARCOMP_HPP
#define STELLARCOMP_HPP

#include "SimulationItem.hpp"
class PhotonPackage;

//////////////////////////////////////////////////////////////////////

/** StellarComp is an abstract class used to represent a stellar component. A stellar
    component is characterized by a geometrical distribution of stars, the spectral energy
    distribution of each star, and the total luminosity of the system. */
class StellarComp : public SimulationItem
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a stellar component")

    //============= Construction - Setup - Destruction =============

protected:
    /** The default constructor; it is protected since this is an abstract class. */
    StellarComp();

    //======================== Other Functions =======================

public:
    /** This function returns the dimension of the stellar component, which depends on the (lack of)
        symmetry of its geometry. A value of 1 means spherical symmetry, 2 means axial symmetry and
        3 means none of these symmetries. */
    virtual int dimension() const = 0;

    /** This function returns the monochromatic luminosity \f$L_\ell\f$ of the stellar component at
        the wavelength index \f$\ell\f$. */
    virtual double luminosity(int ell) const = 0;

    /** This function simulates the emission of a monochromatic photon package with a monochromatic
        luminosity \f$L\f$ at wavelength index \f$\ell\f$ from the stellar component. */
    virtual void launch(PhotonPackage* pp, int ell, double L) const = 0;
};

//////////////////////////////////////////////////////////////////////

#endif // STELLARCOMP_HPP
