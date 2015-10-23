/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef MOVEABLEMESH_HPP
#define MOVEABLEMESH_HPP

#include "Mesh.hpp"

//////////////////////////////////////////////////////////////////////

/** The MoveableMesh class represents meshes that can be moved (offset) in addition to being
    scaled. This class does not add any functionality, it just serves to differentiate anchored
    meshes (which can be meaningfully employed only in the radial direction) and moveable meshes
    (which can be employed anywhere). */
class MoveableMesh : public Mesh
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a moveable mesh")

    //============= Construction - Setup - Destruction =============

protected:
    /** The default constructor; it is protected since this is an abstract class. */
    MoveableMesh();
};

//////////////////////////////////////////////////////////////////////

#endif // MOVEABLEMESH_HPP
