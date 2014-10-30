/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef PROCESSCOMMUNICATOR_HPP
#define PROCESSCOMMUNICATOR_HPP

#include "SimulationItem.hpp"

////////////////////////////////////////////////////////////////////

/** The ProcessCommunicator class is an abstract class representing different types of
    communicators. The class inherits from SimulationItem so that it can be used within a SKIRT
    simulation hierarchy or FitSKIRT fitscheme hieararchy, and is accessible with the discovery
    mechanism. A communicator is used to represent an multiprocess environment, with a certain size
    (the number of processes) and where each process is assigned a certain rank. The communicator
    subclasses are designated for implementing the communication operations between its processes.
    Different communicators can implement different forms of communications, specific for a certain
    parallelization design. The ProcessCommunicator base class provides two data members: _rank and
    _Nprocs. These are declared private, meaning that the subclasses can only access these values
    through corresponding getter functions implemented by this class. */
class ProcessCommunicator : public SimulationItem
{
    Q_OBJECT

protected:
    /** The default constructor of the ProcessCommunicator class. Since this is an abstract class,
        the constructor is protected. Its job is to to initialize its _rank and _Nprocs to default
        values (both -1). Only when the setup of the class is called, these data members obtain
        their real values. */
    ProcessCommunicator();

    /** This function initiates the multiprocess environment. It does so by calling the acquireMPI
        function of the ProcessManager class, which will decide whether this communicator obtains the
        MPI resource or not. If it does, the _rank and _Nprocs data members are given their true value
        according to the MPI library. If the resource is not available at the time this function is
        called, the number of processes is set to one and the rank to zero. */
    void setupSelfBefore();

public:
    /** The (virtual) destructor of SimulationItem is redefined in this class because objects from
        classes inheriting from ProcessCommunicator acquire the MPI resource during their setup. When
        they are destroyed, this resource has to be released again, so that other objects can use it.
        Therefore, this destructor calls the releaseMPI function of the ProcessManager class. */
    ~ProcessCommunicator();

    /** This function returns the rank of the process. */
    int getRank() const;

    /** This function returns the size of the communicator, meaning the number of processes that are in
        its environment. */
    int getSize() const;

    /** This function returns true when multiple processes are in the environment of this communicator,
        and false when there is only one process (when the MPI resource is not available or turned on). */
    bool isMultiProc() const;

private:

    int _rank;      // The rank of the process
    int _Nprocs;    // The number of processes in the environment
};

#endif // PROCESSCOMMUNICATOR_HPP
