/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef PEERTOPEERCOMMUNICATOR_HPP
#define PEERTOPEERCOMMUNICATOR_HPP

#include "ProcessCommunicator.hpp"
class Array;

////////////////////////////////////////////////////////////////////

/** An object of the PeerToPeerCommunicator class, inheriting from ProcessCommunicator, represents
    an environment or ensemble of processes, which are able to communicate according to the
    peer-to-peer model. This means that this class defines functions which implement decentralized
    communications: each process can send to and receive data from each other process. For its
    current use, only collective communications are defined in this class, involving the
    participation of all processes. Communication does not have to go throuh one particular
    process, as in the MasterSlaveCommunicator. Note, however, that this class does define a
    function isRoot(), discriminating between the processes. This is so because some operations are
    only to be performed by one single process, for example logging and writing out the results of
    the simulation to file. The point is that no process delegates what other processes should do
    and that data can flow from each arbritrary process to another. */
class PeerToPeerCommunicator : public ProcessCommunicator
{
    Q_OBJECT

    //============= Construction - Setup - Destruction =============
public:
    /** This function should be used just after this object has been set up, to set the
        _dataParallel member to true if '-d' was specified on the commandline. */
    void setDataParallel(bool dataParallel);

    //====================== Other Functions =======================

public:
    /** This function is used for summing an Array of element-wise across the different processes in the
        communicator. The resulting values are then stored in the same Array passed to this function,
        on the root process. */
    void sum(Array& arr);

    /** This function is used for summing an Array element-wise across the different processes in the
        communicator. The resulting values are then stored in the same Array passed to this function,
        on all processes in the communicator. */
    void sum_all(Array& arr);

    /** This function is used for summing one double across the different processes in the
        communicator. The resulting value is then stored in the double passed to this function, on
        all processes in the communicator. */
    void sum_all(double& dbl);

    /** This function is used for performing the logical OR operation on a boolean across the different
    processes in the communicator. The resulting value is then stored in the boolean passed to this
    function, on all processes in the communicator. */
    void or_all(bool& b);

    /** This function is used for broadcasting the values in an Array from one particular process to
        all other processes in this communicator. The rank of the sending process is indicated with the
        second argument. */
    void broadcast(Array& arr, int sender);

    /** This function is used for broadcasting an integer value from one particular process to all
        other processes in this communicator. The rank of the sending process is indicated with the
        second argument. */
    void broadcast(int& value, int sender);

    /** This function gathers doubles sent by all processes into displaced positions at the receiving process */
    void gatherw(double* sendBuffer, size_t sendCount, double* recvBuffer, int recvRank, size_t recvLength,
                 const std::vector<std::vector<int>>& recvDisplacements);


    void displacedBlocksAllToAll(double* sendBuffer, size_t sendCount, std::vector<std::vector<int>>& sendDisplacements,
                                 size_t sendLength, size_t sendExtent,
                                 double* recvBuffer, size_t recvCount, std::vector<std::vector<int>>& recvDisplacements,
                                 size_t recvLength, size_t recvExtent);

    /** This function returns the rank of the root process. */
    int root();

    /** This function returns whether the process is assigned as the root within this communicator
        or not. */
    bool isRoot();

    /** This function does not return before all processes within the communicator have called it. */
    void wait(QString scope);

    /** This function indicates whether data parallelization is enabled or not */
    bool dataParallel();

private:
    bool _dataParallel;
};

////////////////////////////////////////////////////////////////////

#endif // PEERTOPEERCOMMUNICATOR_HPP
