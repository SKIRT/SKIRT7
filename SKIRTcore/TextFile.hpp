/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef TEXTFILE_HPP
#define TEXTFILE_HPP

#include <fstream>
#include <QString>
#include "Array.hpp"

////////////////////////////////////////////////////////////////////

/** This class represents a writable text file that can be initizialized by providing a filename to
    its constructor. Text is written per line, by calling the writeLine() function. The file is
    automatically closed when the object is destructed. In a multiprocessing environment, only the
    root process will be allowed to write to the specified file; calls to writeLine() performed by
    other processes will have no effect. */
class TextFile
{
    //=============== Construction - Destruction  ==================

public:
    /** The constructor of the TextFile class. As an argument, it takes the filename (a QString
        instance). If the process that invokes it is the root, the output stream for the file is
        initialized. On other processes, this output stream remains uninitialized (and is never used). */
    TextFile(QString filename, bool overwrite = true);

    /** The destructor of the TextFile class. On the root process, the file will be automatically closed. */
    ~TextFile();

    //====================== Other functions =======================

public:
    /** This function is used to write a certain string to the file as a new line. If the calling
        process is not the root, this function will have no effect. */
    void writeLine(QString line);

    //======================== Data Members ========================

private:

    std::ofstream* _out;  // the output stream
};

////////////////////////////////////////////////////////////////////

#endif // TEXTFILE_HPP
