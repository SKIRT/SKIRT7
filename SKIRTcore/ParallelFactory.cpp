/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "FatalError.hpp"
#include "Parallel.hpp"
#include "ParallelFactory.hpp"

////////////////////////////////////////////////////////////////////

ParallelFactory::ParallelFactory()
{
    // Initialize default maximum number of threads
    _maxThreadCount = defaultThreadCount();

    // Remember the current thread, and provide it with an index
    _parentThread = std::this_thread::get_id();
    addThreadIndex(_parentThread, 0);
}

////////////////////////////////////////////////////////////////////

ParallelFactory::~ParallelFactory()
{
    for (size_t i = 0; i<_children.size(); i++) delete _children[i];
}

////////////////////////////////////////////////////////////////////

void ParallelFactory::setMaxThreadCount(int value)
{
    _maxThreadCount = qMax(1, value);
}

////////////////////////////////////////////////////////////////////

int ParallelFactory::maxThreadCount() const
{
    return _maxThreadCount;
}

////////////////////////////////////////////////////////////////////

int ParallelFactory::defaultThreadCount()
{
    int count = std::thread::hardware_concurrency();
    return count>0 ? count : 1;
}

////////////////////////////////////////////////////////////////////

Parallel* ParallelFactory::parallel(int maxThreadCount)
{
    // Verify that we're being called from our parent thread
    if (std::this_thread::get_id() != _parentThread)
        throw FATALERROR("Parallel not spawned from thread that constructed the factory");

    // Get or create a child with the appropriate number of threads
    int numThreads = maxThreadCount>0 ? qMin(maxThreadCount, _maxThreadCount) : _maxThreadCount;
    auto child = _children[numThreads];
    if (!child)
    {
        child = new Parallel(numThreads, this);
        _children[numThreads] = child;
    }
    return child;
}

////////////////////////////////////////////////////////////////////

int ParallelFactory::currentThreadIndex() const
{
    auto search = _indices.find(std::this_thread::get_id());
    if (search == _indices.end()) throw FATALERROR("Current thread index was not found");
    return search->second;
}

////////////////////////////////////////////////////////////////////

void ParallelFactory::addThreadIndex(std::thread::id threadId, int index)
{
    _indices[threadId] = index;
}

////////////////////////////////////////////////////////////////////
