/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "FatalError.hpp"
#include "PeerToPeerCommunicator.hpp"
#include "StaggeredAssigner.hpp"

////////////////////////////////////////////////////////////////////

StaggeredAssigner::StaggeredAssigner()
    : _valuesInBlock(0)
{
}

////////////////////////////////////////////////////////////////////

StaggeredAssigner::StaggeredAssigner(SimulationItem *parent)
    : _valuesInBlock(0)
{
    setParent(parent);
    setup();
}

////////////////////////////////////////////////////////////////////

void StaggeredAssigner::setupSelfBefore()
{
    ProcessAssigner::setupSelfBefore();

    if (!_comm) throw FATALERROR("Could not find an object of type PeerToPeerCommunicator in the simulation hierarchy");
}

////////////////////////////////////////////////////////////////////

StaggeredAssigner* StaggeredAssigner::clone()
{
    StaggeredAssigner* cl = new StaggeredAssigner(this);
    cl->copyFrom(this);
    return cl;
}

////////////////////////////////////////////////////////////////////

void StaggeredAssigner::copyFrom(const StaggeredAssigner *from)
{
    ProcessAssigner::copyFrom(from);
    _valuesInBlock = from->_valuesInBlock;
}

////////////////////////////////////////////////////////////////////
void StaggeredAssigner::assign(size_t size, size_t blocks)
{
    _blocksize = size;
    _nblocks = blocks;
    _valuesInBlock = 0;

    for (size_t i = 0; i < size; i++)
    {
        int rank = i % _comm->size();
        if (rank == _comm->rank()) _valuesInBlock++;
    }

    // Calculate the total number of assigned values (the pattern is repeated for each block)
    setBlocks(blocks);
}

////////////////////////////////////////////////////////////////////

void StaggeredAssigner::setBlocks(size_t blocks)
{
    _nvalues = _valuesInBlock * blocks;
    _nblocks = blocks;
}

////////////////////////////////////////////////////////////////////

size_t StaggeredAssigner::absoluteIndex(size_t relativeIndex)
{
    // Calculate the index of the block corresponding to this relativeIndex and the corresponding
    // relativeIndex in the first block
    size_t block = relativeIndex / _valuesInBlock;
    relativeIndex = relativeIndex - block*_valuesInBlock;

    // Return the absolute index
    return block*_blocksize + _comm->rank() + relativeIndex*_comm->size();
}

////////////////////////////////////////////////////////////////////

size_t StaggeredAssigner::relativeIndex(size_t absoluteIndex)
{
    // Calculate the index of the block corresponding to this absoluteIndex and the corresponding
    // absoluteIndex in the first block
    size_t block = absoluteIndex / _blocksize;
    absoluteIndex = absoluteIndex - block*_blocksize;

    // Return the relative index
    return ((absoluteIndex - _comm->rank()) / _comm->size()) + block*_valuesInBlock;
}

////////////////////////////////////////////////////////////////////

int StaggeredAssigner::rankForIndex(size_t index) const
{
    // Calculate the index of the block corresponding to this (absolute) index and the corresponding
    // (absolute) index in the first block
    size_t block = index / _blocksize;
    index = index - block*_blocksize;

    // Return the rank of the corresponding process
    return (index % _comm->size());
}

////////////////////////////////////////////////////////////////////

bool StaggeredAssigner::parallel() const
{
    return true;
}

////////////////////////////////////////////////////////////////////
