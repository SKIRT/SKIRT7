/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "FatalError.hpp"
#include "RootAssigner.hpp"
#include "PeerToPeerCommunicator.hpp"

////////////////////////////////////////////////////////////////////

RootAssigner::RootAssigner(PeerToPeerCommunicator* comm)
    : ProcessAssigner(comm)
{
}

////////////////////////////////////////////////////////////////////

void RootAssigner::assign(size_t size, size_t blocks)
{
    if (_comm)
    {
        // Set the number of values assigned to this process
        _nvalues = _comm->isRoot() ? size*blocks : 0;
    }
    else
    {
        // Set the number of values assigned to this process
        _nvalues = size*blocks;
    }
}

////////////////////////////////////////////////////////////////////

size_t RootAssigner::absoluteIndex(size_t relativeIndex)
{
    if (_comm && !_comm->isRoot())
        throw FATALERROR("This function should not be called from any process other than the root");

    return relativeIndex;
}

////////////////////////////////////////////////////////////////////

size_t RootAssigner::relativeIndex(size_t absoluteIndex)
{
    if (_comm && !_comm->isRoot())
        throw FATALERROR("This function should not be called from any process other than the root");

    return absoluteIndex;
}

////////////////////////////////////////////////////////////////////

int RootAssigner::rankForIndex(size_t /*index*/) const
{
    if (!_comm)
        throw FATALERROR("This function should never be called");

    return _comm->root();
}

////////////////////////////////////////////////////////////////////

bool RootAssigner::parallel() const
{
    return true;
}

////////////////////////////////////////////////////////////////////
