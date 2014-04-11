/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef MASTERSLAVEMANAGER_HPP
#define MASTERSLAVEMANAGER_HPP

#include <QVariant>
#include <QVector>
#include "ParallelFactory.hpp"
#include "SimulationItem.hpp"

/** A MasterSlaveManager object serves to perform a set of similar tasks in parallel in a local
    (shared memory) or remote (distributed memory) context through a "single master - multiple
    slaves" paradigm.

    <B>Usage Example</B>

    Here is a complete example of using this class:

\code
class Compute
{
public:
    Compute(int size, double factor)
        : _size(size), _factor(factor)
    {
        _mgr.setLocalSlaveCount(4);
        _mgr.registerTask(this, &Compute::doSingle);
    }
    void setup()
    {
        _mgr.acquireSlaves();
    }
    ~Compute()
    {
        _mgr.releaseSlaves();
    }
    void doIt()
    {
        if (_mgr.isMaster())
        {
            QVector<QVariant> data(_size);
            for (int i=0; i<_size; i++) data[i] = (double)i;
            data = _mgr.performTask(data);
            for (int i=0; i<_size; i++) std::cout << data[i].toDouble() << " ";
            std::cout << std::endl;
        }
    }
private:
    QVariant doSingle(QVariant input) { return input.toDouble()*_factor; }
    int _size;
    double _factor;
    MasterSlaveManager _mgr;
};

int main(int argc, char** argv)
{
    MasterSlaveManager::initialize(&argc, &argv);
    {
        Compute c(7, 2.);
        c.setup();
        c.doIt();
    }
    {
        Compute c(11, 0.5);
        c.setup();
        c.doIt();
    }
    MasterSlaveManager::finalize();
}
\endcode

    When executed this example will print:

\verbatim
0 2 4 6 8 10 12
0 0.5 1 1.5 2 2.5 3 3.5 4 4.5 5
\endverbatim

    The constructor of the Compute class initializes a MasterSlaveManager instance and registers
    one of its own methods as a task to be performed by one or more slaves in parallel. The setup()
    function prepares the slaves to receive work. This is split off from the constructor to ensure
    that the Compute object has been fully constructed before calling the acquireSlaves() function,
    which doesn't return immediately on remote slaves. The doIt() function does work only if it is
    running from the master; in that case it prepares some data, commands the slaves to perform
    parallel work on each of the data elements, and finally prints the results.

    The main() function constructs two different Compute objects and performs the corresponding
    calculations. Any context created before calling the acquireSlaves() function (such as the
    multiplication factor in the example) can be used in the slave computations, even if they are
    performed remotely. This is possible because each remote slave also executes all code until the
    acquireSlaves() function is called. For a remote slave, this function starts a communication
    loop that exits when the master calls the releaseSlaves() function. In fact, the two code
    segments are each surrounded by curly braces so that the Compute object would go out of scope
    and execute its destructor, releasing the slaves.

    <B>Parallel Modes</B>

    In local mode the class uses a Parallel object spawned from a privately owned ParallelFactory
    instance. In remote mode the implementation relies on MPI (Message Passing Interface) for
    communication between master and slaves. The implementation automatically selects remote mode
    if the code was compiled and built with MPI support, and if the code is executed under MPI
    control on more than a single node. In all other cases local mode is used instead.

    <B>Passing Data to and from the slaves</B>

    Except for any context established before calling the acquireSlaves() function, all information
    passed from the master to a slave and back must be serialized to a flat byte stream (at least
    in remote mode). This is accomplished through the use of QVariant objects, which can hold
    values of variable type from scalars up to complex data structures, and which can be
    serialized using standard Qt functionality. Refer to the Qt documentation for more info.

    <B>Thread safety (or lack thereof)</B>

    With the exception of isMaster() and isSlave(), all MasterSlaveManager functions (including
    static initialization and instance construction) must be invoked from the very same thread.
    Usually this will be the main program thread. In some key places, a fatal error is thrown if
    this restriction is violated.
*/
class MasterSlaveManager : public SimulationItem
{
    Q_OBJECT

    //============= Construction - Setup - Destruction =============

public:
    /** This static function initializes the MPI library used for remote communication, if present.
        The function must be called exactly once at the very start of the program. It is passed a
        reference to the command line arguments to provide the MPI library with the opportunity to
        use and/or adjust them (e.g. to remove any MPI related arguments). */
    static void initialize(int *argc, char ***argv);

    /** This static function finalizes the MPI library used for remote communication, if present.
        The function must be called exactly once at the very end of the program. */
    static void finalize();

    /** Constructs a MasterSlaveManager object with the local slave count set to the number of
        logical cores detected on the computer running the code. */
    MasterSlaveManager();

    /** Releases the slaves, if applicable, and destructs the object. */
    ~MasterSlaveManager();

    //====================== Other Functions =======================

public:
    /** Sets the number of slaves to be used when operating in local mode; this number is ignored when
        operating in remote mode. Throws a fatal error if called while slaves are acquired. */
    void setLocalSlaveCount(int value);

