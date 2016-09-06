/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef PROCESSASSIGNER_HPP
#define PROCESSASSIGNER_HPP

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
    /** The constructor takes a number of work units as an argument. The constructed object will then
    decide how the work will be divided across the processes. This constructor is proctected
    because this is an abstract class. The algorithm for dividing the work is implemented
    differently in each of the subclasses, and typically involves some calculations to determine
    which process is assigned to which parts of the work. */
    ProcessAssigner(SimulationItem* parent, size_t size);

    /** Subclasses must use this protected setter to set _assigned to the correct value, after
    performing their assignment algorithm in the constructor */
    void setAssigned(size_t assigned);

    //======================== Other Functions =======================

public:
    /** Returns the total number of work units assigned to the processes, i.e. the argument given at
    construction. */
    size_t total() const;

    /** This function returns the number of values (or the number of parts of the total work) that are
        assigned to the calling process. */
    size_t assigned() const;

    /** Calculates the number of work units assigned to any given rank. */
    size_t assignedForRank(int rank) const;

    /** Returns true if this process was assigned to the given index. */
    bool validIndex(size_t absoluteIndex) const;

    /** Returns a vector containing all the absolute work indices assigned to a given process rank. */
    std::vector<int> indicesForRank(int rank) const;

    /** This purely virtual function must be implemented in each of the ProcessAssigner subclasses. As
        an argument, it can take any value between zero and the number of values assigned to the
        process, defined by the _assigned variable. This value is the relative index some part of the
        work assigned to the process. This function translates that relative index to the absolute
        index corresponding with that part of work, a value between zero and the size argument passed
        during construction. According to their assignment procedure, each ProcessAssigner subclass
        defines this function in a different way. */
    virtual size_t absoluteIndex(size_t relativeIndex) const = 0;

    /** This purely virtual function must be implemented in each of the ProcessAssigner subclasses. As
        an argument, it takes a value from zero to _total minus one. This value is the absolute index
        of this part of work. This function translates that absolute index to the relative index
        corresponding with that part of work for this process, a value between zero and the number of
        work units assigned to this process, _assigned. According to their assignment procedure, each
        ProcessAssigner subclass defines this function in a different way. */
    virtual size_t relativeIndex(size_t absoluteIndex) const = 0;

    /** This purely virtual function can be called to determine which process is assigned to a certain
        part of work. The index argument passed to this function is the absolute index of that part,
        ranging from zero to the size argument passed during construction. The return value is the
        rank of the process corresponding with that part of work. */
    virtual int rankForIndex(size_t index) const = 0;

    //======================== Data Members ========================

protected:
    PeerToPeerCommunicator* _comm;  // cached pointer to the peer-to-peer communicator of the simulation

private:
    size_t _assigned;                // the number of values assigned to this process
    size_t _total;
};

#endif // PROCESSASSIGNER_HPP
