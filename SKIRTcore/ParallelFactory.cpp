/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include "FatalError.hpp"
#include "Parallel.hpp"
#include "ParallelFactory.hpp"

////////////////////////////////////////////////////////////////////

ParallelFactory::ParallelFactory()
{
    // initialize default maximum number of threads
    _maxThreadCount = defaultThreadCount();

    // remember the current thread, and provide it with an index
    _parentThread = QThread::currentThread();
    addThreadIndex(_parentThread, 0);
}

////////////////////////////////////////////////////////////////////

ParallelFactory::~ParallelFactory()
{
    foreach (Parallel* child, _children) delete child;
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
    int count = QThread::idealThreadCount();
    return count>0 ? count : 1;
}

////////////////////////////////////////////////////////////////////

Parallel* ParallelFactory::parallel(int maxThreadCount)
{
    // verify that we're being called from our parent thread
    if (QThread::currentThread() != _parentThread)
        throw FATALERROR("Parallel not spawned from thread that constructed the factory");

    // get or create a child with the appropriate number of threads
    int numThreads = maxThreadCount>0 ? qMin(maxThreadCount, _maxThreadCount) : _maxThreadCount;
    Parallel* child = _children.value(numThreads, 0);
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
    int index = _indices.value(QThread::currentThread(), -1);
    if (index<0) throw FATALERROR("Current thread index was not found");
    return index;
}

////////////////////////////////////////////////////////////////////

void ParallelFactory::addThreadIndex(const QThread* thread, int index)
{
    _indices[thread] = index;
}

////////////////////////////////////////////////////////////////////