    /** Returns the number of slaves to be used when operating in local mode. */
    int localSlaveCount() const;

    /** Sets the maximum size in bytes of a message exchanged between master and slave when
        operating in remote mode. This number is ignored when operating in local mode. The number
        must be large enough to accomodate the serialized form of any of the QVariant objects
        passed to or returned from the registerTask() function. The default value is 4000 bytes,
        which is sufficient in most cases. */
    void setMaxMessageSize(int value);

    /** Returns the maximum size in bytes of a message exchanged between master and slave when
        operating in remote mode. */
    int maxMessageSize() const;

    /** Returns true if the caller is the master. */
    bool isMaster() const;

    /** Returns true if the caller is a slave. */
    bool isSlave() const;

    /** Registers the specified member function for the specified target object as a task. Task
        indices are assigned in increasing order starting from zero. Throws a fatal error if called
        while slaves are acquired. */
    template<class T> int registerTask(T* targetObject, QVariant (T::*targetMember)(QVariant input));

    /** Retruns number of tasks. */
    int taskCount() const;

    /** Ensures that master and slaves are ready to command and perform tasks. In remote mode, the
        slaves block until releaseSlaves() is called. Throws a fatal error if called while slaves
        are already acquired. */
    void acquireSlaves();

    /** Release the slaves, if applicable. Does nothing if the slaves are not acquired, or if
        called from a slave. Throws a fatal error if tasks are still being performed. */
    void releaseSlaves();

    /** Make the slaves perform the task with specified index on each of the data items in the
        specified vector (in parallel). The results are returned in a vector with the same size as
        the input vector. Throws a fatal error if called while slaves are not acquired, if called
        from a slave, or if the task index is out of range. */
    QVector<QVariant> performTask(int taskIndex, QVector<QVariant> inputVector);

    /** Make the slaves perform the task with index zero on each of the data items in the
        specified vector. Invokes the general performTask() function with a task index of zero. */
    QVector<QVariant> performTask(QVector<QVariant> data);

    //======================== Nested Classes =======================

public:
    /** The declaration for this pure interface is nested in the MasterSlaveManager class
        declaration. It is an abstract base class for objects that serve as a task in the
        registerTask() function. */
    class Task
    {
    public:
        /** The empty constructor for the interface. */
        Task() { }

        /** The empty constructor for the interface. */
        virtual ~Task() { }

        /** The function that will be invoked by the MasterSlaveManager class to perform a task (in
            parallel with other similar tasks). This function must be implemented in the derived
            class. */
        virtual QVariant perform(QVariant input) = 0;
    };

private:
    /** The declaration for this template class is nested in the MasterSlaveManager class
        declaration. It is used in the implementation of the registerTask() template function to
        allow specifying a task in the form of an arbitrary target member function and target
        object. An instance of this target-type-dependent class bundles the relevant information
        into a single object with a type-independent base class (Task) so that it can be passed to
        the private non-template registerTask() function. */
    template<class T> class MemberTask : public Task
    {
    public:
        /** Constructs a MemberTask instance with a perform() function that calls the specified
            target member function on the specified target object. */
        MemberTask(T* targetObject, QVariant (T::*targetMember)(QVariant input))
            : _targetObject(targetObject), _targetMember(targetMember) { }

        /** Calls the target member function on the target object specified in the constructor. */
        QVariant perform(QVariant input) { return (_targetObject->*(_targetMember))(input); }

    private:
        T* _targetObject;
        QVariant (T::*_targetMember)(QVariant input);
    };

    //============= Private Functions using Nested Classes =========

private:
    /** Registers the specified task and returns the assigned task index. */
    int registerTask(Task* task);

    //============= Private Functions for Remote Operation =========

    /** Implements the command loop for a remote master. */
    QVector<QVariant> master_command_loop(int taskIndex, QVector<QVariant> inputVector);

    /** Implements the obey loop for a remote slave. */
    void slave_obey_loop();

    /** Makes the remote slaves exit their obey loop. */
    void stop_obeying();

    //======================== Data Members ========================

private:
    bool _remote;               // true if operating in remote MPI mode
    bool _acquired;             // true if slaves are acquired
    bool _performing;           // true if we're performing a set of tasks
    ParallelFactory _factory;   // the factory used to spawn objects for local parallellization
    QList<Task*> _tasks;        // registered tasks, in index order
    int _procs;                 // the nr of processes in the communication group (for remote mode)
    int _rank;                  // the rank of this process in the communication group (for remote mode)
    int _bufsize;               // the maximum message size, in bytes (for remote mode)
};

////////////////////////////////////////////////////////////////////

// registerTask() template function implementation
template<class T> int MasterSlaveManager::registerTask(T* targetObject, QVariant (T::*targetMember)(QVariant input))
{
    MemberTask<T>* task = new MemberTask<T>(targetObject, targetMember);
    return registerTask(task);  // the newly created task object is now owned by the MasterSlaveManager
}

////////////////////////////////////////////////////////////////////

#endif // MASTERSLAVEMANAGER_HPP
