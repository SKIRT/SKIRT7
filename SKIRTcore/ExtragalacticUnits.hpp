/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef EXTRAGALACTICUNITS_HPP
#define EXTRAGALACTICUNITS_HPP

#include "Units.hpp"

////////////////////////////////////////////////////////////////////

/** This class provides a system of units appropriate for an (extra-)galactic environment. For
    example, the unit of length is 1 pc and the unit of distance is 1 Mpc. */
class ExtragalacticUnits : public Units
{
    Q_OBJECT
    Q_CLASSINFO("Title", "extragalactic units (length in pc, distance in Mpc)")

    //============= Construction - Setup - Destruction =============

public:
    /** The constructor initializes the base class data members with values appropriate for this
        system of units */
    Q_INVOKABLE ExtragalacticUnits();
};

////////////////////////////////////////////////////////////////////

#endif // EXTRAGALACTICUNITS_HPP
