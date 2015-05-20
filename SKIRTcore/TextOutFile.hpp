/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef TEXTOUTFILE_HPP
#define TEXTOUTFILE_HPP

#include <fstream>
#include <QList>
#include <QString>
#include "Array.hpp"
#include "SimulationItem.hpp"
class Log;

////////////////////////////////////////////////////////////////////

/** This class represents a writable text file that can be initizialized by providing a filename to
    its constructor. Text is written per line, by calling the writeLine() function. The file is
    automatically closed when the object is destructed. In a multiprocessing environment, only the
    root process will be allowed to write to the specified file; calls to writeLine() performed by
    other processes will have no effect. */
class TextOutFile
{
    //=============== Construction - Destruction  ==================

public:
    /** The constructor of the TextOutFile class. As an argument, it takes the filename (a QString
        instance). If the process that invokes it is the root, the output stream for the file is
        initialized. On other processes, this output stream remains uninitialized (and is never used). */
    TextOutFile(const SimulationItem* item, QString filename, QString description, bool overwrite = true);

    /** The destructor of the TextOutFile class. On the root process, the file will be automatically
        closed. */
    ~TextOutFile();

    //====================== Other functions =======================

public:
    /** This function is used to write a certain string to the file as a new line. If the calling
        process is not the root, this function will have no effect. */
    void writeLine(QString line);

    /** This function (virtually) adds a new column to the text file, characterized by a certain
        description, the formatting ('d' for integer values, 'e' (or 'E') for scientific notation, 'f'
        for floating point notation and 'g' (or 'G') for the most concise ['e' ('E') or 'f']. For the
        'e', 'E', and 'f' formats, the precision represents the number of digits after the decimal
        point. For the 'g' and 'G' formats, the precision represents the maximum number of significant
        digits (trailing zeroes are omitted). The description of each column is added to the header of
        the text file, along with the column number. */
    void addColumn(QString description, char format = 'e', int precision = 6);

    /** This function writes the specified list of (double) values to the text file, on a single row
        where adjecent values are seperated by a space. The values are formatted according to the
        'format' and 'precision' specified by the addColumn function. If the number of values in the
        list does not match the number of columns, a FatalError is thrown. */
    void writeRow(QList<double> values);

    //======================== Data Members ========================

private:
    Log* _log;
    QString _filepath;
    std::ofstream _out;  // the output stream
    int _ncolumns;
    QList<char> _formats;
    QList<int> _precisions;
};

////////////////////////////////////////////////////////////////////

#endif // TEXTOUTFILE_HPP
