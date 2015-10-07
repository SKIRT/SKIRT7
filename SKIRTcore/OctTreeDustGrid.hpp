/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef OCTTREEDUSTGRID_HPP
#define OCTTREEDUSTGRID_HPP

#include "TreeDustGrid.hpp"

//////////////////////////////////////////////////////////////////////

/** OctTreeDustGrid is a subclass of the TreeDustGrid class that
    implements an octtree dust grid (8 children per node). */
class OctTreeDustGrid : public TreeDustGrid
{
    Q_OBJECT
    Q_CLASSINFO("Title", "an octtree dust grid")

    Q_CLASSINFO("Property", "barycentric")
    Q_CLASSINFO("Title", "use barycentric subdivision")
    Q_CLASSINFO("Default", "no")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE OctTreeDustGrid();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the flag indicating geometric or barycentric subdivision of cells. If the flag is set
        to false (the default), cells are subdivided in their geometric center. If the flag is set
        to true, cells are subdivided in their center of mass (barycenter). */
    Q_INVOKABLE void setBarycentric(bool value);

    /** Returns the flag indicating geometric or barycentric subdivision of cells. */
    Q_INVOKABLE bool barycentric() const;

    //======================== Other Functions =======================

protected:
    /** This function creates a root node of type OctTreeNode using a node identifier of zero
        and the specified spatial extent, and returns a pointer to it. The caller must take
        ownership of the newly created object. */
    TreeNode* createRoot(const Box& extent);

    //======================== Data Members =======================

private:
    // discoverable attributes
    bool _barycentric;
};

//////////////////////////////////////////////////////////////////////

#endif // OCTTREEDUSTGRID_HPP
