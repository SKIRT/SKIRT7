/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef LOG_HPP
#define LOG_HPP

#include "SimulationItem.hpp"

////////////////////////////////////////////////////////////////////

/** Log is the abstract base class for a message logging mechanism. It offers convenience functions
    for logging messages at various levels (info, warning, success, error), adding a time-stamp
    along the way. All of these functions eventually call a single pure virtual function, which
    must be implemented in a subclass to actually output the message to a device such as the
    console or a file. */
class Log : public SimulationItem
{
    Q_OBJECT

    //============= Construction - Setup - Destruction =============

protected:
    /** The default constructor; it is protected since this is an abstract class. */
    Log();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** This enum includes a constant for each logging level, in increasing order of importance. */
    enum Level { Info, Warning, Success, Error };

    /** Sets the lowest logging level that actually gets written to the output device. In other
        words, any messages logged at a lower level are ignored. The default value is Info, so that
        all messages are logged. */
    void setLowestLevel(Level level);

    /** Returns the lowest logging level that actually gets written to the output device. */
    Level lowestLevel() const;

    /** Sets the Log instance that is linked into this one. All messages received by this Log
        instance are also sent to the linked Log instance, regardless of the logging level set for
        this instance (i.e. each Log instance in the chain decides for itself which messages to
        display). The receiving Log instance assumes ownership of the linked Log instance. */
    void setLinkedLog(Log* log);

    /** Returns the Log instance that is linked into this one, so that it receives a copy of all
        messages. */
    Log* linkedLog() const;


    /** This function .. */
    void setProcessName(int rank);

    //======================== Other Functions =======================

public:
    /** Logs an informational message (i.e. at level Info). */
    void info(QString message);

    /** Logs a warning message (i.e. at level Warning). */
    void warning(QString message);

    /** Logs an informational message (i.e. at level Success). */
    void success(QString message);

    /** Logs an informational message (i.e. at level Error). */
    void error(QString message);

protected:
    /** This pure virtual function must be implemented in a subclass to actually output the
        specified message to a device such as the console or a file. The message already contains a
        time stamp and the level is guaranteed to be at or above the current lowest level. The
        second argument specifies the logging level for the message (info, warning, success,
        error). */
    virtual void output(QString message, Level level) = 0;

    /** This static function returns a formatted timestamp string. */
    static QString timestamp();

    //======================== Data Members ========================

private:
    Level _lowestLevel;
    Log* _link;
    QString _procName;
};

////////////////////////////////////////////////////////////////////

#endif // LOG_HPP
