/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef ARRAYMEMORY_HPP
#define ARRAYMEMORY_HPP

#include <QFile>
#include <QTextStream>

////////////////////////////////////////////////////////////////////

/** The ArrayMemory class is used to write out information about the allocation of memory
    specifically for the Array class. This information is written to a text file much like the
    simulation log file, with a timestamp before each message. */
class ArrayMemory
{
public:
    /* The constructor of this class is explicitly deleted to prevent objects of this
       class from being created. */
    ArrayMemory() = delete;

#ifdef BUILDING_MEMORY
    /** This static function initializes the data members of this class. Most importantly, it opens
        the output file. */
    static void initialize(QString prefix, QString path, double limit);

    /** This static function closes the output file. */
    static void finalize();

    /** This static function writes a log message stating the amount of memory acquired or released
        during a resize operation of an Array instance. */
    static void log_resize(size_t oldsize, size_t newsize, void* ptr);

private:
    /** This private static function returns the full path of an output file with a certain name. The
        simulation prefix is added to this name. */
    static QString outFilePath(QString name);

    /** This private static function logs any message to the output file, prefixed by a timestamp and
        the process rank if the application is run in multiprocessing mode. */
    static void log(QString message);

    /** This private static function writes any message to the output file, if it is opened. */
    static void output(QString message);

    /** This private static function generates a timestamp. */
    static QString timestamp();
#endif
};

#endif // ARRAYMEMORY_HPP
