/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "FatalError.hpp"
#include "PeerToPeerCommunicator.hpp"
#include "SequentialAssigner.hpp"

////////////////////////////////////////////////////////////////////

SequentialAssigner::SequentialAssigner()
    : _start(0), _quotient(0), _remainder(0), _blocksize(0), _valuesInBlock(0)
{
}

////////////////////////////////////////////////////////////////////

SequentialAssigner::SequentialAssigner(SimulationItem *parent)
    : _start(0), _quotient(0), _remainder(0), _blocksize(0), _valuesInBlock(0)
{
    setParent(parent);
    setup();
}

////////////////////////////////////////////////////////////////////

void SequentialAssigner::setupSelfBefore()
{
    ProcessAssigner::setupSelfBefore();

    if (!_comm) throw FATALERROR("Could not find an object of type PeerToPeerCommunicator in the simulation hierarchy");
}

////////////////////////////////////////////////////////////////////

void SequentialAssigner::assign(size_t size, size_t blocks)
{
    int nprocs = _comm->size();   // The number of processes
    int rank = _comm->rank();     // The rank of this process

    _quotient = size / nprocs;
    _remainder = size % nprocs;
    _blocksize = size;

    // Calculate the number of values assigned to this process (in one block and in total)
    _valuesInBlock = ((size_t)rank < _remainder) ? _quotient + 1 : _quotient;
    _nvalues = _valuesInBlock * blocks;

    // Determine the index of the first value assigned to this process
    if ((size_t)rank < _remainder)
    {
        _start = rank * (_quotient + 1);
    }
    else
    {
        _start = _remainder * (_quotient + 1) + (rank - _remainder) * _quotient;
    }
}

////////////////////////////////////////////////////////////////////

size_t SequentialAssigner::absoluteIndex(size_t relativeIndex)
{
    // Calculate the index of the block corresponding to this relativeIndex and its position
    // within the corresponding block
    size_t block = relativeIndex / _valuesInBlock;
    size_t blockIndex = relativeIndex - block*_valuesInBlock + _start;

    // Return the absolute index
    return (block*_blocksize + blockIndex);
}

////////////////////////////////////////////////////////////////////

size_t SequentialAssigner::relativeIndex(size_t absoluteIndex)
{
    // Calculate the index of the block corresponding to this absoluteIndex and its position
    // within the corresponding block
    size_t block = absoluteIndex / _blocksize;
    size_t blockIndex = absoluteIndex % _blocksize;

    // Return the relative index
    return (block*_valuesInBlock + blockIndex - _start);
}

////////////////////////////////////////////////////////////////////

int SequentialAssigner::rankForIndex(size_t index) const
{
    // Calculate the position within the block of this (absolute) index
    size_t blockIndex = index % _blocksize;

    int rank;
    if (blockIndex < _remainder * (_quotient + 1) )
    {
        rank = blockIndex / (_quotient + 1);
    }
    else
    {
        size_t index2 = blockIndex - _remainder*(_quotient + 1);
        rank = _remainder + index2 / _quotient;
    }

    return rank;
}

////////////////////////////////////////////////////////////////////

bool SequentialAssigner::parallel() const
{
    return true;
}

////////////////////////////////////////////////////////////////////
