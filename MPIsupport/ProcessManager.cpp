/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifdef BUILDING_WITH_MPI
#include <mpi.h>
#endif

#include "ProcessManager.hpp"
#include <QDataStream>

////////////////////////////////////////////////////////////////////

std::atomic<int> ProcessManager::requests(0);

//////////////////////////////////////////////////////////////////////

void ProcessManager::initialize(int *argc, char ***argv)
{
#ifdef BUILDING_WITH_MPI
    int initialized;
    MPI_Initialized(&initialized);
    if (!initialized)
    {
        MPI_Init(argc, argv);
    }
#else
    Q_UNUSED(argc) Q_UNUSED(argv)
#endif
}

//////////////////////////////////////////////////////////////////////

void ProcessManager::finalize()
{
#ifdef BUILDING_WITH_MPI
    MPI_Finalize();
#endif
}

//////////////////////////////////////////////////////////////////////

void ProcessManager::acquireMPI(int& rank, int& Nprocs)
{
#ifdef BUILDING_WITH_MPI
    int oldrequests = requests++;

    if (oldrequests) // if requests >=1
    {
        Nprocs = 1;
        rank = 0;
    }
    else // requests == 0
    {
        MPI_Comm_size(MPI_COMM_WORLD, &Nprocs);
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    }
#else
    rank = 0;
    Nprocs = 1;
#endif
}

//////////////////////////////////////////////////////////////////////

void ProcessManager::releaseMPI()
{
#ifdef BUILDING_WITH_MPI
    requests--;
#endif
}

//////////////////////////////////////////////////////////////////////

void ProcessManager::barrier()
{
#ifdef BUILDING_WITH_MPI
    MPI_Barrier(MPI_COMM_WORLD);
#endif
}

//////////////////////////////////////////////////////////////////////

void ProcessManager::sendByteBuffer(QByteArray& buffer, int receiver, int tag)
{
#ifdef BUILDING_WITH_MPI
    MPI_Send(buffer.data(), buffer.size(), MPI_BYTE, receiver, tag, MPI_COMM_WORLD);
#else
    Q_UNUSED(buffer) Q_UNUSED(receiver) Q_UNUSED(tag)
#endif
}

//////////////////////////////////////////////////////////////////////

void ProcessManager::receiveByteBuffer(QByteArray& buffer, int& sender)
{
#ifdef BUILDING_WITH_MPI
    MPI_Status status;
    MPI_Recv(buffer.data(), buffer.size(), MPI_BYTE, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    sender = status.MPI_SOURCE;
#else
    Q_UNUSED(buffer) Q_UNUSED(sender)
#endif
}

//////////////////////////////////////////////////////////////////////

void ProcessManager::receiveByteBuffer(QByteArray &buffer, int sender, int& tag)
{
#ifdef BUILDING_WITH_MPI
    MPI_Status status;
    MPI_Recv(buffer.data(), buffer.size(), MPI_BYTE, sender, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    tag = status.MPI_TAG;
#else
    Q_UNUSED(buffer) Q_UNUSED(sender) Q_UNUSED(tag)
#endif
}

//////////////////////////////////////////////////////////////////////

void ProcessManager::sum(double* my_array, double* result_array, int nvalues, int root)
{
#ifdef BUILDING_WITH_MPI
    MPI_Reduce(my_array, result_array, nvalues, MPI_DOUBLE, MPI_SUM, root, MPI_COMM_WORLD);
#else
    Q_UNUSED(my_array) Q_UNUSED(result_array) Q_UNUSED(nvalues) Q_UNUSED(root)
#endif
}

//////////////////////////////////////////////////////////////////////

void ProcessManager::sum_all(double* my_array, int nvalues)
{
#ifdef BUILDING_WITH_MPI
    MPI_Allreduce(MPI_IN_PLACE, my_array, nvalues, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
#else
    Q_UNUSED(my_array) Q_UNUSED(nvalues)
#endif
}

//////////////////////////////////////////////////////////////////////

void ProcessManager::broadcast(double* my_array, int nvalues, int root)
{
#ifdef BUILDING_WITH_MPI
    MPI_Bcast(my_array, nvalues, MPI_DOUBLE, root, MPI_COMM_WORLD);
#else
    Q_UNUSED(my_array) Q_UNUSED(nvalues) Q_UNUSED(root)
#endif
}

//////////////////////////////////////////////////////////////////////

bool ProcessManager::isRoot()
{
#ifdef BUILDING_WITH_MPI
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    return (rank == 0);
#else
    return true;
#endif
}

//////////////////////////////////////////////////////////////////////

bool ProcessManager::isMultiProc()
{
#ifdef BUILDING_WITH_MPI
    int size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    return (size > 1);
#else
    return false;
#endif
}

//////////////////////////////////////////////////////////////////////
