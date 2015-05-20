/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "FatalError.hpp"
#include "Parallel.hpp"
#include "ParallelFactory.hpp"

////////////////////////////////////////////////////////////////////

Parallel::Parallel(int threadCount, ParallelFactory* factory)
    : _assigner(0)
{
    // remember the current thread
    _parentThread = QThread::currentThread();
    factory->addThreadIndex(_parentThread, 0);

    // initialize the number of active threads (i.e. not waiting for new work) other than the current thread
    _terminate = false;
    _active = threadCount - 1;

    // create and start the extra parallel threads
    for (int index=1; index<threadCount; index++)
    {
        Thread* thread = new Thread(this);
        thread->start();
        _threads << thread;
        factory->addThreadIndex(thread, index);
    }

    // wait until all parallel threads are ready
    waitForThreads();
}

////////////////////////////////////////////////////////////////////

Parallel::~Parallel()
{
    // verify that none of the threads are active
    if (_active) throw FATALERROR("Parallel threads still active upon destruction");

    // ask the parallel threads to exit
    _terminate = true;
    _waitExtra.wakeAll();

    // wait for them to do so and then delete them
    foreach (Thread* thread, _threads)
    {
        thread->wait();
        delete thread;
    }
}

////////////////////////////////////////////////////////////////////

int Parallel::threadCount() const
{
    return _threads.size() + 1;  // also count the parent thread
}

////////////////////////////////////////////////////////////////////

void Parallel::call(ParallelTarget* target, ProcessAssigner* assigner)
{
    // verify that we're being called from our parent thread
    if (QThread::currentThread() != _parentThread)
        throw FATALERROR("Parallel call not invoked from thread that constructed this object");

    // copy the arguments so they can be used from any of the threads
    _target = target;
    _assigner = assigner;
    _limit = _assigner->nvalues();

    // initialize the number of active threads (i.e. not waiting for new work)
    _active = _threads.size();

    // clear the exception pointer
    _exception = 0;

    // initialize the loop variable
    _next = 0;

    // wake all parallel threads, if multithreading is allowed
    if (assigner->parallel()) _waitExtra.wakeAll();

    // do some work ourselves as well
    doWork();

    // wait until all parallel threads are done
    if (assigner->parallel()) waitForThreads();

    // check for and process the exception, if any
    if (_exception)
    {
        throw *_exception;  // throw by value
        delete _exception;  // destroy the heap-allocated copy
    }
}

////////////////////////////////////////////////////////////////////

void Parallel::run()
{
    forever
    {
        // wait for new work in a critical section
        _mutex.lock();
        _active--;                         // indicate that this thread is not longer doing work
        if (!_active) _waitMain.wakeAll(); // tell the main thread that all parallel threads may be waiting
        _waitExtra.wait(&_mutex);
        _mutex.unlock();

        // check for termination request (don't bother decrementing _active; it's no longer used)
        if (_terminate) break;

        // do work as long as some is available
        doWork();
    }
}

////////////////////////////////////////////////////////////////////

void Parallel::doWork()
{
    try
    {
        // do work as long as some is available
        forever
        {
            size_t index = _next++;                  // get the next index atomically
            if (index >= _limit) break;              // break if no more are available

            // execute the body
            _target->body(_assigner->absoluteIndex(index));
        }
    }
    catch (FatalError& error)
    {
        // make a copy of the exception
        reportException(new FatalError(error));
    }
    catch (...)
    {
        // create a fresh exception
        reportException(new FATALERROR("Unhandled exception (not of type FatalError) in a parallel thread"));
    }
}

////////////////////////////////////////////////////////////////////

void Parallel::reportException(FatalError* exception)
{
    // need to lock, in case multiple threads throw simultaneously
    _mutex.lock();
    if (!_exception)  // only store the first exception thrown
    {
        _exception = exception;

        // make the other threads stop by taking away their work
        _limit = 0;  // this is safe because another thread will see either the old value, or zero
    }
    _mutex.unlock();
}

////////////////////////////////////////////////////////////////////

void Parallel::waitForThreads()
{
    // wait until all parallel threads are in wait
    _mutex.lock();
    forever
    {
        if (_active) _waitMain.wait(&_mutex);
        if (!_active) break;
    }
    _mutex.unlock();
}

////////////////////////////////////////////////////////////////////
