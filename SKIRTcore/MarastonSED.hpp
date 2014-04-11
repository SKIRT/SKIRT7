/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef MARASTONSED_HPP
#define MARASTONSED_HPP

#include "StellarSED.hpp"

////////////////////////////////////////////////////////////////////

/** MarastonSED is a class that represents spectral energy distributions of simple stellar
    populations (SSPs) according to the model of Maraston. SSPs with different ages and
    metallicities can be chosen. */
class MarastonSED : public StellarSED
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a Maraston simple stellar population SED")

    Q_CLASSINFO("Property", "age")
    Q_CLASSINFO("Title", "the age of the SSP (in Gyr)")
    Q_CLASSINFO("MinValue", "1e-6")
    Q_CLASSINFO("MaxValue", "15")
    Q_CLASSINFO("Default", "5")

    Q_CLASSINFO("Property", "metallicity")
    Q_CLASSINFO("Title", "the metallicity of the SSP")
    Q_CLASSINFO("MinValue", "0.0001")
    Q_CLASSINFO("MaxValue", "0.07")
    Q_CLASSINFO("Default", "0.02")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE MarastonSED();

protected:
    /** This function reads fluxes from a set of resource files and calculates a vector with the
        %SED by interpolating between the four SSPs with ages and metallicities that bracket the
        desired age \f$\tau\f$ and the desired metallicity \f$Z\f$. This vector is regridded on the
        global wavelength grid. */
    void setupSelfBefore();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the age of the SSP (in Gyr). */
    Q_INVOKABLE void setAge(double value);

    /** Returns the age of the SSP (in Gyr). */
    Q_INVOKABLE double age() const;

    /** Sets the metallicity of the SSP. */
    Q_INVOKABLE void setMetallicity(double value);

    /** Returns the metallicity of the SSP. */
    Q_INVOKABLE double metallicity() const;

    //======================== Data Members ========================

private:
    double _tau;
    double _Z;
};

////////////////////////////////////////////////////////////////////

#endif // MARASTONSED_HPP
