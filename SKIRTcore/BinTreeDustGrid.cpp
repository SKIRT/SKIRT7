/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "BaryBinTreeNode.hpp"
#include "BinTreeDustGrid.hpp"
#include "FatalError.hpp"

//////////////////////////////////////////////////////////////////////

BinTreeDustGrid::BinTreeDustGrid()
    : _directionMethod(Alternating)
{
}

//////////////////////////////////////////////////////////////////////

void BinTreeDustGrid::setupSelfBefore()
{
    TreeDustGrid::setupSelfBefore();

    if (searchMethod() == TreeDustGrid::Bookkeeping)
        throw FATALERROR("Bookkeeping method is not compatible with binary tree");
}

//////////////////////////////////////////////////////////////////////

void BinTreeDustGrid::setDirectionMethod(BinTreeDustGrid::DirectionMethod value)
{
    _directionMethod = value;
}

//////////////////////////////////////////////////////////////////////

BinTreeDustGrid::DirectionMethod BinTreeDustGrid::directionMethod() const
{
    return _directionMethod;
}

//////////////////////////////////////////////////////////////////////

TreeNode* BinTreeDustGrid::createRoot(const Box& extent)
{
    switch (_directionMethod)
    {
    case Barycenter:
        _useDmibForSubdivide = false;  // turn off the use of the DustMassInBox interface since we need the barycenter
        return new BaryBinTreeNode(0, 0, extent);
    default:
        return new BinTreeNode(0, 0, extent);
    }
}

//////////////////////////////////////////////////////////////////////
