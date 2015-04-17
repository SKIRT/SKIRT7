/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "PeerToPeerCommunicator.hpp"
#include "StaggeredAssigner.hpp"

////////////////////////////////////////////////////////////////////

StaggeredAssigner::StaggeredAssigner()
{
}

////////////////////////////////////////////////////////////////////

void StaggeredAssigner::assign(size_t size)
{
    _comm = find<PeerToPeerCommunicator>();

    for (size_t i = 0; i < size; i++)
    {
        int rank = i % _comm->size();
        if (rank == _comm->rank()) _nvalues++;
    }
}

////////////////////////////////////////////////////////////////////

size_t StaggeredAssigner::absoluteIndex(size_t relativeIndex)
{
    size_t absoluteIndex = _comm->rank() + relativeIndex * _comm->size();
    return absoluteIndex;
}

////////////////////////////////////////////////////////////////////

size_t StaggeredAssigner::relativeIndex(size_t absoluteIndex)
{
    size_t relativeIndex = (absoluteIndex - _comm->rank()) / _comm->size();
    return relativeIndex;
}

////////////////////////////////////////////////////////////////////

int StaggeredAssigner::rankForIndex(size_t index) const
{
    return (index % _comm->size());
}

////////////////////////////////////////////////////////////////////

bool StaggeredAssigner::parallel() const
{
    return true;
}

////////////////////////////////////////////////////////////////////
