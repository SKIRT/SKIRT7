/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "FatalError.hpp"
#include "ProcessAssigner.hpp"

////////////////////////////////////////////////////////////////////

ProcessAssigner::ProcessAssigner()
    : _comm(0), _nvalues(0)
{
}

////////////////////////////////////////////////////////////////////

void ProcessAssigner::setupSelfBefore()
{
    try
    {
        // get a pointer to the PeerToPeerCommunicator without performing setup
        // to avoid catching (and hiding) fatal errors during such setup
        _comm = find<PeerToPeerCommunicator>(false);
    }
    catch (FatalError)
    {
        throw FATALERROR("Cannot find an object of type PeerToPeerCommunicator in the simulation hierarchy");
    }

    // Do the find operation again, now to perform the setup of the
    // PeerToPeerCommunicator so that the correct rank is initialized
    _comm = find<PeerToPeerCommunicator>();
}

////////////////////////////////////////////////////////////////////

size_t ProcessAssigner::nvalues()
{
    return _nvalues;
}

////////////////////////////////////////////////////////////////////
