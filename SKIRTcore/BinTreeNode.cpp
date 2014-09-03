/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "BinTreeNode.hpp"
#include "FatalError.hpp"

using namespace std;

//////////////////////////////////////////////////////////////////////

// macros for easily accessing a particular child
#define CHILD_0 _children[0]
#define CHILD_1 _children[1]

//////////////////////////////////////////////////////////////////////

// constants for splitting direction (not an enum because we use modulo arithmetic)
#define XDIR 0
#define YDIR 1
#define ZDIR 2

//////////////////////////////////////////////////////////////////////

BinTreeNode::BinTreeNode(TreeNode* father, int id, const Box& extent)
    : TreeNode(father, id, extent)
{
}

//////////////////////////////////////////////////////////////////////

TreeNode* BinTreeNode::createnode(TreeNode* father, int id, const Box& extent)
{
    return new BinTreeNode(father, id, extent);
}

//////////////////////////////////////////////////////////////////////

void BinTreeNode::createchildren_splitdir(int id, int dir)
{
    _children.resize(2);
    _dir = dir;
    switch (dir)
    {
    case XDIR:
        {
            double xc = 0.5*(_xmin+_xmax);
            CHILD_0 = createnode(this, id++, Box(_xmin, _ymin, _zmin,    xc, _ymax, _zmax));
            CHILD_1 = createnode(this, id++, Box(   xc, _ymin, _zmin, _xmax, _ymax, _zmax));
        }
        break;
    case YDIR:
        {
            double yc = 0.5*(_ymin+_ymax);
            CHILD_0 = createnode(this, id++, Box(_xmin, _ymin, _zmin, _xmax,    yc, _zmax));
            CHILD_1 = createnode(this, id++, Box(_xmin,    yc, _zmin, _xmax, _ymax, _zmax));
        }
        break;
    case ZDIR:
        {
            double zc = 0.5*(_zmin+_zmax);
            CHILD_0 = createnode(this, id++, Box(_xmin, _ymin, _zmin, _xmax, _ymax,    zc));
            CHILD_1 = createnode(this, id++, Box(_xmin, _ymin,    zc, _xmax, _ymax, _zmax));
        }
        break;
    default:
        throw FATALERROR("Incorrect value for subdivision direction");
    }
}

//////////////////////////////////////////////////////////////////////

void BinTreeNode::createchildren(int id)
{
    createchildren_splitdir(id, _level % 3);
}

//////////////////////////////////////////////////////////////////////

void BinTreeNode::createchildren(int id, const TreeNodeDensityCalculator* /*calc*/)
{
    createchildren(id);
}

//////////////////////////////////////////////////////////////////////

