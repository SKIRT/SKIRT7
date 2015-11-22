/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "FatalError.hpp"
#include "ProcessAssigner.hpp"

////////////////////////////////////////////////////////////////////

ProcessAssigner::ProcessAssigner()
    : _comm(0), _nvalues(0), _blocksize(0)
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
