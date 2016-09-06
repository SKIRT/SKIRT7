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

RandomAssigner::RandomAssigner(SimulationItem *parent, size_t size)
    : ProcessAssigner(parent, size)
{
    if (!_comm) throw FATALERROR("Could not find an object of type PeerToPeerCommunicator in the simulation hierarchy");
    _random = find<Random>();

    _assignment.clear();
    _assignment.resize(size);
    _values.clear();
    _values.reserve(1.2*size/_comm->size());

    // For each value in a certain subset of 'size', let this process determine a random process rank
    SequentialAssigner* helpassigner = new SequentialAssigner(this, size);
    for (size_t i = 0; i < helpassigner->assigned(); i++)
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
    setAssigned(_values.size());
}

////////////////////////////////////////////////////////////////////

size_t RandomAssigner::absoluteIndex(size_t relativeIndex) const
{
    return _values[relativeIndex];
}

////////////////////////////////////////////////////////////////////

size_t RandomAssigner::relativeIndex(size_t absoluteIndex) const
{
    return std::find(_values.begin(), _values.end(), absoluteIndex) - _values.begin();
}

////////////////////////////////////////////////////////////////////

int RandomAssigner::rankForIndex(size_t index) const
{
    return _assignment[index];
}

////////////////////////////////////////////////////////////////////
