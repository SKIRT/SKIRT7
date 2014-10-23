/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef BRUZUALCHARLOTSED_HPP
#define BRUZUALCHARLOTSED_HPP

#include "StellarSED.hpp"

////////////////////////////////////////////////////////////////////

/** BruzualCharlotSED is a class that represents spectral energy distributions of simple stellar
    populations (SSPs), parameterized on metallicity and age according to the model of Bruzual &
    Charlot (2003). See the BruzualCharlotSEDFamily class for more information. */
class BruzualCharlotSED : public StellarSED
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a Bruzual & Charlot simple stellar population SED")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE BruzualCharlotSED();

    Q_CLASSINFO("Property", "metallicity")
    Q_CLASSINFO("Title", "the metallicity of the SSP")
    Q_CLASSINFO("MinValue", "0.0001")
    Q_CLASSINFO("MaxValue", "0.05")
    Q_CLASSINFO("Default", "0.02")

    Q_CLASSINFO("Property", "age")
    Q_CLASSINFO("Title", "the age of the SSP (in Gyr)")
    Q_CLASSINFO("MinValue", "0")
    Q_CLASSINFO("MaxValue", "20")
    Q_CLASSINFO("Default", "5")

protected:
    /** This function constructs a temporary instance of the BruzualCharlotSEDFamily class to
        obtain an SED that corresponds to the values of the metallicity and age attributes. */
    void setupSelfBefore();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the metallicity of the SSP. */
    Q_INVOKABLE void setMetallicity(double value);

    /** Returns the metallicity of the SSP. */
    Q_INVOKABLE double metallicity() const;

    /** Sets the age of the SSP (in Gyr). */
    Q_INVOKABLE void setAge(double value);

    /** Returns the age of the SSP (in Gyr). */
    Q_INVOKABLE double age() const;

private:
    double _Z;
    double _t;
};

////////////////////////////////////////////////////////////////////

#endif // BRUZUALCHARLOTSED_HPP
