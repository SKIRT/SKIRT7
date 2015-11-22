/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef RANDOMASSIGNER_HPP
#define RANDOMASSIGNER_HPP

#include <vector>
#include "ProcessAssigner.hpp"
class Random;

//////////////////////////////////////////////////////////////////////

/** The RandomAssigner class is a subclass of the ProcessAssigner class, representing objects that
    assign work to different processes. An object of the RandomAssigner class distributes the work
    amongst the different processes in a random way. For each value (part of work), the assigned
    process is determined by drawing a uniform random number. Because the state of the random
    number generator can be different in each process, each process performs the random assignment
    for a subset of the different values, after which the results are gathered across all
    processes.

    When the assignment procedure of this class (the assign() function) is called with a number of
    blocks that is greater than one, the randomly determined assignment scheme is repeated \f$m\f$
    times, where \f$m\f$ is the number of blocks. */
class RandomAssigner : public ProcessAssigner
{
    Q_OBJECT
    Q_CLASSINFO("Title", "an assigner that assigns the work to the processes in a staggered way")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE RandomAssigner();

    /** This constructor can be invoked by SKIRT classes that wish to hard-code the creation of a new
        ProcessAssigner object of this type (as opposed to creation through the ski file). Before the
        constructor returns, the newly created object is hooked up as a child to the specified parent
        in the simulation hierarchy (so it will automatically be deleted) and the setup of the
        ProcessAssigner base class is invoked, which sets the _comm attribute that points to the object
        of type PeerToPeerCommunicator that is found in the simulation hierarchy. */
    explicit RandomAssigner(SimulationItem* parent);

    /** This function caches the pointer to the Random object in the simulation hierarchy and verifies
        that the pointer to the PeerToPeerCommunicator was set by the base class. If it was not, a
        FatalError is thrown. */
    void setupSelfBefore();

    RandomAssigner* clone();

protected:
    void copyFrom(const RandomAssigner* from);

    //======================== Other Functions =======================

public:
    /** This function invokes the assignment procedure. As an argument, it takes the number of parts of
        work \f$n\f$ that need to be performed. For each part of work \f$i\f$ (with \f$i\f$ ranging
        from 0 to \f$n-1\f$), this function determines the rank of the assigned process, by drawing
        random numbers from a uniform distribution. By using a SequentialAssigner, the range of indices
        \f$i\f$ is split in different domains, so that each process must draw these random numbers only
        for one particular domain. Subsequently, the randomly determined ranks are synchronized across
        all processes, so that every process knows which process is assigned to any part of the work
        (stored in the _assignment vector). Each process then determines which values it is assigned to
        itsself, and stores these values in a seperate list (the _values attribute). This function also
        sets the _blockSize, _valuesInBlock and _nvlaues attributes.
        If \c blocks > 1, the assignment scheme is repeated \c blocks times. */
    void assign(size_t size, size_t blocks = 1);

    void setBlocks(size_t blocks);

    /** This function takes the relative index of a certain part of the work assigned to this process
        as an argument and returns the absolute index of that part, a value from zero to the total
        amount of parts that need to be executed in the simulation. This is done by simply looking up
        the value in the _values list. */
    size_t absoluteIndex(size_t relativeIndex);

    /** This function takes the absolute index of a certain part of the work as an argument and returns
        the relative index of that part, a value from zero to the number of parts that were assigned to
        this process, _nvalues. This is done by performing a search through the _values list for the
        value that matches the absoluteIndex. The relative index that is returned corresponds to the
        index of that value in the _values list. */
    size_t relativeIndex(size_t absoluteIndex);

    /** This function returns the rank of the process that is assigned to a certain part of the work.
        This part is identified by its absolute index, passed to this function as an argument. This is
        done by simply looking up the corresponding rank in the _assignment vector. */
    int rankForIndex(size_t index) const;

    /** This function returns true if the different parts of work are distributed amongst the different
        processes and returns false if each process is assigned to the same work. In this class, the
        processes are assigned to different work so this function returns true. */
    bool parallel() const;

    //======================== Data Members ========================

protected:
    Random* _random;                // a pointer to the Random object of the simulation
    std::vector<int> _assignment;   // for each value, this vector defines the rank of the assigned process
    std::vector<size_t> _values;    // a list of the values assigned to this process
    size_t _valuesInBlock;          // the number of parts of work in a block assigned to this process
};

//////////////////////////////////////////////////////////////////////

#endif // RANDOMASSIGNER_HPP
