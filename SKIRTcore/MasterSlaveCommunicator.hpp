/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef MASTERSLAVECOMMUNICATOR_HPP
#define MASTERSLAVECOMMUNICATOR_HPP

#include <QVariant>
#include <QVector>
#include "ParallelFactory.hpp"
#include "ProcessCommunicator.hpp"

class MasterSlaveCommunicator : public ProcessCommunicator
{
    Q_OBJECT

    //============= Construction - Setup - Destruction =============

public:
    /** Constructs a MasterSlaveManager object with the local slave count set to the number of
        logical cores detected on the computer running the code. */
    MasterSlaveCommunicator();

    /** Releases the slaves, if applicable, and destructs the object. */
    ~MasterSlaveCommunicator();

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
    bool _acquired;             // true if slaves are acquired
    bool _performing;           // true if we're performing a set of tasks
    ParallelFactory _factory;   // the factory used to spawn objects for local parallellization
    QList<Task*> _tasks;        // registered tasks, in index order
    int _bufsize;               // the maximum message size, in bytes (for remote mode)
};

////////////////////////////////////////////////////////////////////

// registerTask() template function implementation
template<class T> int MasterSlaveCommunicator::registerTask(T* targetObject, QVariant (T::*targetMember)(QVariant input))
{
    MemberTask<T>* task = new MemberTask<T>(targetObject, targetMember);
    return registerTask(task);  // the newly created task object is now owned by the MasterSlaveManager
}

////////////////////////////////////////////////////////////////////

#endif // MASTERSLAVECOMMUNICATOR_HPP
