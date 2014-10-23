/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef GENGEOMETRY_HPP
#define GENGEOMETRY_HPP

#include "Geometry.hpp"

////////////////////////////////////////////////////////////////////

/** The GenGeometry class is an abstract subclass of the Geometry class, and serves
    as a base class for general geometries without a specific symmetry. */
class GenGeometry : public Geometry
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a geometry without a specific symmetry")

    //============= Construction - Setup - Destruction =============

protected:
    /** The default constructor; it is protected since this is an abstract class. */
    GenGeometry();

    //======================== Other Functions =======================

public:
    /** This function returns the dimension of the geometry, which is 3 for all subclasses of this
        class since it is a base class for geometries without a specific symmetry. */
    int dimension() const;
};

////////////////////////////////////////////////////////////////////

#endif // GENGEOMETRY_HPP
