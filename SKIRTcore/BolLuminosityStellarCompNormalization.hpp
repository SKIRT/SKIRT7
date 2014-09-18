/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef BOLLUMINOSITYSTELLARCOMPNORMALIZATION_HPP
#define BOLLUMINOSITYSTELLARCOMPNORMALIZATION_HPP

#include "StellarCompNormalization.hpp"

////////////////////////////////////////////////////////////////////

/** BolLuminosityStellarCompNormalization is a class that sets the normalization of a stellar
    component by defining the total bolometric luminosity. */
class BolLuminosityStellarCompNormalization : public StellarCompNormalization
{
    Q_OBJECT
    Q_CLASSINFO("Title", "stellar component normalization through the bolometric luminosity")

    Q_CLASSINFO("Property", "luminosity")
    Q_CLASSINFO("Title", "the bolometric luminosity for this component, in solar units")
    Q_CLASSINFO("MinValue", "0")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE BolLuminosityStellarCompNormalization();

protected:
    /** This function verifies the validity of the luminosity value and converts it to program
        units. */
    void setupSelfBefore();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the bolometric luminosity, given in solar units. */
    Q_INVOKABLE void setLuminosity(double value);

    /** Returns the bolometric luminosity, in solar units. */
    Q_INVOKABLE double luminosity() const;

    //======================== Other Functions =======================

public:
    /** This function returns the bolometric luminosity of a (virtual) stellar component that
        would have a given %SED. For the present type of normalization, this function is trivial as
        the bolometric luminosity is a data member. */
    double totluminosity(SED* sed) const;

    //======================== Data Members ========================

private:
    double _Ltot_Lsun;    // solar units
    double _Ltot_W;       // program units (W)
};

////////////////////////////////////////////////////////////////////

#endif // BOLLUMINOSITYSTELLARCOMPNORMALIZATION_HPP
