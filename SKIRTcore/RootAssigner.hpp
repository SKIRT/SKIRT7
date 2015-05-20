/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef ROOTASSIGNER_HPP
#define ROOTASSIGNER_HPP

#include "ProcessAssigner.hpp"

//////////////////////////////////////////////////////////////////////

/** The RootAssigner class is a subclass of the ProcessAssigner class, representing objects that
    assign work to different processes. The RootAssigner class is used to assign all of the work to
    the root process, as defined by the PeerToPeerCommunicator class, so that the other processes
    do not execute the corresponding code. This is useful for e.g. I/O operations; therefore
    objects of type RootAssigner will be found in various writeXXX() functions across the \c SKIRT
    code. The RootAssigner is also the only type of ProcessAssigner that can be used when there is
    no PeerToPeerCommunicator available, for example in code that is outside of the simulation
    hierarchy and/or where the multiprocessing environment is not (yet) initialized, or in the
    MasterSlaveManager class (used by \c FitSKIRT). If this PeerToPeerCommunicator is not
    available, a value of zero can be passed to the constructor of RootAssigner. In this case, the
    RootAssigner object assumes multiprocessing is not enabled and thus every process that uses it
    is considered root and will be consequently assigned to all of the work. In MasterSlaveManager,
    this is useful since a certain workload is already specific to a certain process, so we want to
    assign all parts of this work to this process, but we can't use an IdenticalAssigner since this
    assigner can not be combined with multithreading (IdenticalAssigner::parallel() returns false). */
class RootAssigner : public ProcessAssigner
{
    Q_OBJECT
    Q_CLASSINFO("Title", "an assigner that only assigns the root process to all of the work")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE RootAssigner();

    /** This constructor can be invoked by SKIRT classes that wish to hard-code the creation of a new
        ProcessAssigner object of this type (as opposed to creation through the ski file). Before the
        constructor returns, the newly created object is hooked up as a child to the specified parent
        in the simulation hierarchy (so it will automatically be deleted) and the setup of the
        ProcessAssigner base class is invoked, which sets the _comm attribute that points to the object
        of type PeerToPeerCommunicator that is found in the simulation hierarchy. */
    explicit RootAssigner(SimulationItem* parent);

    //======================== Other Functions =======================

public:
    /** This function invokes the assignment procedure. As a first argument, it takes the number of
        parts of work that need to be performed. As a second optional argument, it takes the number of
        blocks; which represents the number of times a set of \c size parts is encountered in the
        entire collection that holds each and every distinguishable piece of work. In this class, the
        assignment is straightforward. If a PeerToPeerCommunicator is available, only the root process
        (defined by this communicator) is assigned to all of the work (\c size \f$\times\f$ \c blocks
        parts). If this communicator was not provided, a process is assigned to all parts of work
        regardless of its rank in a multiprocessing environment to which it belongs (if any). */
    void assign(size_t size, size_t blocks = 1);

    /** This function takes the relative index of a certain part of the work assigned to this process
        as an argument and returns the absolute index of that part, a value from zero to the total
        amount of parts that need to be executed in the simulation. If a PeerToPeerCommunicator is
        available and the calling process is not the root, a fatal error is thrown. Otherwise, the
        argument is returned. */
    size_t absoluteIndex(size_t relativeIndex);

    /** This function takes the absolute index of a certain part of the work as an argument and returns
        the relative index of that part, a value from zero to the number of parts that were assigned to
        this process, _nvalues. If a PeerToPeerCommunicator is available and the calling process is not
        the root, a fatal error is thrown. Otherwise, the argument is returned. */
    size_t relativeIndex(size_t absoluteIndex);

    /** This function which must be implemented in each subclass of ProcessAssigner, returns the rank
        assigned to a certain part of the work. In this class, it returns the rank of the root as
        defined in the PeerToPeerCommunicator class, if such an object is available. Otherwise, a fatal
        error is thrown. */
    int rankForIndex(size_t index) const;

    /** In this class, this function returns true, since only the root process is assumed to execute a
        certain part of the code so that multithreading is allowed. */
    bool parallel() const;
};

////////////////////////////////////////////////////////////////////

#endif // ROOTASSIGNER_HPP
