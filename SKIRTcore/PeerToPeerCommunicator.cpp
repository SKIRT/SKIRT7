/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "Array.hpp"
#include "Log.hpp"
#include "PeerToPeerCommunicator.hpp"
#include "ProcessManager.hpp"
#include "ProcessAssigner.hpp"

////////////////////////////////////////////////////////////////////

#define ROOT 0

////////////////////////////////////////////////////////////////////

void PeerToPeerCommunicator::sum(Array& arr)
{
    if (!isMultiProc()) return;

    Array results(arr.size());

    ProcessManager::sum(&(arr[0]),&results[0],arr.size(),0);
    if (isRoot()) arr = results;
}

////////////////////////////////////////////////////////////////////

void PeerToPeerCommunicator::sum_all(Array& arr)
{
    if (!isMultiProc()) return;

    ProcessManager::sum_all(&(arr[0]),arr.size());
}

////////////////////////////////////////////////////////////////////

void PeerToPeerCommunicator::sum_all(double& dbl)
{
    if (!isMultiProc()) return;

    ProcessManager::sum_all(&dbl,1);
}

void PeerToPeerCommunicator::and_all(bool& b)
{
    if (!isMultiProc()) return;

    ProcessManager::and_all(&b);
}

////////////////////////////////////////////////////////////////////

void PeerToPeerCommunicator::broadcast(Array& arr, int sender)
{
    if (!isMultiProc()) return;

    ProcessManager::broadcast(&(arr[0]),arr.size(),sender);
}

////////////////////////////////////////////////////////////////////

void PeerToPeerCommunicator::broadcast(int& value, int sender)
{
    if (!isMultiProc()) return;

    ProcessManager::broadcast(&value,sender);
}

////////////////////////////////////////////////////////////////////

void PeerToPeerCommunicator::gatherw(double* sendBuffer, int sendCount,
                                     double* recvBuffer, int recvRank, int recvLength,
                                     const std::vector<std::vector<int>>& recvDisplacements)
{
    if(!isMultiProc()) return;

    ProcessManager::gatherw(sendBuffer, sendCount, recvBuffer, recvRank, recvLength, recvDisplacements);
}

////////////////////////////////////////////////////////////////////

void PeerToPeerCommunicator::displacedBlocksAllToAll(double* sendBuffer, int sendCount,
                                                     std::vector<std::vector<int>>& sendDisplacements, int sendLength,
                                                     int sendExtent, double* recvBuffer, int recvCount,
                                                     std::vector<std::vector<int>>& recvDisplacements, int recvLength,
                                                     int recvExtent)
{
    if(!isMultiProc()) return;

    ProcessManager::displacedBlocksAllToAll(sendBuffer, sendCount, sendDisplacements, sendLength, sendExtent,
                                            recvBuffer, recvCount, recvDisplacements, recvLength, recvExtent);
}

////////////////////////////////////////////////////////////////////

int PeerToPeerCommunicator::root()
{
    return ROOT;
}

////////////////////////////////////////////////////////////////////

bool PeerToPeerCommunicator::isRoot()
{
    return (rank() == ROOT);
}

////////////////////////////////////////////////////////////////////

void PeerToPeerCommunicator::wait(QString scope)
{
    if (!isMultiProc()) return;

    find<Log>()->info("Waiting for other processes to finish " + scope + "...");

    ProcessManager::barrier();
}

////////////////////////////////////////////////////////////////////
