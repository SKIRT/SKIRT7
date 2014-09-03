/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "BaryOctTreeNode.hpp"
#include "TreeNodeDensityCalculator.hpp"

using namespace std;

//////////////////////////////////////////////////////////////////////

BaryOctTreeNode::BaryOctTreeNode(TreeNode* father, int id, const Box& extent)
    : OctTreeNode(father, id, extent)
{
}

//////////////////////////////////////////////////////////////////////

TreeNode* BaryOctTreeNode::createnode(TreeNode* father, int id, const Box& extent)
{
    return new BaryOctTreeNode(father, id, extent);
}

//////////////////////////////////////////////////////////////////////

void BaryOctTreeNode::createchildren(int id, const TreeNodeDensityCalculator* calc)
{
    createchildren_splitpoint(id, calc->barycenter());
}

//////////////////////////////////////////////////////////////////////
