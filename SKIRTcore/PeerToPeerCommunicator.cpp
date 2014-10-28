/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "Array.hpp"
#include "PeerToPeerCommunicator.hpp"
#include "ProcessManager.hpp"

PeerToPeerCommunicator::PeerToPeerCommunicator()
{
}

void PeerToPeerCommunicator::sum(Array& arr, bool toall)
{
    if (!isMultiProc()) return;

    Array results(arr.size());

    if (toall)
    {
        ProcessManager::sum_all(&(arr[0]),&results[0],arr.size());
        arr = results;
    }
    else
    {
        ProcessManager::sum(&(arr[0]),&results[0],arr.size(),0);
        if (!_rank) arr = results;
    }
}

bool PeerToPeerCommunicator::isRoot()
{
    return !_rank;
}
