/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifdef BUILDING_WITH_MPI
#include <mpi.h>
#endif

#include "ProcessManager.hpp"
#include <QDataStream>

std::atomic<int> ProcessManager::requests(0);

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

void ProcessManager::finalize()
{
#ifdef BUILDING_WITH_MPI
    MPI_Finalize();
#endif
}

void ProcessManager::acquireMPI(int& rank, int& Nprocs)
{
#ifdef BUILDING_WITH_MPI
    if (requests) // if requests >=1
    {
        requests++;

        Nprocs = 1;
        rank = 0;
    }
    else // requests = 0
    {
        requests++;

        MPI_Comm_size(MPI_COMM_WORLD, &Nprocs);
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    }
#else
    rank = 0;
    Nprocs = 1;
#endif
}

void ProcessManager::releaseMPI()
{
#ifdef BUILDING_WITH_MPI
    requests--;
#endif
}

void ProcessManager::barrier()
{
#ifdef BUILDING_WITH_MPI
    MPI_Barrier(MPI_COMM_WORLD);
#endif
}

void ProcessManager::sum(double* my_array, double* result_array, int nvalues, int root)
{
#ifdef BUILDING_WITH_MPI
    MPI_Reduce(my_array, result_array, nvalues, MPI_DOUBLE, MPI_SUM, root, MPI_COMM_WORLD);
#endif
}

void ProcessManager::sum_all(double* my_array, double* result_array, int nvalues)
{
#ifdef BUILDING_WITH_MPI
    MPI_Allreduce(my_array, result_array, nvalues, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
#endif
}

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
