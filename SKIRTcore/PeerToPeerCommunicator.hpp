/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef PEERTOPEERCOMMUNICATOR_HPP
#define PEERTOPEERCOMMUNICATOR_HPP

#include "ArrayTable.hpp"
#include "ProcessCommunicator.hpp"
#include "Table.hpp"
class Array;
class ProcessAssigner;

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

    /** This function is used for broadcasting the values in an Array from one particular process to
        all other processes in this communicator. The rank of the sending process is indicated with the
        second argument. */
    void broadcast(Array& arr, int sender);

    /** This function is used for broadcasting an integer value from one particular process to all
        other processes in this communicator. The rank of the sending process is indicated with the
        second argument. */
    void broadcast(int& value, int sender);

    void sendDouble(double& buffer, int receiver, int tag);

    void receiveDouble(double& buffer, int sender, int tag);

    /** This function returns the rank of the root process. */
    int root();

    /** This function returns whether the process is assigned as the root within this communicator
        or not. */
    bool isRoot();

    /** This function does not return before all processes within the communicator have called it. */
    void wait(QString scope);
};

////////////////////////////////////////////////////////////////////

#endif // PEERTOPEERCOMMUNICATOR_HPP
