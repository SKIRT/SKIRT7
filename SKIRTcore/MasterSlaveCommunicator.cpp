/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "FatalError.hpp"
#include "MasterSlaveCommunicator.hpp"
#include "Parallel.hpp"
#include "ProcessManager.hpp"
#include <QDataStream>
#include <thread>

////////////////////////////////////////////////////////////////////

namespace
{
    // the thread that invoked the first constructor
    std::thread::id _mainThread;
}

////////////////////////////////////////////////////////////////////

MasterSlaveCommunicator::MasterSlaveCommunicator()
    : _acquired(false), _performing(false), _bufsize(4000), _assigner(0)
{
    if (_mainThread == std::thread::id())
    {
        _mainThread = std::this_thread::get_id();
    }
    else
    {
        if (std::this_thread::get_id() != _mainThread)
            throw FATALERROR("Must be invoked from the thread that initialized MasterSlaveCommunicator");
    }

    // Create an assigner that utilizes no specific assignment scheme (assignment of tasks is performed
    // by this class), necessary when the multithreading features of this class are utilized.
    _assigner = new RootAssigner(this);
}

////////////////////////////////////////////////////////////////////

MasterSlaveCommunicator::~MasterSlaveCommunicator()
{
    releaseSlaves();
    foreach (Task* task, _tasks) delete task;
}

////////////////////////////////////////////////////////////////////

void MasterSlaveCommunicator::setLocalSlaveCount(int value)
{
    if (_acquired) throw FATALERROR("Slaves are already acquired");
    _factory.setMaxThreadCount(value);
}

////////////////////////////////////////////////////////////////////

int MasterSlaveCommunicator::localSlaveCount() const
{
    return _factory.maxThreadCount();
}

////////////////////////////////////////////////////////////////////

void MasterSlaveCommunicator::setMaxMessageSize(int value)
{
    if (_acquired) throw FATALERROR("Slaves are already acquired");
    _bufsize = value;
}

////////////////////////////////////////////////////////////////////

int MasterSlaveCommunicator::maxMessageSize() const
{
    return _bufsize;
}

////////////////////////////////////////////////////////////////////

int MasterSlaveCommunicator::taskCount() const
{
    return _tasks.size();
}

////////////////////////////////////////////////////////////////////

int MasterSlaveCommunicator::master() const
{
    return 0;
}

////////////////////////////////////////////////////////////////////

bool MasterSlaveCommunicator::isMaster() const
{
    return !isSlave();
}

////////////////////////////////////////////////////////////////////

bool MasterSlaveCommunicator::isSlave() const
{
    return _performing || (isMultiProc() && rank());
}

////////////////////////////////////////////////////////////////////

int MasterSlaveCommunicator::registerTask(Task* task)
{
    if (_acquired) throw FATALERROR("Slaves are already acquired");
    _tasks << task;
    return _tasks.size()-1;
}

////////////////////////////////////////////////////////////////////

void MasterSlaveCommunicator::acquireSlaves()
{
    if (_acquired) throw FATALERROR("Slaves are already acquired");
    _acquired = true;
    if (isMultiProc() && isSlave())
    {
        slave_obey_loop();
        _acquired = false;
    }
}

////////////////////////////////////////////////////////////////////

void MasterSlaveCommunicator::releaseSlaves()
{
    if (_performing) throw FATALERROR("Still performing tasks");
    if (isMultiProc() && _acquired && isMaster()) stop_obeying();
    _acquired = false;
}

////////////////////////////////////////////////////////////////////

namespace
{
    // simple class to set a boolean flag just for the scope of the SetFlag object
    // (this is safe even when an exception is thrown)
    class SetFlag
    {
    public:
        SetFlag(bool* flag) : _flag(flag) { *_flag = true; }
        ~SetFlag() { *_flag = false; }
    private:
        bool* _flag;
    };
}

////////////////////////////////////////////////////////////////////

namespace
{
    // simple class to serve as a target for local parallel execution
    class LocalTarget : public ParallelTarget
    {
    public:
        LocalTarget(MasterSlaveCommunicator::Task* task, QVector<QVariant> inputVector)
            : _task(task), _inputVector(inputVector), _outputVector(inputVector.size()) { }
        void body(size_t index) { _outputVector[index] = _task->perform(_inputVector[index]); }
        QVector<QVariant> outputVector() { return _outputVector; }
    private:
        MasterSlaveCommunicator::Task* _task;
        QVector<QVariant> _inputVector;
        QVector<QVariant> _outputVector;
    };
}

////////////////////////////////////////////////////////////////////

