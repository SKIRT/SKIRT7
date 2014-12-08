/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef BARYBINTREENODE_HPP
#define BARYBINTREENODE_HPP

#include "BinTreeNode.hpp"

//////////////////////////////////////////////////////////////////////

/** BaryBinTreeNode is a TreeNode subclass that represents nodes in a BinTreeDustGridStructure, using
    the barycenter of a node for determining the orientation of its subdivision plane.
*/
class BaryBinTreeNode : public BinTreeNode
{
public:
    /** This constructor creates a new BaryBinTreeNode node with the specified father node,
        identifier, and spatial extent (defined by the coordinates of the corner points). The
        constructor sets the level of the new node to be one higher than the level of the father.
        If the pointer to the father is null, the level of the new cell is zero. */
    BaryBinTreeNode(TreeNode* father, int id, const Box& extent);

protected:
    /** This function creates a fresh new node of class BaryBinTreeNode, i.e. the same type as the
        receiving node. The arguments are the same as those for the constructor. Ownership for the
        new node is passed to the caller. */
    virtual TreeNode* createnode(TreeNode* father, int id, const Box& extent);

public:
    /** This function creates two new nodes subdividing the node at its geometric center along a
        plane perpendicular to one of the coordinate axes, depending on the position of the
        barycenter. The function finds the x, y, or z direction in which the barycenter is nearest
        to one of the cell walls, and places the dividing plane perpendicular to this axes, through
        the specified point. Finally the function invokes createchildren_splitdir() to actually
        create the child nodes. */
    void createchildren(int id, const TreeNodeDensityCalculator* calc);
};

//////////////////////////////////////////////////////////////////////

#endif // BARYBINTREENODE_HPP
