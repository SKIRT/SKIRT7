/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "FatalError.hpp"
#include "TextOutFile.hpp"
#include "ProcessManager.hpp"

using namespace std;

////////////////////////////////////////////////////////////////////

TextOutFile::TextOutFile(QString filename, bool overwrite)
    : _out(0)
{
    // Initialize the output file, if this is the root process
    if (ProcessManager::isRoot())
    {
        _out = new ofstream(filename.toLocal8Bit().constData(), overwrite ? ios_base::out : ios_base::app);
    }
}

////////////////////////////////////////////////////////////////////

TextOutFile::~TextOutFile()
{
    // Close the output file, if it was opened by this process
    if (_out)
    {
        _out->close();
        delete _out;
    }
}

////////////////////////////////////////////////////////////////////

void TextOutFile::writeLine(QString line)
{
    // Only the root process can write to file
    if (!ProcessManager::isRoot()) return;

    (*_out) << line.toStdString() << endl;
}

////////////////////////////////////////////////////////////////////
