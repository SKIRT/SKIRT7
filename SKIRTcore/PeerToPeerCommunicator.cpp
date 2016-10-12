/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "Array.hpp"
#include "Log.hpp"
#include "PeerToPeerCommunicator.hpp"
#include "ProcessManager.hpp"

////////////////////////////////////////////////////////////////////

namespace
{
    const int ROOT = 0;
}

////////////////////////////////////////////////////////////////////

void PeerToPeerCommunicator::sum(Array& arr)
{
    if (!isMultiProc()) return;

    ProcessManager::sum(&(arr[0]),arr.size(),0);
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

void PeerToPeerCommunicator::or_all(bool& b)
{
    if (!isMultiProc()) return;

    ProcessManager::or_all(&b);
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

void PeerToPeerCommunicator::gatherw(const double* sendBuffer, size_t sendCount,
                                     double* recvBuffer, int recvRank, size_t recvLength,
                                     const std::vector<std::vector<int>>& recvDisplacements)
{
    if (!isMultiProc()) return;

    ProcessManager::gatherw(sendBuffer, sendCount, recvBuffer, recvRank, recvLength, recvDisplacements);
}

////////////////////////////////////////////////////////////////////

void PeerToPeerCommunicator::displacedBlocksAllToAll(const double* sendBuffer, size_t sendCount,
                                                     std::vector<std::vector<int>>& sendDisplacements, size_t sendLength,
                                                     size_t sendExtent, double* recvBuffer, size_t recvCount,
                                                     std::vector<std::vector<int>>& recvDisplacements, size_t recvLength,
                                                     size_t recvExtent)
{
    if (!isMultiProc()) return;

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

bool PeerToPeerCommunicator::dataParallel()
{
    return _dataParallel;
}

////////////////////////////////////////////////////////////////////

void PeerToPeerCommunicator::setDataParallel(bool dataParallel)
{
    _dataParallel = dataParallel;
}

////////////////////////////////////////////////////////////////////
