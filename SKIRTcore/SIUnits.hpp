/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef SIUNITS_HPP
#define SIUNITS_HPP

#include "Units.hpp"

////////////////////////////////////////////////////////////////////

/** This class provides a system of standard SI units. For example, the units of length and
    distance are both meter. */
class SIUnits : public Units
{
    Q_OBJECT
    Q_CLASSINFO("Title", "SI units")

    //============= Construction - Setup - Destruction =============

public:
    /** The constructor initializes the base class data members with values appropriate for this
        system of units */
    Q_INVOKABLE SIUnits();
};

////////////////////////////////////////////////////////////////////

#endif // SIUNITS_HPP
