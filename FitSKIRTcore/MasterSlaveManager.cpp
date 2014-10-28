/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifdef BUILDING_WITH_MPI
#include <mpi.h>
#endif

#include "MasterSlaveManager.hpp"

#include "FatalError.hpp"
#include "Parallel.hpp"
#include <QDataStream>
#include <QThread>

////////////////////////////////////////////////////////////////////

namespace
{
    // the thread that invoked initialize(), or the first constructor
    QThread* _mainThread = 0;
}

////////////////////////////////////////////////////////////////////

void MasterSlaveManager::initialize(int *argc, char ***argv)
{
    _mainThread = QThread::currentThread();

#ifdef BUILDING_WITH_MPI
    MPI_Init(argc, argv);
#else
    Q_UNUSED(argc) Q_UNUSED(argv)
#endif
}

////////////////////////////////////////////////////////////////////

void MasterSlaveManager::finalize()
{
#ifdef BUILDING_WITH_MPI
    MPI_Finalize();
#endif
}

////////////////////////////////////////////////////////////////////

MasterSlaveManager::MasterSlaveManager()
    : _remote(false), _acquired(false), _performing(false),
      _procs(-1), _rank(-1), _bufsize(4000)
{
    if (_mainThread)
    {
        if (QThread::currentThread() != _mainThread)
            throw FATALERROR("Must be invoked from the thread that initialized MasterSlaveManager");
    }
    else
    {
        _mainThread = QThread::currentThread();
    }

#ifdef BUILDING_WITH_MPI
    int initialized;
    MPI_Initialized(&initialized);
    if (initialized)
    {
        MPI_Comm_size(MPI_COMM_WORLD, &_procs);
        if (_procs > 1)
        {
            MPI_Comm_rank(MPI_COMM_WORLD, &_rank);
            _remote = true;
        }
    }
#endif
}

////////////////////////////////////////////////////////////////////

MasterSlaveManager::~MasterSlaveManager()
{
    releaseSlaves();
    foreach (Task* task, _tasks) delete task;
}

////////////////////////////////////////////////////////////////////

void MasterSlaveManager::setLocalSlaveCount(int value)
{
    if (_acquired) throw FATALERROR("Slaves are already acquired");
    _factory.setMaxThreadCount(value);
}

////////////////////////////////////////////////////////////////////

int MasterSlaveManager::localSlaveCount() const
{
    return _factory.maxThreadCount();
}

////////////////////////////////////////////////////////////////////

void MasterSlaveManager::setMaxMessageSize(int value)
{
    if (_acquired) throw FATALERROR("Slaves are already acquired");
    _bufsize = value;
}

////////////////////////////////////////////////////////////////////

int MasterSlaveManager::maxMessageSize() const
{
    return _bufsize;
}

////////////////////////////////////////////////////////////////////

int MasterSlaveManager::taskCount() const
{
    return _tasks.size();
}

////////////////////////////////////////////////////////////////////
bool MasterSlaveManager::isMaster() const
{
    return !isSlave();
}

////////////////////////////////////////////////////////////////////

bool MasterSlaveManager::isSlave() const
{
    return _performing || (_remote && _rank);
}

////////////////////////////////////////////////////////////////////

int MasterSlaveManager::registerTask(Task* task)
{
    if (_acquired) throw FATALERROR("Slaves are already acquired");
    _tasks << task;
    return _tasks.size()-1;
}

////////////////////////////////////////////////////////////////////

void MasterSlaveManager::acquireSlaves()
{
    if (_acquired) throw FATALERROR("Slaves are already acquired");
    _acquired = true;
    if (_remote && isSlave())
    {
        slave_obey_loop();
        _acquired = false;
    }
}

////////////////////////////////////////////////////////////////////

void MasterSlaveManager::releaseSlaves()
{
    if (_performing) throw FATALERROR("Still performing tasks");
    if (_remote && _acquired && isMaster()) stop_obeying();
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
        LocalTarget(MasterSlaveManager::Task* task, QVector<QVariant> inputVector)
            : _task(task), _inputVector(inputVector), _outputVector(inputVector.size()) { }
        void body(size_t index) { _outputVector[index] = _task->perform(_inputVector[index]); }
        QVector<QVariant> outputVector() { return _outputVector; }
    private:
        MasterSlaveManager::Task* _task;
        QVector<QVariant> _inputVector;
        QVector<QVariant> _outputVector;
    };
}

////////////////////////////////////////////////////////////////////

