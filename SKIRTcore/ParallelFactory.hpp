/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef PARALLELFACTORY_HPP
#define PARALLELFACTORY_HPP

#include <QHash>
#include <QThread>
#include "SimulationItem.hpp"
class Parallel;

/** A ParallelFactory object serves as a factory for instances of the Parallel class, called its
    children. An important attribute of a factory object is the maximum number of parallel
    execution threads to be handed to its children. When requesting a new child, the client can
    specify a more stringent limit on the number of threads, but the factory's limit is never
    exceeded. This allows a client to request a Parallel object with a smaller number of threads,
    usually for performance reasons, while still sharing the thread indexing mechanism provided by
    the currentThreadIndex() function. A factory object assumes ownership for all its children. If
    a child with the appropriate number of threads already exists, it will be handed out again.

    A particular Parallel instance can be reused many times for calling various member functions in
    various objects, reducing the overhead of creating and destroying the threads. However all
    children of a particular factory share the same thread pool (at least logically if not
    physically), so they should \em never be used in parallel. Specifically, recursively invoking
    the call() function on the same Parallel instance is not allowed and results in undefined
    behavior. The recommended use is to have a single ParallelFactory instance per simulation, and
    to use yet another ParallelFactory instance to run multiple simulations at the same time. */
class ParallelFactory : public SimulationItem
{
    Q_OBJECT
    friend class Parallel;

    //============= Construction - Setup - Destruction =============

public:
    /** Constructs a ParallelFactory object with a maximum thread count determined as the return value of
        defaultThreadCount(). */
    ParallelFactory();

    /** Destructs the factory object and its children. */
    ~ParallelFactory();

    //====================== Other Functions =======================

public:
    /** Sets the maximum number of threads to be handed out to Parallel objects manufactured by
        this factory object. The minimum value is 1 thread. */
    void setMaxThreadCount(int value);

    /** Returns the maximum number of threads to be handed out to Parallel objects manufactured by
        this factory object. */
    int maxThreadCount() const;

    /** Returns the number of logical cores detected on the computer running the code. */
    static int defaultThreadCount();

    /** Returns a Parallel instance with a particular number of execution threads. If the argument
        is zero or omitted, the number of threads equals the factory maximum. If the argument is
        nonzero, the number of threads is the smaller of the factory maximum and the specified
        maximum. */
    Parallel* parallel(int maxThreadCount=0);

    /** Returns an index for the parallel thread from which this function is called. When invoked
        from within a loop body being iterated by one of the factory's Parallel children, the
        function returns an index from zero to the number of threads in the Parallel instance minus
        one. When invoked from a thread that does not belong to any of the factory's children, the
        function throws a fatal error. */
    int currentThreadIndex() const;

private:
    /** Adds a dictionary item linking the specified thread to a particular index. This is a
        private function used from the Parallel() constructor to provide the information required
        by the currentThreadIndex() function. */
    void addThreadIndex(const QThread* thread, int index);

    //======================== Data Members ========================

private:
    int _maxThreadCount;                // the maximum thread count for the factory
    const QThread* _parentThread;       // the thread that invoked our constructor
    QHash<int, Parallel*> _children;    // our children, keyed on number of threads
    QHash<const QThread*,int> _indices; // the index for each thread, including parent, for all our children
};

#endif // PARALLELFACTORY_HPP
