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
    : _blocksize(0), _valuesInBlock(0)
{
}

////////////////////////////////////////////////////////////////////

RandomAssigner::RandomAssigner(SimulationItem *parent)
    : _blocksize(0), _valuesInBlock(0)
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

void RandomAssigner::assign(size_t size, size_t blocks)
{
    _blocksize = size;
    _assignment.resize(size);
    _values.reserve(1.2*size/_comm->size());

    // For each value in a certain subset of 'size', let this process determine a random process rank
    SequentialAssigner* helpassigner = new SequentialAssigner(this);
    helpassigner->assign(size);
    for (size_t i = 0; i < helpassigner->nvalues(); i++)
    {
        int rank = round(_random->uniform() * _comm->size());
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
    _nvalues = _values.size()*blocks;
}

////////////////////////////////////////////////////////////////////

size_t RandomAssigner::absoluteIndex(size_t relativeIndex)
{
    // Calculate the index of the block corresponding to this relativeIndex and the corresponding
    // relativeIndex in the first block
    size_t block = relativeIndex / _valuesInBlock;
    relativeIndex = relativeIndex - block*_valuesInBlock;

    return _values[relativeIndex];
}

////////////////////////////////////////////////////////////////////

size_t RandomAssigner::relativeIndex(size_t absoluteIndex)
{
    // Calculate the index of the block corresponding to this absoluteIndex and the corresponding
    // absoluteIndex in the first block
    size_t block = absoluteIndex / _blocksize;
    absoluteIndex = absoluteIndex - block*_blocksize;

    return std::find(_values.begin(), _values.end(), absoluteIndex) - _values.begin();
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
