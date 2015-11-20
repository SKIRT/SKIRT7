/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef MEMORYLOGGER_HPP
#define MEMORYLOGGER_HPP

#include <cstddef>

////////////////////////////////////////////////////////////////////

/** MemoryLogger is a pure interface to communicate between the Array class and a FileLog instance.
    This interface provides a function for writing out information about the allocation and
    deallocation of memory by an Array instance. In the implementation, this information is written
    to the simulation's log file(s). */
class MemoryLogger
{
protected:
    /* The empty constructor for the interface. */
    MemoryLogger() {}

public:
    /** The empty destructor for the interface. */
    virtual ~MemoryLogger() {}

    /** This function writes a log message stating the amount of memory acquired or released during
        a resize operation of an Array instance. */
    virtual void memory(size_t oldsize, size_t newsize, void *ptr) = 0;
};

////////////////////////////////////////////////////////////////////

#endif // MEMORYLOGGER_HPP
