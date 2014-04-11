/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef INSTRUMENTSYSTEM_HPP
#define INSTRUMENTSYSTEM_HPP

#include <vector>
#include <QMutex>
#include <QPair>
#include "SimulationItem.hpp"
class Instrument;
class ParallelFactory;

//////////////////////////////////////////////////////////////////////

/** An InstrumentSystem instance keeps a list of zero or more instruments. The instruments can be
    of various nature (e.g. photometric, spectroscopic,...) and do not need to be located at the
    same observing position. */
class InstrumentSystem : public SimulationItem
{
    Q_OBJECT
    Q_CLASSINFO("Title", "an instrument system")

    Q_CLASSINFO("Property", "instruments")
    Q_CLASSINFO("Title", "the instruments")
    Q_CLASSINFO("Optional", "true")
    Q_CLASSINFO("Default", "SimpleInstrument")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor; creates an empty instrument system. */
    Q_INVOKABLE InstrumentSystem();

    /** This function performs setup for the instrument system. */
    void setupSelfBefore();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** This function adds an instrument to the instrument system. */
    Q_INVOKABLE void addInstrument(Instrument* value);

    /** This function returns the list of instruments in the instrument system. */
    Q_INVOKABLE QList<Instrument*> instruments() const;

    //======================== Other Functions =======================

public:
    /** This function adds the specified value to the variable at the specified address in some
        instrument's data cube. It provides the appropriate locking for the instrument's data
        structures, assuming that each instrument maintains a separate data structure for each
        individual wavelength, so that locking is needed only if multiple parallel threads carry
        photon packages of the same wavelength. To reduce the number of lock acquisitions, a number
        of record requests are bunched up (in thread-specific queues) before being executed
        inside a single lock. This mechanism drastically improves performance on systems (such as
        Ubuntu) where acquiring a lock seems to be very expensive if there are many threads. */
    void record(double* address, double value);

    /** This function writes down the results of the instrument system. It flushes any record
        requests still left in the queues maintained by record(), and then calls the write()
        function for each of the instruments. */
    void write();

    //======================== Data Members ========================

private:
    // discoverable attributes
    QList<Instrument*> _instruments;

private:
    // other data members, related to parallelism
    bool _parallelPhotons;    // true if photon packages of the same wavelength are launched in parallel
    ParallelFactory* _parfac; // the ParallelFactory instance associated with this simulation hierarchy
    QMutex _mutex;            // lock for target data structure when photon packages are parallelized
    typedef QPair<double*, double> Record;  // data type for a single record request (target address, value)
    std::vector< std::vector<Record> > _queues;  // data record queues, one per thread
};

////////////////////////////////////////////////////////////////////

#endif // INSTRUMENTSYSTEM_HPP
