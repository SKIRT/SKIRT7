/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef STELLARUNITS_HPP
#define STELLARUNITS_HPP

#include "Units.hpp"

////////////////////////////////////////////////////////////////////

/** This class provides a system of units appropriate for a stellar environment. For example, the
    unit of length is 1 AU and the unit of distance is 1 parsec. */
class StellarUnits : public Units
{
    Q_OBJECT
    Q_CLASSINFO("Title", "stellar units (length in AU, distance in pc)")

    //============= Construction - Setup - Destruction =============

public:
    /** The constructor initializes the base class data members with values appropriate for this
        system of units */
    Q_INVOKABLE StellarUnits();
};

////////////////////////////////////////////////////////////////////

#endif // STELLARUNITS_HPP
