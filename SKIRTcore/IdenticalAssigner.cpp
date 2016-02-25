/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "FatalError.hpp"
#include "IdenticalAssigner.hpp"
#include "SequentialAssigner.hpp"

////////////////////////////////////////////////////////////////////

IdenticalAssigner::IdenticalAssigner()
    : _blockassigner(0), _nblocks(1)
{
}

////////////////////////////////////////////////////////////////////

IdenticalAssigner::IdenticalAssigner(SimulationItem *parent)
    : _blockassigner(0), _nblocks(1)
{
    setParent(parent);
    setup();
}

////////////////////////////////////////////////////////////////////

void IdenticalAssigner::setupSelfBefore()
{
    ProcessAssigner::setupSelfBefore();

    if (!_comm) throw FATALERROR("Could not find an object of type PeerToPeerCommunicator in the simulation hierarchy");
}

////////////////////////////////////////////////////////////////////

IdenticalAssigner* IdenticalAssigner::clone()
{
    IdenticalAssigner* cl = new IdenticalAssigner(this);
    cl->copyFrom(this);
    return cl;
}

////////////////////////////////////////////////////////////////////

void IdenticalAssigner::copyFrom(const IdenticalAssigner* from)
{
    ProcessAssigner::copyFrom(from);
    _start = from->_start;
    _nblocks = from->_nblocks;
    // use the clone function of SequentialAssigner
    if (!from->_blockassigner) _blockassigner = 0;
    else _blockassigner = from->_blockassigner->clone();
}

////////////////////////////////////////////////////////////////////

void IdenticalAssigner::assign(size_t size, size_t blocks)
{
    _blocksize = size;
    _nblocks = blocks;
    _start = 0;
    setBlocks(blocks);
}

////////////////////////////////////////////////////////////////////

void IdenticalAssigner::setBlocks(size_t blocks)
{
    if (blocks > 1)
    {
        // Create a SequentialAssigner to assign the blocks
        if (!_blockassigner) _blockassigner = new SequentialAssigner(this);
        _blockassigner->assign(blocks);

        // Set the number of values for this process and the starting index
        _nvalues = _blockassigner->nvalues() * _blocksize;
        _start = _blockassigner->absoluteIndex(0) * _blocksize;
    }
    else
    {
        delete _blockassigner;
        _blockassigner = 0;
        _nvalues = _blocksize*blocks;
    }
    _nblocks = blocks;
}

////////////////////////////////////////////////////////////////////

size_t IdenticalAssigner::absoluteIndex(size_t relativeIndex) const
{
    return (_start + relativeIndex);
}

////////////////////////////////////////////////////////////////////

size_t IdenticalAssigner::relativeIndex(size_t absoluteIndex) const
{
    return (absoluteIndex - _start);
}

////////////////////////////////////////////////////////////////////

int IdenticalAssigner::rankForIndex(size_t index) const
{
    if (!_blockassigner) throw FATALERROR("This function should never be called when blocks=1");

    // Calculate the block to which the index corresponds and subsequently get the assigned process from
    // the _blockassigner object
    size_t block = index / (_nvalues/_blockassigner->nvalues());
    return _blockassigner->rankForIndex(block);
}

////////////////////////////////////////////////////////////////////

bool IdenticalAssigner::parallel() const
{
    return (!_comm->isMultiProc() || _blockassigner!=0);
}

////////////////////////////////////////////////////////////////////

bool IdenticalAssigner::validIndex(size_t absoluteIndex) const
{
    if (_nblocks < 1) return false;
    else if (_nblocks == 1) return true;
    else return _comm->rank() == rankForIndex(absoluteIndex);
}

////////////////////////////////////////////////////////////////////

size_t IdenticalAssigner::nvaluesForRank(int rank) const
{
    if (_nblocks > 1)
        return _blockassigner->nvaluesForRank(rank) * _blocksize;
    else
        return _nvalues;
}

////////////////////////////////////////////////////////////////////