QVector<QVariant> MasterSlaveCommunicator::performTask(int taskIndex, QVector<QVariant> inputVector)
{
    if (std::this_thread::get_id() != _mainThread)
        throw FATALERROR("Must be invoked from the thread that initialized MasterSlaveCommunicator");
    if (_performing) throw FATALERROR("Already performing tasks");
    if (isSlave()) throw FATALERROR("Only the master can command the slaves");
    if (taskIndex < 0 || taskIndex >= _tasks.size()) throw FATALERROR("Task index out of range");

    // bracket performing tasks with flag to control return value of isMaster() / isSlave()
    SetFlag flag(&_performing);

    if (isMultiProc())
    {
         return master_command_loop(taskIndex, inputVector);
    }
    else
    {
        LocalTarget target(_tasks[taskIndex], inputVector);
        _assigner->assign(inputVector.size());
        _factory.parallel()->call(&target, _assigner);
        return target.outputVector();
    }
}

////////////////////////////////////////////////////////////////////

QVector<QVariant> MasterSlaveCommunicator::performTask(QVector<QVariant> inputVector)
{
    return performTask(0, inputVector);
}

////////////////////////////////////////////////////////////////////

namespace
{
    // serialize a QVariant object into a QByteArray, verifying the maximum length of the result
    QByteArray toByteArray(int maxsize, QVariant variant)
    {
        QByteArray buffer;
        {   // make sure the stream is flushed by forcing it out of scope
            QDataStream stream(&buffer, QIODevice::WriteOnly);
            stream << variant;
        }
        if (buffer.size() > maxsize)
            throw FATALERROR("Message size " + QString::number(buffer.size()) +
                             " larger than maximum size " + QString::number(maxsize));
        return buffer;
    }

    // resurrect a QVariant object from a QByteArray
    QVariant toVariant(QByteArray buffer)
    {
        QDataStream stream(buffer);
        return QVariant(stream);
    }
}

////////////////////////////////////////////////////////////////////

QVector<QVariant> MasterSlaveCommunicator::master_command_loop(int taskIndex, QVector<QVariant> inputVector)
{
    // prepare an output vector of the appropriate size
    int numitems = inputVector.size();
    QVector<QVariant> outputVector(numitems);

    // prepare a vector to remember the index of most recent item handed out to each slave
    QVector<int> itemForSlave(size());

    // the index of the next item to be handed out
    int numsent = 0;

    // hand out an item to each slave (unless there are less items than slaves)
    for (int slave=1; slave<size() && numsent<numitems; slave++,numsent++)
    {
        QByteArray buffer = toByteArray(_bufsize, inputVector[numsent]);

        ProcessManager::sendByteBuffer(buffer, slave, taskIndex);

        itemForSlave[slave] = numsent;
    }

    // receive results, handing out more items until all have been handed out
    QByteArray resultbuffer(_bufsize, 0);
    for (int i=0; i<numitems; i++)
    {
        // receive a message from any slave
        int slave;
        ProcessManager::receiveByteBuffer(resultbuffer, slave);

        // put the result in the output vector
        outputVector[itemForSlave[slave]] = toVariant(resultbuffer);

        // if more items are available, hand one to this slave
        if (numsent<numitems)
        {
            QByteArray buffer = toByteArray(_bufsize, inputVector[numsent]);

            ProcessManager::sendByteBuffer(buffer, slave, taskIndex);

            itemForSlave[slave] = numsent;
            numsent++;
        }
    }
    return outputVector;
}

////////////////////////////////////////////////////////////////////

void MasterSlaveCommunicator::slave_obey_loop()
{
    QByteArray inbuffer(_bufsize, 0);
    while (true)
    {
        // receive the next message from the master
        int tag;
        ProcessManager::receiveByteBuffer(inbuffer, master(), tag);

        // if the message tag specifies a non-existing task, terminate the obey loop
        if (tag < 0 || tag >= _tasks.size()) break;

        // perform the requested task, deserializing and serializing QVariant from/to buffer
        QByteArray outbuffer = toByteArray(_bufsize, _tasks[tag]->perform(toVariant(inbuffer)));

        // send the result back to the master
        ProcessManager::sendByteBuffer(outbuffer, master(), tag);
    }
}

////////////////////////////////////////////////////////////////////

void MasterSlaveCommunicator::stop_obeying()
{
    for (int slave=1; slave<size(); slave++)
    {
        // send an empty message with a tag that specifies a non-existing task
        QByteArray emptybuffer;
        ProcessManager::sendByteBuffer(emptybuffer, slave, _tasks.size());
    }
}

////////////////////////////////////////////////////////////////////
