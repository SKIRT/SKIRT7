/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "OctTreeNode.hpp"

using namespace std;

//////////////////////////////////////////////////////////////////////

// macros for easily accessing a particular child
#define CHILD_0 _children[0]
#define CHILD_1 _children[1]
#define CHILD_2 _children[2]
#define CHILD_3 _children[3]
#define CHILD_4 _children[4]
#define CHILD_5 _children[5]
#define CHILD_6 _children[6]
#define CHILD_7 _children[7]

//////////////////////////////////////////////////////////////////////

OctTreeNode::OctTreeNode(TreeNode* father, int id, const Box& extent)
    : TreeNode(father, id, extent)
{
}

//////////////////////////////////////////////////////////////////////

TreeNode* OctTreeNode::createnode(TreeNode* father, int id, const Box& extent)
{
    return new OctTreeNode(father, id, extent);
}

//////////////////////////////////////////////////////////////////////

void OctTreeNode::createchildren_splitpoint(int id, Vec r)
{
    _children.resize(8);
    CHILD_0 = createnode(this, id++, Box(_xmin, _ymin, _zmin, r.x(), r.y(), r.z()));
    CHILD_1 = createnode(this, id++, Box(r.x(), _ymin, _zmin, _xmax, r.y(), r.z()));
    CHILD_2 = createnode(this, id++, Box(_xmin, r.y(), _zmin, r.x(), _ymax, r.z()));
    CHILD_3 = createnode(this, id++, Box(r.x(), r.y(), _zmin, _xmax, _ymax, r.z()));
    CHILD_4 = createnode(this, id++, Box(_xmin, _ymin, r.z(), r.x(), r.y(), _zmax));
    CHILD_5 = createnode(this, id++, Box(r.x(), _ymin, r.z(), _xmax, r.y(), _zmax));
    CHILD_6 = createnode(this, id++, Box(_xmin, r.y(), r.z(), r.x(), _ymax, _zmax));
    CHILD_7 = createnode(this, id++, Box(r.x(), r.y(), r.z(), _xmax, _ymax, _zmax));
}

//////////////////////////////////////////////////////////////////////

void OctTreeNode::createchildren(int id)
{
    createchildren_splitpoint(id, center());
}

//////////////////////////////////////////////////////////////////////

void OctTreeNode::createchildren(int id, const TreeNodeDensityCalculator* /*calc*/)
{
    createchildren(id);
}

//////////////////////////////////////////////////////////////////////

