/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "FatalError.hpp"
#include "PeerToPeerCommunicator.hpp"
#include "SequentialAssigner.hpp"

////////////////////////////////////////////////////////////////////

SequentialAssigner::SequentialAssigner(size_t size, SimulationItem* parent)
    : ProcessAssigner(size, parent), _start(0), _quotient(0), _remainder(0)
{
    if (!_comm) throw FATALERROR("Could not find an object of type PeerToPeerCommunicator in the simulation hierarchy");

    int nprocs = _comm->size();   // The number of processes
    int rank = _comm->rank();     // The rank of this process

    _quotient = size / nprocs;
    _remainder = size % nprocs;

    // Calculate the number of values assigned to this process (in one block and in total)
    setAssigned(((size_t)rank < _remainder) ? _quotient + 1 : _quotient);

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

size_t SequentialAssigner::absoluteIndex(size_t relativeIndex) const
{
    return _start + relativeIndex;
}

////////////////////////////////////////////////////////////////////

size_t SequentialAssigner::relativeIndex(size_t absoluteIndex) const
{
    return absoluteIndex - _start;
}

////////////////////////////////////////////////////////////////////

int SequentialAssigner::rankForIndex(size_t index) const
{
    int rank;
    if (index < _remainder * (_quotient + 1) )
    {
        rank = index / (_quotient + 1);
    }
    else
    {
        size_t index2 = index - _remainder*(_quotient + 1);
        rank = _remainder + index2 / _quotient;
    }

    return rank;
}

////////////////////////////////////////////////////////////////////
