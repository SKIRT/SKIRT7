/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef OCTTREENODE_HPP
#define OCTTREENODE_HPP

#include "TreeNode.hpp"

//////////////////////////////////////////////////////////////////////

/** OctTreeNode is a TreeNode subclass that represents nodes in an OctTreeDustGridStructure. It
    implements a regular geometric subdivision scheme, and can be subclassed to implement a more
    complicated scheme. */
class OctTreeNode : public TreeNode
{
public:
    /** This constructor creates a new octtree node with the specified father node, identifier, and
        spatial extent (defined by the coordinates of the corner points). The constructor sets the
        level of the new node to be one higher than the level of the father. If the pointer to the
        father is null, the level of the new cell is zero. */
    OctTreeNode(TreeNode* father, int id, const Box& extent);

protected:
    /** This function creates a fresh new node of class OctTreeNode, i.e. the same type as the
        receiving node. The arguments are the same as those for the constructor. Ownership for the
        new node is passed to the caller. */
    virtual TreeNode* createnode(TreeNode* father, int id, const Box& extent);

    /** This function is for use here and in subclasses. It creates eight new nodes subdividing the
        node at the specified point, and adds these new nodes as its own child nodes. The children
        are added in the following order: back-left-bottom (0), front-left-bottom (1),
        back-right-bottom (2), front-right-bottom (3), back-left-top (4), front-left-top (5),
        back-right-top (6), front-right-top (7). The children are assigned consecutive integer
        identifiers in this same order. The first child receives the identifier specified as an
        argument to this function, the second child receives the specified identifier plus one, and
        so on, and the last child receives the specified identifier plus seven. A node does NOT
        take ownership of its children, so the caller is responsible for deleting the child nodes
        when they are no longer needed. */
    void createchildren_splitpoint(int id, Vec r);

public:
    /** This function creates eight new nodes subdividing the node at the geometric center, and
        adds these new nodes as its own child nodes. It invokes createchildren_splitpoint() to
        accomplish its task. */
    void createchildren(int id);

    /** This function creates child nodes exactly in the same way as the createchildren() function
        without a calculator argument. It can be overridden in a subclass to implement more complex
        subdivision scheme. */
    void createchildren(int id, const TreeNodeDensityCalculator* calc);

    /** This function adds the relevant neighbors to a node with children (the function does nothing
        if the node doesn't have any children). It considers internal neighbors (each of the 8
        children has 3 neighbors among its siblings) as well as the neighbors of the father node
        (i.e. this node). The inherited neighbors must be distributed among the children depending
        on the geometry; note that a particular neighbor may be inherited by multiple children. */
    void addneighbors();

    /** This function returns a pointer to the node's child that contains the specified point. More
        accurately, it returns the child corresponding to the quadrant that contains the specified
        point relative to the node's central division point. If the specified point is inside the
        node, then it will also be inside the returned child. This function crashes if the node is
        childless. */
    TreeNode* child(Vec r) const;
};

//////////////////////////////////////////////////////////////////////

#endif // OCTTREENODE_HPP
