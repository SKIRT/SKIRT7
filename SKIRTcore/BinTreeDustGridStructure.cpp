/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include "BaryBinTreeNode.hpp"
#include "BinTreeDustGridStructure.hpp"
#include "FatalError.hpp"

//////////////////////////////////////////////////////////////////////

BinTreeDustGridStructure::BinTreeDustGridStructure()
    : _directionMethod(Alternating)
{
}

//////////////////////////////////////////////////////////////////////

void BinTreeDustGridStructure::setupSelfBefore()
{
    TreeDustGridStructure::setupSelfBefore();

    if (searchMethod() == TreeDustGridStructure::Bookkeeping)
        throw FATALERROR("Bookkeeping method is not compatible with binary tree");
}

//////////////////////////////////////////////////////////////////////

void BinTreeDustGridStructure::setDirectionMethod(BinTreeDustGridStructure::DirectionMethod value)
{
    _directionMethod = value;
}

//////////////////////////////////////////////////////////////////////

BinTreeDustGridStructure::DirectionMethod BinTreeDustGridStructure::directionMethod() const
{
    return _directionMethod;
}

//////////////////////////////////////////////////////////////////////

TreeNode* BinTreeDustGridStructure::createRoot(const Box& extent)
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
