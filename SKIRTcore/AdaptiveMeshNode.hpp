/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef ADAPTIVEMESHNODE_HPP
#define ADAPTIVEMESHNODE_HPP

#include <vector>
#include "Box.hpp"
class AdaptiveMeshFile;

////////////////////////////////////////////////////////////////////

/** The AdaptiveMeshNode class is a helper class used by the AdaptiveMesh class to represent
    individual nodes in a tree data structure. An AdaptiveMeshNode instance can represent
    a leaf or a nonleaf node. A nonleaf node maintains a list of pointers to its children.
    A leaf node instead keeps a list of pointers to the most likely neighbor for each of the
    six cell walls. For more information, refer to the AdaptiveMesh class. */
class AdaptiveMeshNode : public Box
{
public:

    //================= Construction - Destruction =================

    /** The constructor receives the node's extent from its arguments, and reads the other node
        data from the following record in the specified file. In addition to constructing the new
        node, it also adds leaf node pointers and field values to the respective vectors held by
        the mesh. The list of field indices should not have duplicate or negative values. */
    AdaptiveMeshNode(const Box& extent, QList<int> fieldIndices, AdaptiveMeshFile* meshfile,
                     std::vector<AdaptiveMeshNode*>& leafnodes, std::vector< std::vector<double> >& fieldvalues);

    /** This function adds neighbor information to the receiving leaf node. Specifically, it
        constructs a list of the node's most likely neighbor at each of its six walls. The function
        does nothing if neighbor information has already been added, or if the node is a nonleaf
        node. The first argument specifies the adaptive mesh root node, which is queried by this
        function to locate the node's neighbors. The second argument specifies a very small offset
        (relative to the domain size) used to determine a location just beyond a node wall. */
    void addNeighbors(AdaptiveMeshNode* root, double eps);

    /** The destructor releases the node's children if it is a nonleaf node. */
    ~AdaptiveMeshNode();

    //======================= Other functions ======================

    /** For leaf nodes, this function returns the Morton order cell index of the corresponding
        cell. For nonleaf nodes, this function returns -1. */
    int cellIndex() const;

    /** This function returns true if the node is a leaf node, false if it is a nonleaf node. */
    bool isLeaf() const;

    /** This function returns a pointer to the node's immediate child that contains the specified
        point, assuming that the point is inside the node (which is not verified). This function
        crashes if the node is a leaf node. */
    const AdaptiveMeshNode* child(Vec r) const;

    /** This function returns a pointer to the deepest node in the child hierarchy of this node
        that contains the specified point, or null if the point is outside the node. It uses the
        child() function repeatedly to locate the appropriate node. */
    const AdaptiveMeshNode* whichnode(Vec r) const;

    /** This enum contains a constant for each of the walls in a node. The x-coordinate increases
        from BACK to FRONT, the y-coordinate increases from LEFT to RIGHT, and the z-coordinate
        increases from BOTTOM to TOP. */
    enum Wall { BACK=0, FRONT, LEFT, RIGHT, BOTTOM, TOP };

    /** This function returns a pointer to the node just beyond a given wall that contains the
        specified position, or null if the specified position is not inside the most likely
        neighbor for that wall, or if neighbors have not been added for the node, or if this
        is not a leaf node. */
    const AdaptiveMeshNode* whichnode(Wall wall, Vec r) const;

    //========================= Data members =======================

private:
    int _Nx, _Ny, _Nz;   // number of grid cells in each direction; zero for leaf nodes
    int _m;              // Morton order index for the cell represented by this leaf node; -1 for nonleaf nodes
    std::vector<const AdaptiveMeshNode*> _nodes;  // pointers to children (nonleaf nodes) or neighbors (leaf nodes)
};

////////////////////////////////////////////////////////////////////

#endif // ADAPTIVEMESHNODE_HPP
