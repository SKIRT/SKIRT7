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

void PeerToPeerCommunicator::sendDouble(double& buffer, int receiver, int tag)
{
    if (!isMultiProc()) return;

    ProcessManager::sendDouble(buffer,receiver,tag);
}

////////////////////////////////////////////////////////////////////

void PeerToPeerCommunicator::receiveDouble(double& buffer, int sender, int tag)
{
    if (!isMultiProc()) return;

    ProcessManager::receiveDouble(buffer,sender,tag);
}

////////////////////////////////////////////////////////////////////

void PeerToPeerCommunicator::finishRequests()
{
    if (!isMultiProc()) return;

    ProcessManager::wait_all();
}

////////////////////////////////////////////////////////////////////

void PeerToPeerCommunicator::gatherw(double* sendBuffer, int sendCount,
                                     double* recvBuffer, int recvRank, int recvLength,
                                     const std::vector<std::vector<int>>& recvDisplacements)
{
    if(!isMultiProc()) return;

    ProcessManager::gatherw(sendBuffer, sendCount, recvBuffer, recvRank, recvLength, recvDisplacements);
}

void PeerToPeerCommunicator::scatterw(double* sendBuffer, int sendRank, int sendLength,
                                      const std::vector<std::vector<int>>& sendDisplacements,
                                      double* recvBuffer, int recvCount)
{
    if(!isMultiProc()) return;

    ProcessManager::scatterw(sendBuffer, sendRank, sendLength, sendDisplacements, recvBuffer, recvCount);
}

////////////////////////////////////////////////////////////////////

void PeerToPeerCommunicator::presetGatherw(double *sendBuffer, int sendCount, double *recvBuffer, int recvRank)
{
    if(!isMultiProc()) return;

    ProcessManager::presetGatherw(sendBuffer, sendCount, recvBuffer, recvRank);
}

////////////////////////////////////////////////////////////////////

void PeerToPeerCommunicator::presetScatterw(double *sendBuffer, int sendRank, double *recvBuffer, int recvCount)
{
    if(!isMultiProc()) return;

    ProcessManager::presetScatterw(sendBuffer, sendRank, recvBuffer, recvCount);
}

////////////////////////////////////////////////////////////////////

void PeerToPeerCommunicator::presetConfigure(int length, const std::vector<std::vector<int> > &displacements)
{
    if(!isMultiProc()) return;

    ProcessManager::presetConfigure(length,displacements);
}

////////////////////////////////////////////////////////////////////

void PeerToPeerCommunicator::presetClear()
{
    if(!isMultiProc()) return;

    ProcessManager::presetClear();
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
