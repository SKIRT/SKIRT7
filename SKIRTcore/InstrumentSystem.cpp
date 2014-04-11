/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include "FatalError.hpp"
#include "Instrument.hpp"
#include "InstrumentSystem.hpp"
#include "MonteCarloSimulation.hpp"
#include "ParallelFactory.hpp"

using namespace std;

////////////////////////////////////////////////////////////////////

// the length of the record queues (guestimate)
#define QUEUE_LEN 10000

//////////////////////////////////////////////////////////////////////

InstrumentSystem::InstrumentSystem()
    : _parallelPhotons(false), _parfac(0)
{
}

////////////////////////////////////////////////////////////////////

void InstrumentSystem::setupSelfBefore()
{
    SimulationItem::setupSelfBefore();

    // if we need locking (and thus queuing), setup the appropriate data members
    if (find<MonteCarloSimulation>()->parallelPhotonsOfSameWaveLength()
        && find<ParallelFactory>()->maxThreadCount() > 1)
    {
        _parallelPhotons = true;
        _parfac = find<ParallelFactory>();
        int maxThreadCount = _parfac->maxThreadCount();
        _queues.resize(maxThreadCount);
        for (int i = 0; i < maxThreadCount; i++) _queues[i].reserve(QUEUE_LEN);
    }
}

//////////////////////////////////////////////////////////////////////

void InstrumentSystem::addInstrument(Instrument* value)
{
    if (!value) throw FATALERROR("Instrument pointer shouldn't be null");
    value->setParent(this);
    _instruments << value;
}

//////////////////////////////////////////////////////////////////////

QList<Instrument*> InstrumentSystem::instruments() const
{
    return _instruments;
}

//////////////////////////////////////////////////////////////////////

void InstrumentSystem::record(double* address, double value)
{
    // if photons of the same wavelength are being launched in parallel,
    // then we need to update the instrument data structures inside a critical section;
    // we bunch up record requests to reduce the number of lock acquisitions
    if (_parallelPhotons)
    {
        // put the record request in the queue for this thread
        vector<Record>& queue = _queues[_parfac->currentThreadIndex()];
        queue.push_back(qMakePair(address,value));

        // if the queue is full, detect and delete all the packages in the queue
        if (queue.size() == QUEUE_LEN)
        {
            _mutex.lock();
            for (int k = 0; k < QUEUE_LEN; k++) *(queue[k].first) += queue[k].second;
            _mutex.unlock();
            queue.clear();
        }
    }
    // if there is no need for locking, simply perform the record request
    else
    {
        *address += value;
    }
}

//////////////////////////////////////////////////////////////////////

void InstrumentSystem::write()
{
    // if photons of the same wavelength are being launched in parallel,
    // we need to flush all the queues used to bunch up the record requests;
    // since this is executed in the main thread, there is no need for locking
    if (_parallelPhotons)
    {
        int numQueues = _queues.size();
        for (int i = 0; i < numQueues; i++)
        {
            vector<Record>& queue = _queues[i];
            int numRecords = queue.size();
            for (int k = 0; k < numRecords; k++) *(queue[k].first) += queue[k].second;
        }
        _queues.clear();
    }

    // perform the actual write
    foreach (Instrument* instrument, _instruments) instrument->write();
}

//////////////////////////////////////////////////////////////////////
