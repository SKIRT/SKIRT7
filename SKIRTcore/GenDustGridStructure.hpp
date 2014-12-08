/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef GENDUSTGRIDSTRUCTURE_HPP
#define GENDUSTGRIDSTRUCTURE_HPP

#include "DustGridStructure.hpp"

//////////////////////////////////////////////////////////////////////

/** The GenDustGridStructure class is an abstract subclass of the DustGridStructure class, and
    serves as a base class for general dust grids without a specific symmetry. */
class GenDustGridStructure : public DustGridStructure
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a dust grid structure without a specific symmetry")

    //============= Construction - Setup - Destruction =============

protected:
    /** The default constructor; it is protected since this is an abstract class. */
    GenDustGridStructure();

    //======================== Other Functions =======================

public:
    /** This function returns the dimension of the grid, which is 3 for all subclasses of this
        class since it is a base class for grids without a specific symmetry. */
    int dimension() const;
};

//////////////////////////////////////////////////////////////////////

#endif // GENDUSTGRIDSTRUCTURE_HPP