void OctTreeNode::addneighbors()
{
    // if we don't have any children, we can't add neighbors
    if (_children.empty()) return;

    // ensure that all involved nodes have a neighbor list for each of the walls
    ensureneighborlists();
    CHILD_0->ensureneighborlists();
    CHILD_1->ensureneighborlists();
    CHILD_2->ensureneighborlists();
    CHILD_3->ensureneighborlists();
    CHILD_4->ensureneighborlists();
    CHILD_5->ensureneighborlists();
    CHILD_6->ensureneighborlists();
    CHILD_7->ensureneighborlists();

    // Internal neighbors: each of the 8 new children has 3 neighbors among its siblings
    makeneighbors(FRONT, CHILD_0, CHILD_1);
    makeneighbors(RIGHT, CHILD_0, CHILD_2);
    makeneighbors(TOP  , CHILD_0, CHILD_4);
    makeneighbors(RIGHT, CHILD_1, CHILD_3);
    makeneighbors(TOP  , CHILD_1, CHILD_5);
    makeneighbors(FRONT, CHILD_2, CHILD_3);
    makeneighbors(TOP  , CHILD_2, CHILD_6);
    makeneighbors(TOP  , CHILD_3, CHILD_7);
    makeneighbors(FRONT, CHILD_4, CHILD_5);
    makeneighbors(RIGHT, CHILD_4, CHILD_6);
    makeneighbors(RIGHT, CHILD_5, CHILD_7);
    makeneighbors(FRONT, CHILD_6, CHILD_7);

    // The point where this node is split into its children
    double xc = CHILD_0->xmax();
    double yc = CHILD_0->ymax();
    double zc = CHILD_0->zmax();

    // The BACK neighbors of this node
    {
        const vector<TreeNode*>& neighbors = _neighbors[BACK];
        for (unsigned int i=0; i<neighbors.size(); i++)
        {
            TreeNode* neighbor = neighbors[i];
            neighbor->deleteneighbor(FRONT, this);
            if (neighbor->ymin() <= yc  &&  neighbor->zmin() <= zc) makeneighbors(FRONT, neighbor, CHILD_0);
            if (neighbor->ymax() >= yc  &&  neighbor->zmin() <= zc) makeneighbors(FRONT, neighbor, CHILD_2);
            if (neighbor->ymin() <= yc  &&  neighbor->zmax() >= zc) makeneighbors(FRONT, neighbor, CHILD_4);
            if (neighbor->ymax() >= yc  &&  neighbor->zmax() >= zc) makeneighbors(FRONT, neighbor, CHILD_6);
        }
    }
    // The FRONT neighbors of this node
    {
        const vector<TreeNode*>& neighbors = _neighbors[FRONT];
        for (unsigned int i=0; i<neighbors.size(); i++)
        {
            TreeNode* neighbor = neighbors[i];
            neighbor->deleteneighbor(BACK, this);
            if (neighbor->ymin() <= yc  &&  neighbor->zmin() <= zc) makeneighbors(BACK, neighbor, CHILD_1);
            if (neighbor->ymax() >= yc  &&  neighbor->zmin() <= zc) makeneighbors(BACK, neighbor, CHILD_3);
            if (neighbor->ymin() <= yc  &&  neighbor->zmax() >= zc) makeneighbors(BACK, neighbor, CHILD_5);
            if (neighbor->ymax() >= yc  &&  neighbor->zmax() >= zc) makeneighbors(BACK, neighbor, CHILD_7);
        }
    }
    // The LEFT neighbors of this node
    {
        const vector<TreeNode*>& neighbors = _neighbors[LEFT];
        for (unsigned int i=0; i<neighbors.size(); i++)
        {
            TreeNode* neighbor = neighbors[i];
            neighbor->deleteneighbor(RIGHT, this);
            if (neighbor->xmin() <= xc  &&  neighbor->zmin() <= zc) makeneighbors(RIGHT, neighbor, CHILD_0);
            if (neighbor->xmax() >= xc  &&  neighbor->zmin() <= zc) makeneighbors(RIGHT, neighbor, CHILD_1);
            if (neighbor->xmin() <= xc  &&  neighbor->zmax() >= zc) makeneighbors(RIGHT, neighbor, CHILD_4);
            if (neighbor->xmax() >= xc  &&  neighbor->zmax() >= zc) makeneighbors(RIGHT, neighbor, CHILD_5);
        }
    }
    // The RIGHT neighbors of this node
    {
        const vector<TreeNode*>& neighbors = _neighbors[RIGHT];
        for (unsigned int i=0; i<neighbors.size(); i++)
        {
            TreeNode* neighbor = neighbors[i];
            neighbor->deleteneighbor(LEFT, this);
            if (neighbor->xmin() <= xc  &&  neighbor->zmin() <= zc) makeneighbors(LEFT, neighbor, CHILD_2);
            if (neighbor->xmax() >= xc  &&  neighbor->zmin() <= zc) makeneighbors(LEFT, neighbor, CHILD_3);
            if (neighbor->xmin() <= xc  &&  neighbor->zmax() >= zc) makeneighbors(LEFT, neighbor, CHILD_6);
            if (neighbor->xmax() >= xc  &&  neighbor->zmax() >= zc) makeneighbors(LEFT, neighbor, CHILD_7);
        }
    }
    // The BOTTOM neighbors of this node
    {
        const vector<TreeNode*>& neighbors = _neighbors[BOTTOM];
        for (unsigned int i=0; i<neighbors.size(); i++)
        {
            TreeNode* neighbor = neighbors[i];
            neighbor->deleteneighbor(TOP, this);
            if (neighbor->xmin() <= xc  &&  neighbor->ymin() <= yc) makeneighbors(TOP, neighbor, CHILD_0);
            if (neighbor->xmax() >= xc  &&  neighbor->ymin() <= yc) makeneighbors(TOP, neighbor, CHILD_1);
            if (neighbor->xmin() <= xc  &&  neighbor->ymax() >= yc) makeneighbors(TOP, neighbor, CHILD_2);
            if (neighbor->xmax() >= xc  &&  neighbor->ymax() >= yc) makeneighbors(TOP, neighbor, CHILD_3);
        }
    }
    // The TOP neighbors of this node
    {
        const vector<TreeNode*>& neighbors = _neighbors[TOP];
        for (unsigned int i=0; i<neighbors.size(); i++)
        {
            TreeNode* neighbor = neighbors[i];
            neighbor->deleteneighbor(BOTTOM, this);
            if (neighbor->xmin() <= xc  &&  neighbor->ymin() <= yc) makeneighbors(BOTTOM, neighbor, CHILD_4);
            if (neighbor->xmax() >= xc  &&  neighbor->ymin() <= yc) makeneighbors(BOTTOM, neighbor, CHILD_5);
            if (neighbor->xmin() <= xc  &&  neighbor->ymax() >= yc) makeneighbors(BOTTOM, neighbor, CHILD_6);
            if (neighbor->xmax() >= xc  &&  neighbor->ymax() >= yc) makeneighbors(BOTTOM, neighbor, CHILD_7);
        }
    }
}

//////////////////////////////////////////////////////////////////////

TreeNode* OctTreeNode::child(Vec r) const
{
    Vec rc = CHILD_0->rmax();
    int l = (r.x()<rc.x() ? 0 : 1) + (r.y()<rc.y() ? 0 : 2) + (r.z()<rc.z() ? 0 : 4);
    return _children[l];
}

//////////////////////////////////////////////////////////////////////

