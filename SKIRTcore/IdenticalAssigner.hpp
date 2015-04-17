/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef IDENTICALASSIGNER_HPP
#define IDENTICALASSIGNER_HPP

#include "ProcessAssigner.hpp"

//////////////////////////////////////////////////////////////////////

/** The IdenticalAssigner class is a subclass of the ProcessAssigner class, representing objects
    that assign work to different processes. The IdenticalAssigner class is used to assign each
    process to the same work. When a certain method in another class requires the execution of
    different small parts of work, and it uses an object of this class as the process assigner,
    then all processes will execute all these parts. The execution of these parts is thus not
    parallelized with regard to the different processes, although each process can still execute
    these parts in parallel threads. Since each process executes the same algorithm, no
    communication is needed afterwards to accumulate the results.

    The assignment mechanism explained above is represented graphically in the following figure.

    \image html identicalassigner.png "The IdenticalAssigner class assigns each process to the same work." */
class IdenticalAssigner : public ProcessAssigner
{
    Q_OBJECT
    Q_CLASSINFO("Title", "an assigner that assigns each process to the same work")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE IdenticalAssigner();

    //======================== Other Functions =======================

public:
    /** This function invokes the assignment procedure. As an argument, it takes the number of parts of
        work that need to be performed. In this class, the assignment is straightforward and does not
        require any calculations. Therefore, this function only copies the value of its argument into
        the _nvalues member. */
    void assign(size_t size);

    /** This function takes the relative index of a certain part of the work assigned to this process
        as an argument and returns the absolute index of that part, a value from zero to the total
        amount of parts that need to be executed in the simulation. Since this class assigns all
        processes to all of the work, this function just returns the argument. */
    size_t absoluteIndex(size_t relativeIndex);

    /** This function takes the absolute index of a certain part of the work as an argument and returns
        the relative index of that part, a value from zero to the number of parts that were assigned to
        this process, _nvalues. Since this class assigns all processes to all of the work, this
        function just returns the argument. */
    size_t relativeIndex(size_t absoluteIndex);

    /** This function which must be implemented in each subclass of ProcessAssigner, returns the rank
        assigned to a certain part of the work. In this class however, each process is assigned to each
        part of work, so this function has no purpose and even has no meaning. Therefore, the
        programmer must ensure that this function is never called for an object of the
        IdenticalAssigner class. If this function does get invoked, a fatal error is thrown. The
        programmer can identify objects of this class by calling the parallel function, since only this
        class will return false. */
    int rankForIndex(size_t index) const;

    /** In this class, this function returns false since by assigning each process to the same work,
        this work is not executed in parallel. */
    bool parallel() const;
};

////////////////////////////////////////////////////////////////////

#endif // IDENTICALASSIGNER_HPP
