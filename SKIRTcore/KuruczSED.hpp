/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef KURUCZSED_HPP
#define KURUCZSED_HPP

#include "StellarSED.hpp"

////////////////////////////////////////////////////////////////////

/** KuruczSED is a class that represents spectral energy distributions of stars according to the
    model of Kurucz (1993). SSPs with different metallicities, effective temperatures and surface
    gravities can be chosen. */
class KuruczSED : public StellarSED
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a Kurucz SED")

    Q_CLASSINFO("Property", "metallicity")
    Q_CLASSINFO("Title", "the metallicity")
    Q_CLASSINFO("MinValue", "-5")
    Q_CLASSINFO("MaxValue", "1")
    Q_CLASSINFO("Default", "0.02")

    Q_CLASSINFO("Property", "temperature")
    Q_CLASSINFO("Title", "the effective temperature")
    Q_CLASSINFO("Quantity", "temperature")
    Q_CLASSINFO("MinValue", "3500")
    Q_CLASSINFO("MaxValue", "10000")
    Q_CLASSINFO("Default", "6000")

    Q_CLASSINFO("Property", "gravity")
    Q_CLASSINFO("Title", "the surface gravity")
    Q_CLASSINFO("MinValue", "0")
    Q_CLASSINFO("MaxValue", "5")
    Q_CLASSINFO("Default", "2")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE KuruczSED();

protected:
    /** The SKIRT resource data includes a huge library with Kurucz model atmospheres on a 3D
        parameter space grid in metallicity, effective temperature and surface gravity. This
        function first determines the metallicity from the metallicity grid that is closest to the
        desired metallicity \f$\mu\f$. The same goes for the surface gravity \f$g\f$. Once these
        values are known, the correct file is searched and the constructor calculates a vector with
        the %SED by interpolating between the two SEDs with effective temperatures that bracket the
        desired effective temperature \f$T_{\text{eff}}\f$. This vector is then regridded on the
        global wavelength grid. */
    void setupSelfBefore();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the metallicity. */
    Q_INVOKABLE void setMetallicity(double value);

    /** Returns the metallicity. */
    Q_INVOKABLE double metallicity() const;

    /** Sets the effective temperature. */
    Q_INVOKABLE void setTemperature(double value);

    /** Returns the effective temperature. */
    Q_INVOKABLE double temperature() const;

    /** Sets the surface gravity. */
    Q_INVOKABLE void setGravity(double value);

    /** Returns the surface gravity. */
    Q_INVOKABLE double gravity() const;

    //======================== Data Members ========================

private:
    double _Z;
    double _Teff;
    double _g;
};

////////////////////////////////////////////////////////////////////

#endif // KURUCZSED_HPP
