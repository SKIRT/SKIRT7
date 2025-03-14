/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "FatalError.hpp"
#include "ProcessAssigner.hpp"

////////////////////////////////////////////////////////////////////

ProcessAssigner::ProcessAssigner(size_t size, SimulationItem* parent)
    : _comm(0), _assigned(0), _total(size)
{
    setParent(parent);
    setup();

    try
    {
        // get a pointer to the PeerToPeerCommunicator without performing setup
        // to avoid catching (and hiding) fatal errors during such setup
        _comm = find<PeerToPeerCommunicator>(false);
    }
    catch (FatalError)
    {
        return;
    }

    // Do the find operation again, now to perform the setup of the
    // PeerToPeerCommunicator so that the correct rank is initialized
    _comm = find<PeerToPeerCommunicator>();
}

////////////////////////////////////////////////////////////////////

void ProcessAssigner::setAssigned(size_t assigned)
{
    _assigned = assigned;
}

////////////////////////////////////////////////////////////////////

size_t ProcessAssigner::total() const
{
    return _total;
}

////////////////////////////////////////////////////////////////////

size_t ProcessAssigner::assigned() const
{
    return _assigned;
}

////////////////////////////////////////////////////////////////////

size_t ProcessAssigner::assignedForRank(int rank) const
{
    size_t result = 0;

    for (size_t absoluteIndex=0; absoluteIndex<total(); absoluteIndex++)
        if (rank == rankForIndex(absoluteIndex)) result++;
    return result;
}

////////////////////////////////////////////////////////////////////

bool ProcessAssigner::validIndex(size_t absoluteIndex) const
{
    return _comm->rank() == rankForIndex(absoluteIndex);
}

////////////////////////////////////////////////////////////////////

std::vector<int> ProcessAssigner::indicesForRank(int rank) const
{
    std::vector<int> result;
    result.reserve(_assigned);

    // add all the absolute indices that correspond to the given rank
    for (size_t absoluteIndex=0; absoluteIndex<total(); absoluteIndex++)
        if (rank == rankForIndex(absoluteIndex))
            result.push_back(absoluteIndex);

    return result;
}

////////////////////////////////////////////////////////////////////
