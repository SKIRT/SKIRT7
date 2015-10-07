/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef LINMESH_HPP
#define LINMESH_HPP

#include "Mesh.hpp"

//////////////////////////////////////////////////////////////////////

/** The LinMesh class represents meshes on the unit interval \f$[0,1]\f$ with a linear distribution
    of the mesh points. */
class LinMesh : public Mesh
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a linear mesh")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE LinMesh();

    /** This function sets up the mesh by calling the lingrid function in the NR class. */
    void setupSelfBefore();

};

//////////////////////////////////////////////////////////////////////

#endif // LINMESH_HPP