QVector<QVariant> MasterSlaveManager::performTask(int taskIndex, QVector<QVariant> inputVector)
{
    if (QThread::currentThread() != _mainThread)
        throw FATALERROR("Must be invoked from the thread that initialized MasterSlaveManager");
    if (_performing) throw FATALERROR("Already performing tasks");
    if (isSlave()) throw FATALERROR("Only the master can command the slaves");
    if (taskIndex < 0 || taskIndex >= _tasks.size()) throw FATALERROR("Task index out of range");

    // bracket performing tasks with flag to control return value of isMaster() / isSlave()
    SetFlag flag(&_performing);

    if (_remote)
    {
         return master_command_loop(taskIndex, inputVector);
    }
    else
    {
        LocalTarget target(_tasks[taskIndex], inputVector);
        _factory.parallel()->call(&target, inputVector.size());
        return target.outputVector();
    }
}

////////////////////////////////////////////////////////////////////

QVector<QVariant> MasterSlaveManager::performTask(QVector<QVariant> inputVector)
{
    return performTask(0, inputVector);
}

////////////////////////////////////////////////////////////////////

#ifdef BUILDING_WITH_MPI
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
#endif

////////////////////////////////////////////////////////////////////

QVector<QVariant> MasterSlaveManager::master_command_loop(int taskIndex, QVector<QVariant> inputVector)
{
#ifdef BUILDING_WITH_MPI
    // prepare an output vector of the appropriate size
    int numitems = inputVector.size();
    QVector<QVariant> outputVector(numitems);

    // prepare a vector to remember the index of most recent item handed out to each slave
    QVector<int> itemForSlave(_procs);

    // the index of the next item to be handed out
    int numsent = 0;

    // hand out an item to each slave (unless there are less items than slaves)
    for (int slave=1; slave<_procs && numsent<numitems; slave++,numsent++)
    {
        QByteArray buffer = toByteArray(_bufsize, inputVector[numsent]);
        MPI_Send(buffer.data(), buffer.size(), MPI_BYTE, slave, taskIndex, MPI_COMM_WORLD);
        itemForSlave[slave] = numsent;
    }

    // receive results, handing out more items until all have been handed out
    QByteArray resultbuffer(_bufsize, 0);
    for (int i=0; i<numitems; i++)
    {
        // receive a message from any slave
        MPI_Status status;
        MPI_Recv(resultbuffer.data(), _bufsize, MPI_BYTE, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        int slave = status.MPI_SOURCE;

        // put the result in the output vector
        outputVector[itemForSlave[slave]] = toVariant(resultbuffer);

        // if more items are available, hand one to this slave
        if (numsent<numitems)
        {
            QByteArray buffer = toByteArray(_bufsize, inputVector[numsent]);
            MPI_Send(buffer.data(), buffer.size(), MPI_BYTE, slave, taskIndex, MPI_COMM_WORLD);
            itemForSlave[slave] = numsent;
            numsent++;
        }
    }
    return outputVector;
#else
    Q_UNUSED(taskIndex)
    return inputVector;
#endif
}

////////////////////////////////////////////////////////////////////

void MasterSlaveManager::slave_obey_loop()
{
#ifdef BUILDING_WITH_MPI
    QByteArray inbuffer(_bufsize, 0);
    while (true)
    {
        // receive the next message from the master
        MPI_Status status;
        MPI_Recv(inbuffer.data(), _bufsize, MPI_BYTE, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

        // if the message tag specifies a non-existing task, terminate the obey loop
        if (status.MPI_TAG < 0 || status.MPI_TAG >= _tasks.size()) break;

        // perform the requested task, deserializing and serializing QVariant from/to buffer
        QByteArray outbuffer = toByteArray(_bufsize, _tasks[status.MPI_TAG]->perform(toVariant(inbuffer)));

        // send the result back to the master
        MPI_Send(outbuffer.data(), outbuffer.size(), MPI_BYTE, 0, status.MPI_TAG, MPI_COMM_WORLD);
    }
#endif
}

////////////////////////////////////////////////////////////////////

void MasterSlaveManager::stop_obeying()
{
#ifdef BUILDING_WITH_MPI
    for (int slave=1; slave<_procs; slave++)
    {
        // send an empty message with a tag that specifies a non-existing task
        MPI_Send(0, 0, MPI_BYTE, slave, _tasks.size(), MPI_COMM_WORLD);
    }
#endif
}

////////////////////////////////////////////////////////////////////
