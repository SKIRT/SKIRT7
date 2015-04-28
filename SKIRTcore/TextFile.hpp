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

/** This class... */
class TextFile
{
public:
    /** Constructor. */
    TextFile(QString filepath);

    /** Destructor */
    ~TextFile();

    /** This function .. */
    void writeLine(QString line);

private:
    std::ofstream _out;
};

////////////////////////////////////////////////////////////////////

#endif // TEXTFILE_HPP
