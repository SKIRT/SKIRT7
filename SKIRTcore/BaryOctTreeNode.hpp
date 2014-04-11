/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef BARYOCTTREENODE_HPP
#define BARYOCTTREENODE_HPP

#include "OctTreeNode.hpp"

//////////////////////////////////////////////////////////////////////

/** BaryOctTreeNode is a TreeNode subclass that represents nodes in an OctTreeDustGridStructure
    using barycentric subdivision. */
class BaryOctTreeNode : public OctTreeNode
{
public:
    /** This constructor creates a new barycentric octtree node with the specified father node,
        identifier, and spatial extent (defined by the coordinates of the corner points). The
        constructor sets the level of the new node to be one higher than the level of the father.
        If the pointer to the father is null, the level of the new cell is zero. */
    BaryOctTreeNode(TreeNode* father, int id, const Box& extent);

protected:
    /** This function creates a fresh new node of class BaryOctTreeNode, i.e. the same type as the
        receiving node. The arguments are the same as those for the constructor. Ownership for the
        new node is passed to the caller. */
    virtual TreeNode* createnode(TreeNode* father, int id, const Box& extent);

public:
    /** This function creates eight new nodes subdividing the node at the barycenter, and adds
        these new nodes as its own child nodes. It invokes createchildren_splitpoint() to
        accomplish its task. */
    void createchildren(int id, const TreeNodeDensityCalculator* calc);
};

//////////////////////////////////////////////////////////////////////

#endif // BARYOCTTREENODE_HPP
