/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef IDENTICALASSIGNER_HPP
#define IDENTICALASSIGNER_HPP

#include "ProcessAssigner.hpp"
#include "SequentialAssigner.hpp"

//////////////////////////////////////////////////////////////////////

/** The IdenticalAssigner class is a subclass of the ProcessAssigner class, representing objects
    that assign work to different processes. In its primary use, the IdenticalAssigner assign each
    process to the same work. When a certain method in another class requires the execution of
    different small parts of work, and it uses an object of this class as the process assigner,
    then all processes will execute all these parts. The execution of these parts is thus not
    parallelized with regard to the different processes, so the same result is expected on each
    process afterwards. In this case, the parallel() function of this class will return \em false,
    meaning that parallelizing the work amongst different threads is not allowed. Since each
    process executes the same algorithm, no communication is needed afterwards to accumulate the
    results.

    The assignment mechanism explained above is represented graphically in the following figure.

    \image html identicalassigner.png "The IdenticalAssigner class assigns each process to the same work."

    When the assignment procedure of this class (the assign() function) is called with a number of
    blocks that is greater than one, the behaviour is completely different. In each block, each
    process is assigned to all of the parts of work, similar as its standard behaviour, but the
    processes are not assigned to each block. Instead, an instance of the SequentialAssigner class
    is used to distribute the blocks amongst the different processes. Subsequent calls to
    absoluteIndex, relativeIndex or rankForIndex are then redirected to this companion assigner.
    Naturally, the parallel() function will now return true, since each block (and each part of
    work associated with it) is assigned to only one process. The rationale for this behaviour,
    apart from its usefulness in the MonteCarloSimulation class (and subclasses) for shooting
    photon packages, is that using an IdenticalAssigner to assign \f$n\f$ parts of work and copying
    this assignment scheme for \f$m\f$ blocks (\f$m>0\f$) would be completely analogous to simply
    assigning \f$n \times m\f$ parts of work (since each process would be assigned to each part of
    work and consequently to every block too). Therefore, the behaviour of the IdenticalAssigner is
    different from the other assigners in the way that it treats these blocks; instead of copying
    the assigment for each block, the IdenticalAssigner essentially becomes a parallel assigner
    when \c blocks > 1. */
class IdenticalAssigner : public ProcessAssigner
{
    Q_OBJECT
    Q_CLASSINFO("Title", "an assigner that assigns each process to the same work")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE IdenticalAssigner();

    /** This constructor can be invoked by SKIRT classes that wish to hard-code the creation of a new
        ProcessAssigner object of this type (as opposed to creation through the ski file). Before the
        constructor returns, the newly created object is hooked up as a child to the specified parent
        in the simulation hierarchy (so it will automatically be deleted) and the setup of the
        ProcessAssigner base class is invoked, which sets the _comm attribute that points to the object
        of type PeerToPeerCommunicator that is found in the simulation hierarchy. */
    explicit IdenticalAssigner(SimulationItem* parent);

    /** This function verifies that the pointer to the PeerToPeerCommunicator was set by the base
        class. If it was not, a FatalError is thrown. */
    void setupSelfBefore();

    IdenticalAssigner* clone();

protected:
    void copyFrom(const IdenticalAssigner* from);

    //======================== Other Functions =======================

public:
    /** This function invokes the assignment procedure. As a first argument, it takes the number of
        parts of work that need to be performed. As a second optional argument, it takes the number of
        blocks; which represents the number of times a set of \c size parts is encountered in the
        entire collection that holds each and every distinguishable piece of work. If the \c blocks
        argument is omitted from the call to this function, or set to 1, the assigment procedure in
        this class is straightforward and does not require any calculations. In this case, each process
        is assigned to each part of work. The value of the \em size argument is just copied into the \c
        _nvalues member. If \c blocks > 1, not all \c size \f$\times\f$ \c blocks parts of work are
        assigned to each process. Each block is assigned entirely to one particular process, but the
        assignment of these blocks to the different processes is directed by an instance of the
        SequentialAssigner class. */
    void assign(size_t size, size_t blocks = 1);

    void setBlocks(size_t blocks);

    /** This function takes the relative index of a certain part of the work assigned to this process
        as an argument and returns the absolute index of that part, a value from zero to the total
        amount of parts that need to be executed in the simulation. When \c blocks = 1, this function
        simply returns the argument. For \c blocks > 1, the relative index depends on which blocks were
        assigned to the process that calls it. */
    size_t absoluteIndex(size_t relativeIndex);

    /** This function takes the absolute index of a certain part of the work as an argument and returns
        the relative index of that part, a value from zero to the number of parts that were assigned to
        this process, \c _nvalues. When \c blocks = 1, this function simply returns the argument. For
        \c blocks > 1, the absolute index depends on which blocks were assigned to the process that
        calls it. */
    size_t relativeIndex(size_t absoluteIndex);

    /** This function which must be implemented in each subclass of ProcessAssigner, returns the rank
        assigned to a certain part of the work. In this class however, if \c blocks = 1, each process
        is assigned to each part of work, so the result of this function would have no meaning in that
        case. Therefore, the programmer must ensure that this function is never called for an object of
        the IdenticalAssigner class if the number of blocks is one. If this function does get called in
        that case, a fatal error is thrown. If \c blocks > 1, calling this function does make sence,
        and the assigned process for a part of work with the specified index is determined by which
        block it falls into. */
    int rankForIndex(size_t index) const;

    /** This function returns false if the assignment was invoked with \c blocks = 1, since each
        process was assigned to the same work and so the same output is expected on each process. The
        value of false thus indicates that parallelizing this work amongst different parallel threads
        is not allowed. If the assignment was performed with \c blocks > 1, this function returns true
        since each process was assigned to a different set of blocks. */
    bool parallel() const;

    //======================== Data Members ========================

protected:
    size_t _start;                   // the index of the first value assigned to this process (zero if blocks = 1)
    SequentialAssigner* _blockassigner; // the assigner that is used to assign the different blocks (if multiple are used)
    size_t _nblocks;
};

////////////////////////////////////////////////////////////////////

#endif // IDENTICALASSIGNER_HPP
