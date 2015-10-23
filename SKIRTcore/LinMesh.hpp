/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef LINMESH_HPP
#define LINMESH_HPP

#include "MoveableMesh.hpp"

//////////////////////////////////////////////////////////////////////

/** The LinMesh class represents meshes on the unit interval \f$[0,1]\f$ with a linear distribution
    of the mesh points. */
class LinMesh : public MoveableMesh
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a linear mesh")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE LinMesh();

    //======================== Other Functions =======================

public:
    /** This function returns an array containing the mesh points. */
    Array mesh() const;

};

//////////////////////////////////////////////////////////////////////

#endif // LINMESH_HPP
