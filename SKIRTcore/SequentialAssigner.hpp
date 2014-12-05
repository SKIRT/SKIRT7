/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef SEQUENTIALASSIGNER_HPP
#define SEQUENTIALASSIGNER_HPP

#include "ProcessAssigner.hpp"

//////////////////////////////////////////////////////////////////////

/** The SequentialAssigner class is a subclass of the ProcessAssigner class, representing objects
    that assign work to different processes. The SequentialAssigner does this by dividing the work
    (consisting of many parts) in sequential blocks, where each block contains more or less the
    same number of parts. Then, each process is assigned to a different block, according to their
    rank. If a certain method in another class incorporates an object of the SequentialAssigner
    class for performing a set of tasks (or parts of work), each process will execute a different
    subset of these tasks. After performing this work in parallel, communication is typically
    needed to accumulate the results stored at different processes.

    The assignment mechanism explained above is represented graphically in the following figure.

    \image html sequentialassigner.png "The SequentialAssigner class assigns processes to a subset of the work." */
class SequentialAssigner : public ProcessAssigner
{
    Q_OBJECT
    Q_CLASSINFO("Title", "an assigner that assigns each process sequentially to a subset of the work")

    //============= Construction - Setup - Destruction =============

public:
    /** Default constructor. */
    Q_INVOKABLE SequentialAssigner();

    //======================== Other Functions =======================

public:
    /** This function invokes the assignment procedure. As an argument, it takes the number of parts of
        work \f$n\f$ that need to be performed. Based on this number, the number of processes and the
        rank of this process, this function determines the number of tasks that are assigned to this
        process and stores it in the _nvalues member. The algorithm of this function goes as follows.
        First, a pointer to the PeerToPeerCommunicator object is obtained and the rank \f$i\f$ and size
        \f$N_{P}\f$ are acquired and stored in temporary variables. Then, the quotient and the
        remainder of the integer division of \f$n\f$ and \f$N_{P}\f$ are calculated: \f[ q = \left
        \lfloor{\frac{n}{N_{P}}}\right \rfloor \f] \f[ r = n \bmod{N_{P}} \f] These two values are
        respectively stored in the private _quotient and _remainder members, for use of the
        rankForIndex function. Next, based on \f$q\f$ and \f$r\f$, the number of values assigned to the
        process is determined. This is done based on the following simple principle: <ol> <li> First,
        hand out \f$q\f$ values to each process. <li> Then, give the first \f$r\f$ process one value
        extra. </ol> This principle is illustrated in the following figure.
        \image html sequentialassigner_tasks.png "An illustration of how the number of tasks assigned to each process is determined."
        With the above method, all \f$n\f$ values get assigned to a process, and the work
        load is maximally balanced (the difference in number of tasks between two arbitrary processes
        is no more than one). The number of values (or tasks) as calculated based on the method
        described above, is stored in the _nvalues member. The last thing the assign function
        calculates is the starting value for the particular process. This is done by differentiating
        between two seperate cases: <ul> <li> Either the rank \f$i\f$ of the process is smaller than
        \f$r\f$, <li> or \f$i\f$ is greater than or equal to \f$r\f$. </ul> In the former case, the
        starting index is given by: \f[ t_0 = i \cdot ( q + 1 ) \f] In the latter case, this index is
        given by: \f[ t_0 = r \cdot ( q + 1) + (i - r) \cdot q \f] The resulting value is stored in
        the private _start member of this class, for use in the absoluteIndex function. */
    void assign(size_t size);

    /** This function takes the relative index of a certain part of the work assigned to this process
        as an argument and returns the absolute index of that part, a value from zero to the total
        amount of parts that need to be executed in the simulation. This is done by adding the relative
        index to the index of the starting value for this process, stored in the _start member. */
    size_t absoluteIndex(size_t relativeIndex);

    /** This function returns the rank of the process that is assigned to a certain part of the work.
        This part is identified by its absolute index, passed to this function as an argument. The
        algorithm of this function differentiates between two cases: <ol> <li> The index \f$t\f$ is
        smaller than \f$r \cdot (q + 1) \f$, <li> or \f$i\f$ is greater than or equal to \f$r \cdot (q
        + 1) \f$. </ol> In the former case, the rank \f$j\f$ is found by the following formula: \f[ j =
        \frac{t}{q + 1} \f] In the latter case, the rank is determined by: \f[ j = r + \frac{t^{*}}{q}
        \f] where \f$ t^{*} = t - r \cdot (q + 1) \f$ */
    int rankForIndex(size_t index) const;

    /** This function returns true if the different parts of work are distributed amongst the different
        processes and returns false if each process is assigned to the same work. In this class, the
        processes are assigned to different work so this function returns true. */
    bool parallel() const;

    //======================== Data Members ========================

protected:
    size_t _start;          // the index of the first value assigned to this process
    size_t _quotient;       // the quotient
    size_t _remainder;      // the remainder
};

////////////////////////////////////////////////////////////////////

#endif // SEQUENTIALASSIGNER_HPP
