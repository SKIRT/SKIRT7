/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "BaryOctTreeNode.hpp"
#include "OctTreeDustGrid.hpp"

//////////////////////////////////////////////////////////////////////

OctTreeDustGrid::OctTreeDustGrid()
    : _barycentric(false)
{
}

//////////////////////////////////////////////////////////////////////

void OctTreeDustGrid::setBarycentric(bool value)
{
    _barycentric = value;
}

//////////////////////////////////////////////////////////////////////

bool OctTreeDustGrid::barycentric() const
{
    return _barycentric;
}

//////////////////////////////////////////////////////////////////////

TreeNode* OctTreeDustGrid::createRoot(const Box& extent)
{
    if (_barycentric)
    {
        _useDmibForSubdivide = false;  // turn off the use of the DustMassInBox interface since we need the barycenter
        return new BaryOctTreeNode(0, 0, extent);
    }
    else
    {
        return new OctTreeNode(0, 0, extent);
    }
}

//////////////////////////////////////////////////////////////////////
