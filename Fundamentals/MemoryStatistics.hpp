/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef MEMORY_STATISTICS_HPP
#define MEMORY_STATISTICS_HPP

#include <QString>

////////////////////////////////////////////////////////////////////

/** This namespace provides functions to obtain the size of the available system memory, and the
    current and peak (maximum so far) memory usage for the current process. The implementation is
    based on code written by <a href="http://NadeauSoftware.com">David Robert Nadeau</a>
    (NadeauSoftware.com) and made available under the <a
    href="http://creativecommons.org/licenses/by/3.0/deed.en_US">Creative Commons Attribution 3.0
    Unported License</a>. */
namespace MemoryStatistics
{
    /** Returns the size of physical memory (RAM) available on the system in bytes. */
    size_t availableMemory();

    /** Returns the peak (maximum so far) resident set size (physical memory use) for the current
        process in bytes, or zero if the value cannot be determined on this OS. */
    size_t peakMemoryUsage();

    /** Returns the current resident set size (physical memory use) for the current process in
        bytes, or zero if the value cannot be determined on this OS. */
    size_t currentMemoryUsage();

    /** Returns a string that reports the amount of available memory in a form ready for human
        consumption. */
    QString reportAvailable(bool showinfo = false);

    /** Returns a string that reports the peak (maximum so far) memory usage in a form ready for
        human consumption. */
    QString reportPeak(bool showinfo = false);

    /** Returns a string that reports the current memory usage in a form ready for human
        consumption. */
    QString reportCurrent(bool showinfo = false);
}

////////////////////////////////////////////////////////////////////

#endif // MEMORY_STATISTICS_HPP
