/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef COMPSTELLARSYSTEM_HPP
#define COMPSTELLARSYSTEM_HPP

#include "ArrayTable.hpp"
#include "StellarSystem.hpp"
class PhotonPackage;
class Random;
class StellarComp;

//////////////////////////////////////////////////////////////////////

/** The CompStellarSystem class is a subclass of the general StellarSystem class and represents
    stellar systems that are the superposition of a number of stellar components. That is also how
    they are defined internally: they are basically a vector of pointers to objects of the
    StellarComp class. */
class CompStellarSystem : public StellarSystem
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a stellar system composed of various stellar components")

    Q_CLASSINFO("Property", "components")
    Q_CLASSINFO("Title", "the stellar components")
    Q_CLASSINFO("Default", "OligoStellarComp")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor; creates an empty stellar component system. */
    Q_INVOKABLE CompStellarSystem();

protected:
    /** This function verifies that at least one component has been added. */
    void setupSelfBefore();

    /** This function calculates and cashes luminosity information about the components for later use. */
    void setupSelfAfter();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** This function adds a stellar component to the system. */
    Q_INVOKABLE void addComponent(StellarComp* value);

    /** This function returns the list of stellar components in the system. */
    Q_INVOKABLE QList<StellarComp*> components() const;

    //======================== Other Functions =======================

public:
    /** This function returns the monochromatic luminosity \f$L_\ell\f$ of the stellar system at
        the wavelength index \f$\ell\f$. */
    double luminosity(int ell) const;

    /** This function returns the dimension of the stellar system, which depends on the (lack of)
        symmetry in the geometries of its stellar components. A value of 1 means spherical
        symmetry, 2 means axial symmetry and 3 means none of these symmetries. The stellar
        component with the least symmetry (i.e. the highest dimension) determines the result for
        the whole system. */
    int dimension() const;

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

////////////////////////////////////////////////////////////////////

#endif // COMPSTELLARSYSTEM_HPP
