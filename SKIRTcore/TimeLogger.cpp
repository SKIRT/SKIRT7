/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include <exception>
#include "Log.hpp"
#include "TimeLogger.hpp"

////////////////////////////////////////////////////////////////////

TimeLogger::TimeLogger(Log *log, QString scope)
    :_log(log), _scope(scope), _started(QDateTime::currentDateTime())
{
    if (log) log->info("Starting " + scope + "...");
}

////////////////////////////////////////////////////////////////////

TimeLogger::~TimeLogger()
{
    // If no Log instance was passed, we don't have to calculate the elapsed time
    if (!_log) return;

    // if this destructor is executed while an execption is unwinding the stack,
    // then we shouldn't report a success message!
    if (std::uncaught_exception()) return;

    // local constants
    const qint64 msecsInSecond = 1000;
    const qint64 msecsInMinute = 60 * msecsInSecond;
    const qint64 msecsInHour   = 60 * msecsInMinute;
    const qint64 msecsInDay    = 24 * msecsInHour;

    // get the elapsed time in milliseconds
    qint64 msecs = _started.msecsTo(QDateTime::currentDateTime());
    bool moreThanMinute = msecs >= msecsInMinute;

    // always include the elapsed time in seconds
    QString elapsed = QString::number(double(msecs)/double(msecsInSecond), 'f', moreThanMinute ? 0 : 1) + " s";

    // if the elapsed time is over a minute, also include a "0d 0h 0m 0s" format
    if (moreThanMinute)
    {
        qint64 days = msecs/msecsInDay;
        msecs -= days*msecsInDay;
        qint64 hours = msecs/msecsInHour;
        msecs -= hours*msecsInHour;
        qint64 minutes = msecs/msecsInMinute;
        msecs -= minutes*msecsInMinute;
        qint64 seconds = (msecs + msecsInSecond/2) /msecsInSecond;

        elapsed += " (";
        if (days) elapsed += QString::number(days) + "d ";
        if (days || hours) elapsed += QString::number(hours) + "h ";
        elapsed += QString::number(minutes) + "m ";
        elapsed += QString::number(seconds) + "s)";
    }

    _log->success("Finished " + _scope + " in " + elapsed + ".");
}

////////////////////////////////////////////////////////////////////
