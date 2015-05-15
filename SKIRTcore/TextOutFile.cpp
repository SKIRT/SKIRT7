/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "TextOutFile.hpp"
#include "ProcessManager.hpp"

using namespace std;

////////////////////////////////////////////////////////////////////

TextOutFile::TextOutFile(QString filename, bool overwrite)
{
    // Only open the output file if this is the root process
    if (ProcessManager::isRoot())
    {
        _out.open(filename.toLocal8Bit().constData(), overwrite ? ios_base::out : ios_base::app);
    }
}

////////////////////////////////////////////////////////////////////

TextOutFile::~TextOutFile()
{
    // Close the output file, if it was opened by this process
    if (_out.is_open())
    {
        _out.close();
    }
}

////////////////////////////////////////////////////////////////////

void TextOutFile::writeLine(QString line)
{
    if (_out.is_open())
    {
        _out << line.toStdString() << endl;
    }
}

////////////////////////////////////////////////////////////////////
