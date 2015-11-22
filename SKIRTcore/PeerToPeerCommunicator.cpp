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

void PeerToPeerCommunicator::col_to_row_distributed(Table<2>& source, ProcessAssigner* colAssigner,
                                                    Table<2>& target, ProcessAssigner* rowAssigner,
                                                    int totalRows, int totalCols)
{
    if (!isMultiProc()) return;

    for (int ell=0; ell<totalCols; ell++) // for each possible column of the big array
        for (int m=0; m<totalRows; m++) // for each possible row of the big array
        {
            int tgtRank = rowAssigner->rankForIndex(m); // the rank where target has row m in it
            int srcRank = colAssigner->rankForIndex(ell); // the rank where the source has column ell in it
            // if this process has column ell, do send
            if (rank()==srcRank)
            {
                double& sendbuf = source(m,colAssigner->relativeIndex(ell));
                int tag = m*totalCols + ell; // unique for each position in the big array
                // copy it if my 'target' has row m, else send it
                if (rank()==tgtRank) target(rowAssigner->relativeIndex(m), ell) = sendbuf;
                else ProcessManager::sendDouble(sendbuf,tgtRank,tag);
            }
            // if this process does not have column ell, receive if targets needs m.
            // there will always be one process that posts a receive
            else if (rank()==tgtRank)
            {
                double& recvbuf = target(rowAssigner->relativeIndex(m),ell);
                int tag = m*totalCols + ell;
                // receive it only if source is on another process, otherwise it is already copied above here
                if (rank()!=srcRank) ProcessManager::receiveDouble(recvbuf,srcRank,tag);
            }
        }
    wait("communicating Labs tables"); // wait for request handles here? new function needed...
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
