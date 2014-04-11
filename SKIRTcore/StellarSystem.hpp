/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef STELLARSYSTEM_HPP
#define STELLARSYSTEM_HPP

#include "SimulationItem.hpp"
class PhotonPackage;

//////////////////////////////////////////////////////////////////////

/** The StellarSystem class is the abstract base class for objects that simulate a complete stellar
    system. Such a system should contain a complete description of the spatial and spectral
    distribution of the stars (or any other primary source of radiation, such as an AGN). */
class StellarSystem : public SimulationItem
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a stellar system")

    //============= Construction - Setup - Destruction =============

protected:
    /** The default constructor; it is protected since this is an abstract class. */
    StellarSystem();

    //======================== Other Functions =======================

public:
    /** This pure virtual function returns the monochromatic luminosity \f$L_\ell\f$ of the stellar
        system at the wavelength index \f$\ell\f$. Its implementation must be provided in a
        subclass. */
    virtual double luminosity(int ell) const = 0;

    /** This pure virtual function returns the dimension of the stellar system, which depends on
        the (lack of) symmetry in its geometry. A value of 1 means spherical symmetry, 2 means
        axial symmetry and 3 means none of these symmetries. The function's implementation must be
        provided in a subclass. */
    virtual int dimension() const = 0;

    /** This pure virtual function simulates the emission of a monochromatic photon package with a
        monochromatic luminosity \f$L\f$ at wavelength index \f$\ell\f$ from the stellar system.
        Its implementation must be provided in a subclass. */
    virtual void launch(PhotonPackage* pp, int ell, double L) const = 0;
};

////////////////////////////////////////////////////////////////

#endif // STELLARSYSTEM_HPP