void BinTreeNode::addneighbors()
{
    // if we don't have any children, we can't add neighbors
    if (_children.empty()) return;

    // ensure that all involved nodes have a neighbor list for each of the walls
    ensureneighborlists();
    CHILD_0->ensureneighborlists();
    CHILD_1->ensureneighborlists();

    switch (_dir)
    {
    case XDIR:
        {
            double xc = CHILD_0->xmax();

            // Internal neighbors
            makeneighbors(FRONT, CHILD_0, CHILD_1);

            // The BACK neighbors of this node
            {
                const vector<TreeNode*>& neighbors = _neighbors[BACK];
                for (unsigned int i=0; i<neighbors.size(); i++)
                {
                    TreeNode* neighbor = neighbors[i];
                    neighbor->deleteneighbor(FRONT, this);
                    makeneighbors(FRONT, neighbor, CHILD_0);
                }
            }
            // The FRONT neighbors of this node
            {
                const vector<TreeNode*>& neighbors = _neighbors[FRONT];
                for (unsigned int i=0; i<neighbors.size(); i++)
                {
                    TreeNode* neighbor = neighbors[i];
                    neighbor->deleteneighbor(BACK, this);
                    makeneighbors(BACK, neighbor, CHILD_1);
                }
            }
            // The LEFT neighbors of this node
            {
                const vector<TreeNode*>& neighbors = _neighbors[LEFT];
                for (unsigned int i=0; i<neighbors.size(); i++)
                {
                    TreeNode* neighbor = neighbors[i];
                    neighbor->deleteneighbor(RIGHT, this);
                    if (neighbor->xmin() <= xc) makeneighbors(RIGHT, neighbor, CHILD_0);
                    if (neighbor->xmax() >= xc) makeneighbors(RIGHT, neighbor, CHILD_1);
                }
            }
            // The RIGHT neighbors of this node
            {
                const vector<TreeNode*>& neighbors = _neighbors[RIGHT];
                for (unsigned int i=0; i<neighbors.size(); i++)
                {
                    TreeNode* neighbor = neighbors[i];
                    neighbor->deleteneighbor(LEFT, this);
                    if (neighbor->xmin() <= xc) makeneighbors(LEFT, neighbor, CHILD_0);
                    if (neighbor->xmax() >= xc) makeneighbors(LEFT, neighbor, CHILD_1);
                }
            }
            // The BOTTOM neighbors of this node
            {
                const vector<TreeNode*>& neighbors = _neighbors[BOTTOM];
                for (unsigned int i=0; i<neighbors.size(); i++)
                {
                    TreeNode* neighbor = neighbors[i];
                    neighbor->deleteneighbor(TOP, this);
                    if (neighbor->xmin() <= xc) makeneighbors(TOP, neighbor, CHILD_0);
                    if (neighbor->xmax() >= xc) makeneighbors(TOP, neighbor, CHILD_1);
                }
            }
            // The TOP neighbors of this node
            {
                const vector<TreeNode*>& neighbors = _neighbors[TOP];
                for (unsigned int i=0; i<neighbors.size(); i++)
                {
                    TreeNode* neighbor = neighbors[i];
                    neighbor->deleteneighbor(BOTTOM, this);
                    if (neighbor->xmin() <= xc) makeneighbors(BOTTOM, neighbor, CHILD_0);
                    if (neighbor->xmax() >= xc) makeneighbors(BOTTOM, neighbor, CHILD_1);
                }
            }
        }
        break;
    case YDIR:
        {
            double yc = CHILD_0->ymax();

            // Internal neighbors
            makeneighbors(RIGHT, CHILD_0, CHILD_1);

            // The BACK neighbors of this node
            {
                const vector<TreeNode*>& neighbors = _neighbors[BACK];
                for (unsigned int i=0; i<neighbors.size(); i++)
                {
                    TreeNode* neighbor = neighbors[i];
                    neighbor->deleteneighbor(FRONT, this);
                    if (neighbor->ymin() <= yc) makeneighbors(FRONT, neighbor, CHILD_0);
                    if (neighbor->ymax() >= yc) makeneighbors(FRONT, neighbor, CHILD_1);
                }
            }
            // The FRONT neighbors of this node
            {
                const vector<TreeNode*>& neighbors = _neighbors[FRONT];
                for (unsigned int i=0; i<neighbors.size(); i++)
                {
                    TreeNode* neighbor = neighbors[i];
                    neighbor->deleteneighbor(BACK, this);
                    if (neighbor->ymin() <= yc) makeneighbors(BACK, neighbor, CHILD_0);
                    if (neighbor->ymax() >= yc) makeneighbors(BACK, neighbor, CHILD_1);
                }
            }
            // The LEFT neighbors of this node
            {
                const vector<TreeNode*>& neighbors = _neighbors[LEFT];
                for (unsigned int i=0; i<neighbors.size(); i++)
                {
                    TreeNode* neighbor = neighbors[i];
                    neighbor->deleteneighbor(RIGHT, this);
                    makeneighbors(RIGHT, neighbor, CHILD_0);
                }
            }
            // The RIGHT neighbors of this node
            {
                const vector<TreeNode*>& neighbors = _neighbors[RIGHT];
                for (unsigned int i=0; i<neighbors.size(); i++)
                {
                    TreeNode* neighbor = neighbors[i];
                    neighbor->deleteneighbor(LEFT, this);
                    makeneighbors(LEFT, neighbor, CHILD_1);
                }
            }
            // The BOTTOM neighbors of this node
            {
                const vector<TreeNode*>& neighbors = _neighbors[BOTTOM];
                for (unsigned int i=0; i<neighbors.size(); i++)
                {
                    TreeNode* neighbor = neighbors[i];
                    neighbor->deleteneighbor(TOP, this);
                    if (neighbor->ymin() <= yc) makeneighbors(TOP, neighbor, CHILD_0);
                    if (neighbor->ymax() >= yc) makeneighbors(TOP, neighbor, CHILD_1);
                }
            }
            // The TOP neighbors of this node
            {
                const vector<TreeNode*>& neighbors = _neighbors[TOP];
                for (unsigned int i=0; i<neighbors.size(); i++)
                {
                    TreeNode* neighbor = neighbors[i];
                    neighbor->deleteneighbor(BOTTOM, this);
                    if (neighbor->ymin() <= yc) makeneighbors(BOTTOM, neighbor, CHILD_0);
                    if (neighbor->ymax() >= yc) makeneighbors(BOTTOM, neighbor, CHILD_1);
                }
            }
        }
        break;
    case ZDIR:
        {
            double zc = CHILD_0->zmax();

            // Internal neighbors
            makeneighbors(TOP, CHILD_0, CHILD_1);

            // The BACK neighbors of this node
            {
                const vector<TreeNode*>& neighbors = _neighbors[BACK];
                for (unsigned int i=0; i<neighbors.size(); i++)
                {
                    TreeNode* neighbor = neighbors[i];
                    neighbor->deleteneighbor(FRONT, this);
                    if (neighbor->zmin() <= zc) makeneighbors(FRONT, neighbor, CHILD_0);
                    if (neighbor->zmax() >= zc) makeneighbors(FRONT, neighbor, CHILD_1);
                }
            }
            // The FRONT neighbors of this node
            {
                const vector<TreeNode*>& neighbors = _neighbors[FRONT];
                for (unsigned int i=0; i<neighbors.size(); i++)
                {
                    TreeNode* neighbor = neighbors[i];
                    neighbor->deleteneighbor(BACK, this);
                    if (neighbor->zmin() <= zc) makeneighbors(BACK, neighbor, CHILD_0);
                    if (neighbor->zmax() >= zc) makeneighbors(BACK, neighbor, CHILD_1);
                }
            }
            // The LEFT neighbors of this node
            {
                const vector<TreeNode*>& neighbors = _neighbors[LEFT];
                for (unsigned int i=0; i<neighbors.size(); i++)
                {
                    TreeNode* neighbor = neighbors[i];
                    neighbor->deleteneighbor(RIGHT, this);
                    if (neighbor->zmin() <= zc) makeneighbors(RIGHT, neighbor, CHILD_0);
                    if (neighbor->zmax() >= zc) makeneighbors(RIGHT, neighbor, CHILD_1);
                }
            }
            // The RIGHT neighbors of this node
            {
                const vector<TreeNode*>& neighbors = _neighbors[RIGHT];
                for (unsigned int i=0; i<neighbors.size(); i++)
                {
                    TreeNode* neighbor = neighbors[i];
                    neighbor->deleteneighbor(LEFT, this);
                    if (neighbor->zmin() <= zc) makeneighbors(LEFT, neighbor, CHILD_0);
                    if (neighbor->zmax() >= zc) makeneighbors(LEFT, neighbor, CHILD_1);
                }
            }
            // The BOTTOM neighbors of this node
            {
                const vector<TreeNode*>& neighbors = _neighbors[BOTTOM];
                for (unsigned int i=0; i<neighbors.size(); i++)
                {
                    TreeNode* neighbor = neighbors[i];
                    neighbor->deleteneighbor(TOP, this);
                    makeneighbors(TOP, neighbor, CHILD_0);
                }
            }
            // The TOP neighbors of this node
            {
                const vector<TreeNode*>& neighbors = _neighbors[TOP];
                for (unsigned int i=0; i<neighbors.size(); i++)
                {
                    TreeNode* neighbor = neighbors[i];
                    neighbor->deleteneighbor(BOTTOM, this);
                    makeneighbors(BOTTOM, neighbor, CHILD_1);
                }
            }
        }
        break;
    default:
        throw FATALERROR("Incorrect value for subdivision direction");
    }
}

//////////////////////////////////////////////////////////////////////

TreeNode* BinTreeNode::child(Vec r) const
{
    switch (_dir)
    {
    case XDIR:  return  r.x() < CHILD_0->xmax()  ?  CHILD_0 : CHILD_1;
    case YDIR:  return  r.y() < CHILD_0->ymax()  ?  CHILD_0 : CHILD_1;
    case ZDIR:  return  r.z() < CHILD_0->zmax()  ?  CHILD_0 : CHILD_1;
    }
    throw FATALERROR("Incorrect value for subdivision direction");
}

//////////////////////////////////////////////////////////////////////
