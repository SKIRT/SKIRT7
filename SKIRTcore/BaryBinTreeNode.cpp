/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "BaryBinTreeNode.hpp"
#include "TreeNodeDensityCalculator.hpp"

using namespace std;

//////////////////////////////////////////////////////////////////////

// constants for splitting direction (not an enum because we use modulo arithmetic)
#define XDIR 0
#define YDIR 1
#define ZDIR 2

//////////////////////////////////////////////////////////////////////

BaryBinTreeNode::BaryBinTreeNode(TreeNode* father, int id, const Box& extent)
    : BinTreeNode(father, id, extent)
{
}

//////////////////////////////////////////////////////////////////////

TreeNode* BaryBinTreeNode::createnode(TreeNode* father, int id, const Box& extent)
{
    return new BaryBinTreeNode(father, id, extent);
}

//////////////////////////////////////////////////////////////////////

void BaryBinTreeNode::createchildren(int id, const TreeNodeDensityCalculator* calc)
{
    Vec b = calc->barycenter();

    // fractional distance to nearest wall, in each direction
    double dx = min(b.x()-_xmin, _xmax-b.x())/(_xmax-_xmin);
    double dy = min(b.y()-_ymin, _ymax-b.y())/(_ymax-_ymin);
    double dz = min(b.z()-_zmin, _zmax-b.z())/(_zmax-_zmin);

    // select the direction with the smallest relative distance
    int dir;
    if (dx < dy)
    {
        if (dx < dz) dir = XDIR;
        else dir = ZDIR;
    }
    else
    {
        if (dy < dz) dir = YDIR;
        else dir = ZDIR;
    }

    // split along that direction
    createchildren_splitdir(id, dir);
}

//////////////////////////////////////////////////////////////////////
