/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "FatalError.hpp"
#include "PeerToPeerCommunicator.hpp"
#include "StaggeredAssigner.hpp"

////////////////////////////////////////////////////////////////////

StaggeredAssigner::StaggeredAssigner(size_t size, SimulationItem* parent)
    : ProcessAssigner(size, parent)
{
    if (!_comm) throw FATALERROR("Could not find an object of type PeerToPeerCommunicator in the simulation hierarchy");

    size_t assigned = 0;
    for (size_t i = 0; i < size; i++)
    {
        int rank = i % _comm->size();
        if (rank == _comm->rank()) assigned++;
    }

    // Set the total number of assigned values
    setAssigned(assigned);
}

////////////////////////////////////////////////////////////////////

size_t StaggeredAssigner::absoluteIndex(size_t relativeIndex) const
{
    return _comm->rank() + relativeIndex*_comm->size();
}

////////////////////////////////////////////////////////////////////

size_t StaggeredAssigner::relativeIndex(size_t absoluteIndex) const
{
    return ((absoluteIndex - _comm->rank()) / _comm->size());
}

////////////////////////////////////////////////////////////////////

int StaggeredAssigner::rankForIndex(size_t index) const
{
    return (index % _comm->size());
}

////////////////////////////////////////////////////////////////////
