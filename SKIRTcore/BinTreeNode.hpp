/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef BINTREENODE_HPP
#define BINTREENODE_HPP

#include "TreeNode.hpp"

//////////////////////////////////////////////////////////////////////

/** BinTreeNode is a TreeNode subclass that represents nodes in a BinTreeDustGridStructure. It
    implements a regular alternating subdivision scheme, and can be subclassed to implement a more
    complicated scheme.*/
class BinTreeNode : public TreeNode
{
public:
    /** This constructor creates a new BinTree node with the specified father node, identifier, and
        spatial extent (defined by the coordinates of the corner points). The constructor sets the
        level of the new node to be one higher than the level of the father. If the pointer to the
        father is null, the level of the new cell is zero. */
    BinTreeNode(TreeNode* father, int id, const Box& extent);

protected:
    /** This function creates a fresh new node of class BinTreeNode, i.e. the same type as the
        receiving node. The arguments are the same as those for the constructor. Ownership for the
        new node is passed to the caller. */
    virtual TreeNode* createnode(TreeNode* father, int id, const Box& extent);

    /** This function is for use here and in subclasses. It creates two new nodes subdividing the
        node along a plane perpendicular to the coordinate axis specified by \em dir (0=x, 1=y,
        2=z) at the geometric center. The two children are added in "ascending" order, i.e.\ left,
        right (when splitting along a plane perpendicular to the x axis); back, front (when
        perpendicular to the y axis); bottom, top (when perpendicular to the z axis). The first
        child receives the identifier specified as an argument to this function, and the second
        child receives the specified identifier plus one. A node does NOT take ownership of its
        children, so the caller is responsible for deleting the child nodes when they are no longer
        needed. */
    void createchildren_splitdir(int id, int dir);

public:
    /** This function creates two new nodes subdividing the node at its geometric center along a
        plane perpendicular to one of the coordinate axes, depending on the node's level in the tree.
        The splitting direction is selected as the modulo of the node's level, with (0=x, 1=y, 2=z),
        so that the nodes are alternatively divided along each of the axes when descending the tree.
        Finally the function invokes createchildren_splitdir() to actually create the child nodes. */
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

    //============= Data members =============

protected:
    int _dir;   // splitting direction is perpendicular to x-axis (0), y-axis(1) or z-axis (2)
};

//////////////////////////////////////////////////////////////////////

#endif // BINTREENODE_HPP
