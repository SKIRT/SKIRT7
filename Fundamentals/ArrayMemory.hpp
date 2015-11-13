/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef ARRAYMEMORY_HPP
#define ARRAYMEMORY_HPP

#include <QFile>
#include <QTextStream>

////////////////////////////////////////////////////////////////////

/** The ArrayMemory class ... */
class ArrayMemory
{
public:
    /* The constructor of this class is explicitly deleted to prevent objects of this
        class from being created. */
    ArrayMemory() = delete;

#ifdef BUILDING_MEMORY
    /** This static function initializes ... */
    static void initialize(QString prefix, QString path);

    /** This static function finalizes ... */
    static void finalize();

    /** This static function ... */
    static void log_resize(size_t oldsize, size_t newsize);

private:
    /** This private static function ... */
    static QString outFilePath(QString name);

    /** This private static function ... */
    static void log(QString message);

    /** This private static function ... */
    static void output(QString message);

    /** This private static function ... */
    static QString timestamp();
#endif

private:
    // Output prefix and path
    static QString _outputPath;
    static QString _outputPrefix;

    // Strings identifying the process
    static QString _procNameShort;
    static QString _procNameLong;

    // Objects used for writing to file
    static QFile _file;
    static QTextStream _out;
};

#endif // ARRAYMEMORY_HPP
