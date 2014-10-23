/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "FatalError.hpp"
#include "StopWatch.hpp"

////////////////////////////////////////////////////////////////////

// ==============================================
// ===== platform-specific code for MacOS X =====
// =====      (nanosecond resolution)       =====
// ==============================================

#ifdef __MACH__
#include <mach/mach_time.h>

namespace
{
    // return the current time in system units
    quint64 current_time()
    {
        return mach_absolute_time();
    }

    // return the conversion factor from system units to seconds
    double conversion_factor()
    {
        mach_timebase_info_data_t info;
        kern_return_t err = mach_timebase_info( &info );
        return err ? 0 : 1e-9 * (double)info.numer / (double)info.denom;
    }
}

#endif // MACH

// ==============================================
// ===== platform-specific code for Linux   =====
// =====      (microsecond resolution)      =====
// ==============================================

// We do not use clock_gettime(), which offers nanosecond resolution, because
// this function often fails to compile and/or link without additional care

#ifdef __linux__
#include <sys/time.h>

namespace
{
    // return the current time in system units
    quint64 current_time()
    {
        timeval time;
        gettimeofday(&time, 0);
        return ((quint64)time.tv_sec) * 1000000 + time.tv_usec;
    }

    // return the conversion factor from system units to seconds
    double conversion_factor()
    {
        return 1e-6;
    }
}

#endif // linux

// ==============================================
// ===== end of platform-specific code      =====
// ==============================================

////////////////////////////////////////////////////////////////////

namespace
{
    const int N = 5;    // the number of timers exposed to the user
    int _level = -1;    // the current level == index in timer arrays

    // these arrays have an element for each timer
    quint64 _count[N] = { 0 };  // total number of calls to stop()
    quint64 _total[N] = { 0 };  // accumulated elapsed time between start/stop (in system units)
    quint64 _start[N] = { 0 };  // absolute time at most recent start (in system units)
}

////////////////////////////////////////////////////////////////////

StopWatch::StopWatch()
{
    start();
}

////////////////////////////////////////////////////////////////////

StopWatch::~StopWatch()
{
    stop();
}

////////////////////////////////////////////////////////////////////

void StopWatch::start()
{
    _level++;
    if (_level >= N) throw FATALERROR("Timer nesting overflow");

    _start[_level] = current_time();
}

////////////////////////////////////////////////////////////////////

void StopWatch::stop()
{
    if (_level < 0) throw FATALERROR("Timer nesting underflow");

    quint64 stop = current_time();
    _count[_level]++;
    _total[_level] += stop - _start[_level];
    _start[_level] = stop;

    _level--;
}

////////////////////////////////////////////////////////////////////

QStringList StopWatch::report()
{
    if (_level != -1) throw FATALERROR("Timer nesting inbalance");
    QStringList result;

    // only produce a non-empty result if at least one timer was actually used
    if (_count[0])
    {
        // get the conversion factor from system units to seconds
        double conv = conversion_factor();

        // add a line per timer that was actually used,
        // and calculate the total and maximum number of start/stop sequences
        quint64 totalcount = 0;
        quint64 maxcount = 0;
        double total0 = conv * _total[0];
        for (int i = 0; i < N; i++)
        {
            if (_count[i])
            {
                double total = conv * _total[i];
                result << QString("Timer %1 :%2 s  %3 %")
                          .arg(i+1) .arg(total, 10,'f',3) .arg(100*total/total0, 5,'f',1);
                totalcount += _count[i];
                maxcount = qMax(maxcount, _count[i]);
            }
        }

        // estimate the start/stop time using the first timer
        const int K = 5000; // number of start/stop sequences in the test
        _total[0] = 0;
        for (int k=0; k<K; k++) { start(); stop(); }
        double startstop = conv * _total[0] / K;

        // add a line with error information
        // estimate the total error: start/stop time accumulates across timers, measurement resolution doesn't
        double error = startstop*totalcount + conv*maxcount;
        result << QString("Error ± :%1 s  %2 %") .arg(error, 10,'f',3) .arg(100*error/total0, 5,'f',1);
    }
    return result;
}

////////////////////////////////////////////////////////////////////
