/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "FatalError.hpp"
#include "ProcessAssigner.hpp"

////////////////////////////////////////////////////////////////////

ProcessAssigner::ProcessAssigner()
    : _comm(0), _nvalues(0), _blocksize(0), _nblocks(1)
{
}

////////////////////////////////////////////////////////////////////

void ProcessAssigner::setupSelfBefore()
{
    SimulationItem::setupSelfBefore();

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

void ProcessAssigner::copyFrom(const ProcessAssigner *from)
{
    _comm = from->_comm;
    _nvalues = from->_nvalues;
    _blocksize = from->_blocksize;
}

////////////////////////////////////////////////////////////////////

size_t ProcessAssigner::nvalues() const
{
    return _nvalues;
}

////////////////////////////////////////////////////////////////////

size_t ProcessAssigner::total() const
{
    return _blocksize*_nblocks;
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
    result.reserve(_nvalues);

    // add all the absolute indices that correspond to the given rank
    for (size_t absoluteIndex=0; absoluteIndex<total(); absoluteIndex++)
        if (rank == rankForIndex(absoluteIndex))
            result.push_back(absoluteIndex);

    return result;
}
