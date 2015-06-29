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
    // Remember the ID of the current thread
    _parentThread = std::this_thread::get_id();
    factory->addThreadIndex(_parentThread, 0);

    // Initialize the number of active threads (i.e. not waiting for new work) other than the current thread
    _terminate = false;
    _active = threadCount - 1;

    // Create the extra parallel threads
    for (int index = 1; index < threadCount; index++)
    {
        _threads.push_back(std::thread(&Parallel::run, this));
        factory->addThreadIndex(_threads.back().get_id(), index);
    }

    // Wait until all parallel threads are ready
    waitForThreads();
}

////////////////////////////////////////////////////////////////////

Parallel::~Parallel()
{
    // Verify that none of the threads are active
    if (_active) throw FATALERROR("Parallel threads still active upon destruction");

    // Ask the parallel threads to exit
    _terminate = true;
    _condition_extra.notify_all();

    // Wait for them to do so
    for (auto& thread: _threads) thread.join();
}

////////////////////////////////////////////////////////////////////

int Parallel::threadCount() const
{
    return _threads.size() + 1;  // also count the parent thread
}

////////////////////////////////////////////////////////////////////

void Parallel::call(ParallelTarget* target, ProcessAssigner* assigner)
{
    // Verify that we're being called from our parent thread
    if (std::this_thread::get_id() != _parentThread)
        throw FATALERROR("Parallel call not invoked from thread that constructed this object");

    // Copy the arguments so they can be used from any of the threads
    _target = target;
    _assigner = assigner;
    _limit = _assigner->nvalues();

    // Initialize the number of active threads (i.e. not waiting for new work)
    _active = assigner->parallel() ? _threads.size() : 0;

    // Clear the exception pointer
    _exception = 0;

    // Initialize the loop variable
    _next = 0;

    // Wake all parallel threads, if multithreading is allowed
    if (assigner->parallel()) _condition_extra.notify_all();

    // Do some work ourselves as well
    doWork();

    // Wait until all parallel threads are done
    if (assigner->parallel()) waitForThreads();

    // Check for and process the exception, if any
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
        // Wait for new work in a critical section
        std::unique_lock<std::mutex> lock(_mutex);
        _active--;                                  // indicate that this thread is not longer doing work
        if (!_active) _condition_main.notify_all(); // tell the main thread that all parallel threads may be waiting
        _condition_extra.wait(lock);
        lock.unlock();

        // Check for termination request (don't bother decrementing _active; it's no longer used)
        if (_terminate) break;

        // Do work as long as some is available
        doWork();
    }
}

////////////////////////////////////////////////////////////////////

void Parallel::doWork()
{
    try
    {
        // Do work as long as some is available
        forever
        {
            size_t index = _next++;                  // get the next index atomically
            if (index >= _limit) break;              // break if no more are available

            // Execute the body
            _target->body(_assigner->absoluteIndex(index));
        }
    }
    catch (FatalError& error)
    {
        // Make a copy of the exception
        reportException(new FatalError(error));
    }
    catch (...)
    {
        // Create a fresh exception
        reportException(new FATALERROR("Unhandled exception (not of type FatalError) in a parallel thread"));
    }
}

////////////////////////////////////////////////////////////////////

void Parallel::reportException(FatalError* exception)
{
    // Need to lock, in case multiple threads throw simultaneously
    std::unique_lock<std::mutex> lock(_mutex);
    if (!_exception)  // only store the first exception thrown
    {
        _exception = exception;

        // Make the other threads stop by taking away their work
        _limit = 0;  // this is safe because another thread will see either the old value, or zero
    }
    lock.unlock();
}

////////////////////////////////////////////////////////////////////

void Parallel::waitForThreads()
{
    // Wait until all parallel threads are in wait
    std::unique_lock<std::mutex> lock(_mutex);
    forever
    {
        if (_active) _condition_main.wait(lock);
        if (!_active) break;
    }
    // lock is automatically released
}

////////////////////////////////////////////////////////////////////
