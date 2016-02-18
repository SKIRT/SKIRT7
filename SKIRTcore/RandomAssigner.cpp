/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "FatalError.hpp"
#include "PeerToPeerCommunicator.hpp"
#include "Random.hpp"
#include "RandomAssigner.hpp"
#include "SequentialAssigner.hpp"

////////////////////////////////////////////////////////////////////

RandomAssigner::RandomAssigner()
    : _valuesInBlock(0)
{
}

////////////////////////////////////////////////////////////////////

RandomAssigner::RandomAssigner(SimulationItem *parent)
    : _valuesInBlock(0)
{
    setParent(parent);
    setup();
}

////////////////////////////////////////////////////////////////////

void RandomAssigner::setupSelfBefore()
{
    ProcessAssigner::setupSelfBefore();

    if (!_comm) throw FATALERROR("Could not find an object of type PeerToPeerCommunicator in the simulation hierarchy");

    _random = find<Random>();
}

////////////////////////////////////////////////////////////////////

RandomAssigner* RandomAssigner::clone()
{
    RandomAssigner* cl = new RandomAssigner(this);
    cl->copyFrom(this);
    return cl;
}

////////////////////////////////////////////////////////////////////

void RandomAssigner::copyFrom(const RandomAssigner *from)
{
    ProcessAssigner::copyFrom(from);
    _random = from->_random;
    _assignment = from->_assignment;
    _values = from->_values;
    _valuesInBlock = from->_valuesInBlock;
}

////////////////////////////////////////////////////////////////////

void RandomAssigner::assign(size_t size, size_t blocks)
{
    _blocksize = size;
    _nblocks = blocks;
    _assignment.clear();
    _assignment.resize(size);
    _values.clear();
    _values.reserve(1.2*size/_comm->size());

    // For each value in a certain subset of 'size', let this process determine a random process rank
    SequentialAssigner* helpassigner = new SequentialAssigner(this);
    helpassigner->assign(size);
    for (size_t i = 0; i < helpassigner->nvalues(); i++)
    {
        int rank = floor(_random->uniform() * _comm->size());
        _assignment[helpassigner->absoluteIndex(i)] = rank;
    }

    // Communication of the randomly determined ranks
    for (size_t j = 0; j < size; j++)
    {
        int sender = helpassigner->rankForIndex(j);
        _comm->broadcast(_assignment[j], sender);

        // If the process assigned to this value is this process, add the value to the list
        if (_assignment[j] == _comm->rank())
        {
            _values.push_back(j);
        }
    }

    // Set the number of values assigned to this process
    _valuesInBlock = _values.size();
    setBlocks(blocks);
}

////////////////////////////////////////////////////////////////////

void RandomAssigner::setBlocks(size_t blocks)
{
    _nvalues = _values.size()*blocks;
    _nblocks = blocks;
}

////////////////////////////////////////////////////////////////////

size_t RandomAssigner::absoluteIndex(size_t relativeIndex)
{
    // Calculate the index of the block corresponding to this relativeIndex and the corresponding
    // relativeIndex in the first block
    size_t block = relativeIndex / _valuesInBlock;
    relativeIndex = relativeIndex - block*_valuesInBlock;

    return _values[relativeIndex] + _blocksize*block;
}

////////////////////////////////////////////////////////////////////

size_t RandomAssigner::relativeIndex(size_t absoluteIndex)
{
    // Calculate the index of the block corresponding to this absoluteIndex and the corresponding
    // absoluteIndex in the first block
    size_t block = absoluteIndex / _blocksize;
    absoluteIndex = absoluteIndex - block*_blocksize;

    size_t relativeIndexInBlock = std::find(_values.begin(), _values.end(), absoluteIndex) - _values.begin();
    return relativeIndexInBlock + block*_valuesInBlock;
}

////////////////////////////////////////////////////////////////////

int RandomAssigner::rankForIndex(size_t index) const
{
    // Calculate the index of the block corresponding to this (absolute) index and the corresponding
    // (absolute) index in the first block
    size_t block = index / _blocksize;
    index = index - block*_blocksize;

    return _assignment[index];
}

////////////////////////////////////////////////////////////////////

bool RandomAssigner::parallel() const
{
    return true;
}

////////////////////////////////////////////////////////////////////
