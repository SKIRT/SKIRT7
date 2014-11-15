/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef STELLARSYSTEM_HPP
#define STELLARSYSTEM_HPP

#include "ArrayTable.hpp"
#include "SimulationItem.hpp"
class PhotonPackage;
class Random;
class StellarComp;

//////////////////////////////////////////////////////////////////////

/** An instance of the StellarSystem class represents a complete stellar system, which is the
    superposition of one or more stellar components. Each stellar component provides a complete
    description of the spatial and spectral distribution of the stars (or any other primary source
    of radiation, such as an AGN). The main function of this class is to manage a list of objects
    of type StellarComp, and to implement the superposition of the distributions defined in these
    objects. */
class StellarSystem : public SimulationItem
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a stellar system")

    Q_CLASSINFO("Property", "components")
    Q_CLASSINFO("Title", "the stellar components")
    Q_CLASSINFO("Default", "GeometricStellarComp")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor; creates an empty stellar system. */
    Q_INVOKABLE StellarSystem();

protected:
    /** This function verifies that at least one component has been added. */
    void setupSelfBefore();

    /** This function calculates and cashes luminosity information about the components for later
        use. */
    void setupSelfAfter();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** This function inserts a stellar component into the stellar system at the specified index. */
    Q_INVOKABLE void insertComponent(int index, StellarComp* value);

    /** This function removes the stellar component with the specified index from the stellar system. */
    Q_INVOKABLE void removeComponent(int index);

    /** This function returns the list of stellar components in the system. */
    Q_INVOKABLE QList<StellarComp*> components() const;

    //======================== Other Functions =======================

public:
    /** This function returns the dimension of the stellar system, which depends on the (lack of)
        symmetry in the geometries of its stellar components. A value of 1 means spherical
        symmetry, 2 means axial symmetry and 3 means none of these symmetries. The stellar
        component with the least symmetry (i.e. the highest dimension) determines the result for
        the whole system. */
    int dimension() const;

    /** This function returns the number of components in the stellar system. */
    int Ncomp() const;

    /** This function returns the monochromatic luminosity \f$L_\ell\f$ of the stellar system at
        the wavelength index \f$\ell\f$, which is the sum of the luminosities of the stellar
        components in the system. */
    double luminosity(int ell) const;

    /** This function simulates the emission of a monochromatic photon package with a monochromatic
        luminosity \f$L\f$ at wavelength index \f$\ell\f$ from the stellar system. It randomly
        chooses a stellar component from which to emit the photon and then simulates the emission
        through the corresponding StellarComp::launch() function. */
    void launch(PhotonPackage* pp, int ell, double L) const;

    //======================== Data Members ========================

private:
    QList<StellarComp*> _scv;
    Array _Lv;
    ArrayTable<2> _Xvv;

    Random* _random;
};

////////////////////////////////////////////////////////////////

#endif // STELLARSYSTEM_HPP
