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

    /** The purpose of this function is setting the _procName attribute, using the rank of the process.
        This rank is obtained by using the find algorithm to search for an instance of a
        ProcessCommunicator subclass. If none is found, because for example the simulation or fitscheme
        hierarchy has not yet been created, the function returns and _procName retains its empty state.
        If the find algorithm does succeed, the find algorithm is applied again, this time to invoke
        the setup of the ProcessCommunicator object, if this is not yet done. Then, the rank is
        obtained from the ProcessCommunicator and passed as an argument to the setProcessName()
        function. */
    void setupSelfBefore();

private:
    /** This private function sets the process name used for logging based on the process rank
        passed as an argument to this function. This function makes sure that the same procedure is
        also applied for the linked Log instance, if present. */
    void setRank(int rank);

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

    /** Sets or unsets the verbose mode for this Log instance. */
    void setVerbose(bool value);

    /** Returns whether the Log is set in verbose mode or not. */
    bool verbose() const;

    /** Sets or unsets the memory logging mode for this Log instance. */
    void setMemoryLogging(bool value);

    /** Returns whether memory usage is logged or not. */
    bool memoryLogging() const;

    //======================== Other Functions =======================

public:
    /** Logs an informational message (i.e. at level Info). In multiprocessing mode, only the info messages of the root processes are actually logged. When compiled in debug mode,
        all processes log and the process name is attached to the info message. */
    void info(QString message);

    /** Logs a warning message (i.e. at level Warning). Each warning message is logged, irrespective of the which process invokes this function. The warning message is
        prefixed with the process name, if multiprocessing mode is used. */
    void warning(QString message);

    /** Logs an informational message (i.e. at level Success). In multiprocessing mode, only the success messages of the root processes are actually logged. When compiled in debug mode,
        all processes log and the process name is attached to the success message. */
    void success(QString message);

    /** Logs an informational message (i.e. at level Error). Each error message is logged, irrespective of which process invokes this function. The error message is
        prefixed with the process name, if multiprocessing mode is used. */
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

    /** This function returns a string identifying this process of the form "Pnnn",
        where nnn is the rank of the process. In singleprocessing mode, this string is empty. */
    QString processName();

    //======================== Data Members ========================

private:
    Level _lowestLevel;
    Log* _link;
    bool _verbose;
    bool _logmemory;
    QString _procNameShort;
    QString _procNameLong;
};

////////////////////////////////////////////////////////////////////

#endif // LOG_HPP
