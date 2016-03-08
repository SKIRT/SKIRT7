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

namespace
{
    std::vector<MPI_Request> pendingrequests;

    std::vector<MPI_Datatype> presetDatatypes;

    void createDisplacedDoubleBlocks(int blocklength, const std::vector<int>& displacements, MPI_Datatype* newtype)
    {
        int count = displacements.size();               // number of blocks

        std::vector<int> multiplied_disp;
        multiplied_disp.reserve(count);
        for(auto i: displacements) multiplied_disp.push_back(blocklength*i); // list of displacements in doubles as unit

        MPI_Type_create_indexed_block(count, blocklength, &multiplied_disp[0], MPI_DOUBLE, newtype);
    }
}

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

void ProcessManager::sendDouble(double& buffer, int receiver, int tag)
{
#ifdef BUILDING_WITH_MPI

    //MPI_Send(&buffer, 1, MPI_DOUBLE, receiver, tag, MPI_COMM_WORLD);

    MPI_Request request;
    MPI_Isend(&buffer, 1, MPI_DOUBLE, receiver, tag, MPI_COMM_WORLD, &request);
    pendingrequests.push_back(request);

#else
    Q_UNUSED(buffer) Q_UNUSED(receiver) Q_UNUSED(tag)
#endif
}

//////////////////////////////////////////////////////////////////////

void ProcessManager::receiveDouble(double& buffer, int sender, int tag)
{
#ifdef BUILDING_WITH_MPI

    //MPI_Recv(&buffer, 1, MPI_DOUBLE, sender, tag, MPI_COMM_WORLD, &st);

    MPI_Request request;
    MPI_Irecv(&buffer, 1, MPI_DOUBLE, sender, tag, MPI_COMM_WORLD, &request);
    pendingrequests.push_back(request);

#else
    Q_UNUSED(buffer) Q_UNUSED(receiver) Q_UNUSED(tag)
#endif
}

//////////////////////////////////////////////////////////////////////

void ProcessManager::gatherw(double* sendBuffer, int sendCount,
                             double* recvBuffer, int recvRank, int recvLength,
                             const std::vector<std::vector<int>>& recvDisplacements)
{
#ifdef BUILDING_WITH_MPI
    int size;
    int rank;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // parameters for the senders
    std::vector<int> sendcnts(size, 0);                     // every process sends nothing to all processes
    sendcnts[recvRank] = sendCount;                         // except to the receiver
    std::vector<int> sdispls(size, 0);                      // starting from sendBuffer + 0
    std::vector<MPI_Datatype> sendtypes(size, MPI_DOUBLE);  // all doubles

    // parameters on the receiving end
    std::vector<int> recvcnts;
    if (rank != recvRank) recvcnts.resize(size, 0); // I am not receiver: receive nothing from every process
    else recvcnts.resize(size, 1);                  // I am receiver: receive 1 from every process
    std::vector<int> rdispls(size, 0);              // displacements will be contained in the datatypes
    std::vector<MPI_Datatype> recvtypes;            // we will construct derived datatypes for receiving from each process
    recvtypes.reserve(size);

    for (int rank=0; rank<size; rank++)
    {
        MPI_Datatype newtype;
        createDisplacedDoubleBlocks(recvLength, recvDisplacements[rank], &newtype);
        MPI_Type_commit(&newtype);
        recvtypes.push_back(newtype);
    }

    MPI_Alltoallw(sendBuffer, &sendcnts[0], &sdispls[0], &sendtypes[0],
                  recvBuffer, &recvcnts[0], &rdispls[0], &recvtypes[0],
                  MPI_COMM_WORLD);

    for (int rank=0; rank<size; rank++) MPI_Type_free(&recvtypes[rank]);
#else
    Q_UNUSED(sendBuffer) Q_UNUSED(sendCount) Q_UNUSED(recvBuffer) Q_UNUSED(recvRank) Q_UNUSED(recvDisplacements)
#endif
}

//////////////////////////////////////////////////////////////////////

void ProcessManager::scatterw(double *sendBuffer, int sendRank, int sendLength,
                              const std::vector<std::vector<int> > &sendDisplacements,
                              double *recvBuffer, int recvCount)
{
#ifdef BUILDING_WITH_MPI
    int size;
    int rank;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // parameters for the sender
    std::vector<int> sendcnts;
    if (rank != sendRank) sendcnts.resize(size, 0);     // I am not sender: send nothing to every process
    else sendcnts.resize(size, 1);                      // I am sender: send 1 to each process
    std::vector<int> sdispls(size, 0);                  // displacements in buffer will be contained in datatypes
    std::vector<MPI_Datatype> sendtypes;                // these will be constructed for sending to each process
    sendtypes.reserve(size);

    for (int rank=0; rank<size; rank++)
    {
        MPI_Datatype newtype;
        createDisplacedDoubleBlocks(sendLength, sendDisplacements[rank], &newtype);
        MPI_Type_commit(&newtype);
        sendtypes.push_back(newtype);
    }

    // parameters for the receivers
    std::vector<int> recvcnts(size, 0);                     // each process will receive nothing from all processes
    recvcnts[sendRank] = recvCount;                         // except from the sender
    std::vector<int> rdispls(size, 0);                      // starting from recvBuffer + 0
    std::vector<MPI_Datatype> recvtypes(size, MPI_DOUBLE);  // all doubles
    recvtypes.reserve(size);

    MPI_Alltoallw(sendBuffer, &sendcnts[0], &sdispls[0], &sendtypes[0],
                  recvBuffer, &recvcnts[0], &rdispls[0], &recvtypes[0],
                  MPI_COMM_WORLD);

    for (int rank=0; rank<size; rank++) MPI_Type_free(&sendtypes[rank]);
#else
    Q_UNUSED(sendBuffer) Q_UNUSED(sendRank) Q_UNUSED(sendDisplacements) Q_UNUSED(recvBuffer) Q_UNUSED(recvCount)
#endif
}

