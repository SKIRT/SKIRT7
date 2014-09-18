/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef GEOMETRICSTELLARCOMP_HPP
#define GEOMETRICSTELLARCOMP_HPP

#include "Array.hpp"
#include "StellarComp.hpp"
class PhotonPackage;
class Geometry;

//////////////////////////////////////////////////////////////////////

/** GeometricStellarComp represents a stellar component in which the spatial distribution of stars
    is characterized by a built-in geometry. This abstract class handles an instance of the
    Geometry class to define the spatial distribution. Each subclasses is expected to define the
    spectral energy distribution of the stars (which is constant across the spatial distribution)
    and some form of normalization to specify the total luminosity of the component. */
class GeometricStellarComp : public StellarComp
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a stellar component with a built-in geometry")

    Q_CLASSINFO("Property", "geometry")
    Q_CLASSINFO("Title", "the geometry of the spatial stellar distribution")
    Q_CLASSINFO("Default", "PointGeometry")

    //============= Construction - Setup - Destruction =============

protected:
    /** The default constructor; it is protected since this is an abstract class. */
    GeometricStellarComp();

    /** This function verifies that the geometry has been appropriately set. */
    void setupSelfBefore();

    /** This function verifies that the subclass added the same number of luminosities as there are
        wavelengths in the simulation's wavelength grid. */
    void setupSelfAfter();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** This function sets the geometry for this stellar component. */
    Q_INVOKABLE void setGeometry(Geometry* value);

    /** This function returns a pointer to the geometry for this stellar component. */
    Q_INVOKABLE Geometry* geometry() const;

    //======================== Other Functions =======================

public:
    /** This function returns the dimension of the stellar component, which depends on the (lack of)
        symmetry of its geometry. A value of 1 means spherical symmetry, 2 means axial symmetry and
        3 means none of these symmetries. */
    int dimension() const;

    /** This function returns the monochromatic luminosity \f$L_\ell\f$ of the stellar component at
        the wavelength index \f$\ell\f$. It is found as \f$L_\ell = L_{\text{tot}}\, S_\ell\f$,
        with \f$L_{\text{tot}}\f$ the total bolometric luminosity and \f$S_\ell\f$ the value of the
        SED at the wavelength index \f$\ell\f$. */
    double luminosity(int ell) const;

    /** This function simulates the emission of a monochromatic photon package with a monochromatic
        luminosity \f$L\f$ at wavelength index \f$\ell\f$ from the stellar component. The position
        and propagation direction of the emission are determined randomly from the geometry of the
        stellar component. */
    void launch(PhotonPackage* pp, int ell, double L) const;

    //======================== Data Members ========================

private:
    Geometry* _geom;

protected:
    // subclasses should set the luminosities in this vector (one for each wavelength)
    Array _Lv;
};

//////////////////////////////////////////////////////////////////////

#endif // GEOMETRICSTELLARCOMP_HPP
