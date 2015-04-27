/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef PROCESSASSIGNER_HPP
#define PROCESSASSIGNER_HPP

#include <cstddef>
#include "PeerToPeerCommunicator.hpp"
#include "SimulationItem.hpp"

//////////////////////////////////////////////////////////////////////

/** ProcessAssigner is an abstract class representing objects that assign different parts of work
    to the different processes in the simulation. Different subclasses of ProcessAssigner do this
    in different ways. A ProcessAssigner can be a part of any item in the simulation hierarchy. The
    DustLib object for example, can incorporate a ProcessAssigner object to define the way in which
    the process work together to calculate the dust emission for the different library items. The
    ProcessAssigner can also be associated with for example the WavelengthGrid, to assign the
    simulation of different wavelengths to different processes. The ProcessAssigner is always used
    in combination with the PeerToPeerCommunicator of the simulation. The latter defines the
    communications needed between the processes, while the former defines the work assignment. The
    MasterSlaveCommunicator implements the work assignment itself; letting one process hand out
    the parts of work to the others by sending messages. */
class ProcessAssigner : public SimulationItem
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a process assigner")

    //============= Construction - Setup - Destruction =============

protected:
    /** The default constructor; it is protected since this is an abstract class. */
    ProcessAssigner(PeerToPeerCommunicator* comm);

    //======================== Other Functions =======================

public:
    /** This function returns the number of values (or the number of parts of the total work) that are
        assigned to the calling process. This number is stored in the _nvalues data member. */
    size_t nvalues();

    /** This function invokes the assignment procedure. As a first argument, it takes the number of
        parts of work that need to be performed. As a second optional argument, it takes the number of
        blocks; which represents the number of times a set of \c size parts is encountered in the
        entire collection that holds each and every distinguishable piece of work. This function is
        declared purely virtual so it must be defined in either of the subclasses. The algorithm of
        this function is therefore implemented differently in each of the subclasses, and typically
        involves some calculations to determine which process is assigned to which parts of work. Each
        subclass must ensure that the assign function determines the number of values that are assigned
        to the process, and stores it in the _nvalues member. The typical behaviour of a subclass would
        be to repeat its assignment scheme used for the first \c size values, for as many times as
        there are blocks, if \c blocks > 1 (although the IdenticalAssigner subclass deviates from this
        behaviour). */
    virtual void assign(size_t size, size_t blocks = 1) = 0;

    /** This purely virtual function must be implemented in each of the ProcessAssigner subclasses. As
        an argument, it can take any value between zero and the number of values assigned to the
        process, defined by the _nvalues variable. This value is the relative index some part of the
        work assigned to the process. This function translates that relative index to the absolute
        index corresponding with that part of work, a value between zero and the size argument passed
        to the assign function. According to their assignment procedure, each ProcessAssigner subclass
        defines this function in a different way. */
    virtual size_t absoluteIndex(size_t relativeIndex) = 0;

    /** This purely virtual function must be implemented in each of the ProcessAssigner subclasses. As
        an argument, it takes a value from zero to the number of parts of work that is used for the
        assignment, minus one. This value is the absolute index of this part of work. This function
        translates that absolute index to the relative index corresponding with that part of work for
        this process, a value between zero and the number of values assigned to this process, _nvalues.
        According to their assignment procedure, each ProcessAssigner subclass defines this function in
        a different way. */
    virtual size_t relativeIndex(size_t absoluteIndex) = 0;

    /** This purely virtual function can be called to determine which process is assigned to a certain
        part of work. The index argument passed to this function is the absolute index of that part,
        ranging from zero to the size argument passed to the assign function. The return value is the
        rank of the process corresponding with that part of work. */
    virtual int rankForIndex(size_t index) const = 0;

    /** This function returns \c true if the different parts of work are distributed amongst the
        different processes and returns \c false if each process is assigned to the same work. Thus, it
        indicates whether multithreading can be safely used to parallelize the work even further or
        not. Since this is a purely virtual function, subclasses must implement it appropriately.
        Except for the IdenticalAssigner class, each subclass always returns true. */
    virtual bool parallel() const = 0;

    //======================== Data Members ========================

protected:
    PeerToPeerCommunicator* _comm;  // cached pointer to the peer-to-peer communicator of the simulation
    size_t _nvalues;                // the number of values assigned to this process
};

#endif // PROCESSASSIGNER_HPP