//////////////////////////////////////////////////////////////////////

void ProcessManager::presetGatherw(double *sendBuffer, int sendCount, double *recvBuffer, int recvRank)
{
#ifdef BUILDING_WITH_MPI
    int size;
    int rank;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // parameters for the senders
    std::vector<int> sendcnts(size, 0);                     // every process sends...
    sendcnts[recvRank] = sendCount;                         // ... only to the receiver
    std::vector<int> sdispls(size, 0);                      // starting from sendBuffer + 0
    std::vector<MPI_Datatype> sendtypes(size, MPI_DOUBLE);  // using doubles

    // parameters on the receiving end
    std::vector<int> recvcnts;
    if (rank != recvRank) recvcnts.resize(size, 0); // I am not receiver: receive nothing from every process
    else recvcnts.resize(size, 1);                  // I am receiver: receive 1 custom type from every process
    std::vector<int> rdispls(size, 0);              // displacements will be contained in the datatypes
                                                    // use the preset datatypes to receive

    MPI_Alltoallw(sendBuffer, &sendcnts[0], &sdispls[0], &sendtypes[0],
                  recvBuffer, &recvcnts[0], &rdispls[0], &presetDatatypes[0],
                  MPI_COMM_WORLD);
#else
    Q_UNUSED(sendBuffer) Q_UNUSED(sendCount) Q_UNUSED(recvBuffer) Q_UNUSED(recvRank) Q_UNUSED(recvDisplacements)
#endif
}

//////////////////////////////////////////////////////////////////////

void ProcessManager::presetScatterw(double *sendBuffer, int sendRank, double *recvBuffer, int recvCount)
{
#ifdef BUILDING_WITH_MPI
    int size;
    int rank;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // parameters for the sender
    std::vector<int> sendcnts;
    if (rank != sendRank) sendcnts.resize(size, 0);     // I am not sender: send nothing to every process
    else sendcnts.resize(size, 1);                      // I am sender: send 1 to each process
    std::vector<int> sdispls(size, 0);                  // displacements in buffer will be contained in datatypes
                                                        // use the preset datatypes

    // parameters for the receivers
    std::vector<int> recvcnts(size, 0);                     // each process will receive nothing from all processes
    recvcnts[sendRank] = recvCount;                         // except from the sender
    std::vector<int> rdispls(size, 0);                      // starting from recvBuffer + 0
    std::vector<MPI_Datatype> recvtypes(size, MPI_DOUBLE);  // all doubles
    recvtypes.reserve(size);

    MPI_Alltoallw(sendBuffer, &sendcnts[0], &sdispls[0], &presetDatatypes[0],
                  recvBuffer, &recvcnts[0], &rdispls[0], &recvtypes[0],
                  MPI_COMM_WORLD);
#else
    Q_UNUSED(sendBuffer) Q_UNUSED(sendRank) Q_UNUSED(sendDisplacements) Q_UNUSED(recvBuffer) Q_UNUSED(recvCount)
#endif
}

//////////////////////////////////////////////////////////////////////

void ProcessManager::presetConfigure(int length, const std::vector<std::vector<int>>& displacements)
{
    for (int rank=0; rank<displacements.size(); rank++)
    {
        MPI_Datatype newtype;
        createDisplacedDoubleBlocks(length, displacements[rank], &newtype);
        MPI_Type_commit(&newtype);
        presetDatatypes.push_back(newtype);
    }
}

//////////////////////////////////////////////////////////////////////

void ProcessManager::presetClear()
{
    for (auto& datatype : presetDatatypes) MPI_Type_free(&datatype);

    presetDatatypes.resize(0);
}

//////////////////////////////////////////////////////////////////////

void ProcessManager::wait_all()
{
#ifdef BUILDING_WITH_MPI

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Status statuses[pendingrequests.size()];
    MPI_Waitall(pendingrequests.size(), &pendingrequests[0], &statuses[0]);
    pendingrequests.clear();

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

void ProcessManager::broadcast(int* value, int root)
{
#ifdef BUILDING_WITH_MPI
    MPI_Bcast(value, 1, MPI_INT, root, MPI_COMM_WORLD);
#else
    Q_UNUSED(value) Q_UNUSED(root)
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
