/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include <sstream>
#include "FatalError.hpp"
#include "FilePaths.hpp"
#include "Log.hpp"
#include "TextInFile.hpp"

using namespace std;

////////////////////////////////////////////////////////////////////

TextInFile::TextInFile(const SimulationItem* item, QString filename, QString description)
{
    // open the file and log a message
    QString filepath = item->find<FilePaths>()->input(filename);
    _in.open(filepath.toLocal8Bit().constData());
    if (!_in.is_open()) throw FATALERROR("Could not open the " + description + " data file " + filepath);
    item->find<Log>()->info("Reading " + description + " from file " + filepath + "...");
}

////////////////////////////////////////////////////////////////////

bool TextInFile::readRow(Array& values, size_t ncols, size_t noptcols)
{
    // read new line until it is non-empty and non-comment
    string line;
    while (_in.good())
    {
        getline(_in,line);
        auto pos = line.find_first_not_of(" \t");
        if (pos!=string::npos && line[pos]!='#')
        {
            // resize and clear result array
            values.resize(ncols);

            // convert values from line and store them in result array
            stringstream linestream(line);
            for (size_t i=0; i<ncols; ++i)
            {
                if (linestream.eof())
                {
                    if (i<ncols-noptcols) throw FATALERROR("One or more required value(s) on text line are missing");
                    break;
                }
                double value;
                linestream >> value;
                if (linestream.fail()) throw FATALERROR("Input text is not formatted as a floating point number");
                values[i] = value;
            }
            return true;
        }
    }

    // end of file was reached
    return false;
}

////////////////////////////////////////////////////////////////////
