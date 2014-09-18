/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef TREENODE_HPP
#define TREENODE_HPP

#include <vector>
#include "Box.hpp"
class TreeNodeDensityCalculator;

//////////////////////////////////////////////////////////////////////

/** TreeNode is an abstract class that represents nodes in a TreeDustGridStructure. It holds a node
    identifier, the corners of the node in the three spatial directions, and links to the father,
    children and neighbors of the node. */
class TreeNode : public Box
{
    //============= Constructing and destructing =============

protected:
    /** This constructor creates a new tree node with the specified father node, identifier, and
        spatial extent (defined by the coordinates of the corner points). The constructor sets the
        level of the new node to be one higher than the level of the father. If the pointer to the
        father is null, the level of the new cell is zero. */
    TreeNode(TreeNode* father, int id, const Box& extent);

public:
    /** Trivial virtual destructor. */
    virtual ~TreeNode();

    //============= Functions to be implemented in subclass =============

protected:
    /** This function creates a fresh new node of the same type as the receiving node. The arguments
      are the same as those for the constructor. Ownership for the new node is passed to the caller. */
    virtual TreeNode* createnode(TreeNode* father, int id, const Box& extent) = 0;

public:
    /** This function creates new nodes partitioning the node, and adds these new nodes as its own
        child nodes. Subdivision happens according to some default "regular" scheme, since no
        density information is passed to this function. The children are assigned consecutive
        integer identifiers, starting with the identifier specified as an argument to this
        function. A node does NOT take ownership of its children, so the caller is responsible for
        deleting the child nodes when they are no longer needed. */
    virtual void createchildren(int id) = 0;

    /** This function creates new nodes partitioning the node, and adds these new nodes as its own
        child nodes. Subdivision happens according to a possibly complex scheme implemented by
        the particular TreeNode subclass, based on the density information that can be obtained
        from the calculator passed as an argument to this function. The children are assigned
        consecutive integer identifiers, starting with the identifier specified as an argument to
        this function. A node does NOT take ownership of its children, so the caller is responsible
        for deleting the child nodes when they are no longer needed. */
    virtual void createchildren(int id, const TreeNodeDensityCalculator* calc) = 0;

    /** This function adds the relevant neighbors to a node with children (the function does nothing
        if the node doesn't have any children). It considers internal neighbors among the children
        as well as the neighbors of the father node (i.e. this node). The inherited neighbors must be
        distributed among the children depending on the geometry; note that a particular neighbor
        may be inherited by multiple children. */
    virtual void addneighbors() = 0;

    /** This function returns a pointer to the node's child that contains the specified point,
        assuming that the point is inside the node. This function crashes if the node is childless. */
    virtual TreeNode* child(Vec r) const = 0;

    //============= Functions implemented here =============

public:
    /** This function returns a pointer to the father of the node. */
    TreeNode* father() const;

    /** This function returns the ID number of the node. */
    int id() const;

    /** This function returns the level of the node. */
    int level() const;

    /** This function returns true if the node has no children, or false if it has children. */
    bool ynchildless() const;

    /** This function returns a list of pointers to the node's children. The list is either empty or
        of size 8. */
    const std::vector<TreeNode*>& children() const;

    /** This function returns a pointer to the node's child number \f$l\f$. It crashes if the node
        is childless or if \f$l\f$ is larger than or equal to the number of children. */
    TreeNode* child(int l) const;

    /** This function returns a pointer to the node in the child hierarchy of this node that
        contains the specified position, or null if the position is outside the node. It uses the
        child() function resursively to locate the appropriate node. */
    const TreeNode* whichnode(Vec r) const;

    /** This enum contains a constant for each of the walls in a node. The x-coordinate increases
        from BACK to FRONT, the y-coordinate increases from LEFT to RIGHT, and the z-coordinate
        increases from BOTTOM to TOP. */
    enum Wall { BACK=0, FRONT, LEFT, RIGHT, BOTTOM, TOP };

    /** This function returns a pointer to the node just beyond a given wall that contains the
        specified position, or null if such a node can't be found by searching the neighbors of
        that wall. The function expects that the neighbors of the node have been added. */
    const TreeNode* whichnode(Wall wall, Vec r) const;

    /** This function ensures that the node has 6 neighbor lists; it should be called before
        adding any neighbors to the node. */
    void ensureneighborlists();

    /** This function sorts the neighbor lists for each wall of this node so that neighbors with a
        larger overlap area are listed first. The function should be called only after neighbors
        have been added for all nodes in the tree. */
    void sortneighbors();

    /** This function adds a node to the list of neighbors corresponding to a given wall. */
    void addneighbor(Wall wall, TreeNode* node);

    /** This function deletes a node from the list of neighbors corresponding to a given wall. */
    void deleteneighbor(Wall wall, TreeNode* node);

    /** This static function makes the two specified nodes neighbors by adding node2 as a neighbor
        to node1 at wall1, and adding node1 as a neighbor to node2 at the complementing wall
        (back/front, left/right, bottom/top). */
    static void makeneighbors(Wall wall1, TreeNode* node1, TreeNode* node2);

    //============= Data members =============

protected:
    int _id;
    int _level;
    TreeNode* _father;
    std::vector<TreeNode*> _children;
    std::vector< std::vector<TreeNode*> > _neighbors;
};

//////////////////////////////////////////////////////////////////////

#endif // TREENODE_HPP
