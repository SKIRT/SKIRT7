/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "FatalError.hpp"
#include "PeerToPeerCommunicator.hpp"
#include "RootAssigner.hpp"

////////////////////////////////////////////////////////////////////

RootAssigner::RootAssigner()
{
}

////////////////////////////////////////////////////////////////////

RootAssigner::RootAssigner(SimulationItem *parent)
{
    setParent(parent);
    setup();
}

////////////////////////////////////////////////////////////////////

RootAssigner* RootAssigner::clone()
{
    RootAssigner* cl = new RootAssigner(this);
    cl->copyFrom(this);
    return cl;
}

////////////////////////////////////////////////////////////////////

void RootAssigner::copyFrom(const RootAssigner *from)
{
    ProcessAssigner::copyFrom(from);
}

////////////////////////////////////////////////////////////////////

void RootAssigner::assign(size_t size, size_t blocks)
{
    _blocksize = size;
    _nblocks = blocks;
    if (_comm)
    {
        // Set the number of values assigned to this process
        _nvalues = _comm->isRoot() ? size : 0;
    }
    else
    {
        // Set the number of values assigned to this process
        _nvalues = size;
    }
    setBlocks(blocks);
}

////////////////////////////////////////////////////////////////////

void RootAssigner::setBlocks(size_t blocks)
{
    _nvalues = _blocksize*blocks;
}

////////////////////////////////////////////////////////////////////

size_t RootAssigner::absoluteIndex(size_t relativeIndex)
{
    if (_comm && !_comm->isRoot())
        throw FATALERROR("This function should not be called from any process other than the root");

    return relativeIndex;
}

////////////////////////////////////////////////////////////////////

size_t RootAssigner::relativeIndex(size_t absoluteIndex)
{
    if (_comm && !_comm->isRoot())
        throw FATALERROR("This function should not be called from any process other than the root");

    return absoluteIndex;
}

////////////////////////////////////////////////////////////////////

int RootAssigner::rankForIndex(size_t /*index*/) const
{
    if (!_comm)
        throw FATALERROR("This function should never be called");

    return _comm->root();
}

////////////////////////////////////////////////////////////////////

bool RootAssigner::parallel() const
{
    return true;
}

////////////////////////////////////////////////////////////////////
