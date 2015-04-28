/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "FatalError.hpp"
#include "TextFile.hpp"
#include "ProcessManager.hpp"

using namespace std;

////////////////////////////////////////////////////////////////////

TextFile::TextFile(QString filename)
{
    // Initialize the output file, if this is the root process
    if (ProcessManager::isRoot())
    {
        _out = ofstream(filename.toLocal8Bit().constData());
    }
}

////////////////////////////////////////////////////////////////////

TextFile::~TextFile()
{
    // Close the output file, if it was opened by this process
    if (ProcessManager::isRoot())
    {
        _out.close();
    }
}

////////////////////////////////////////////////////////////////////

void TextFile::writeLine(QString line)
{
    // Only the root process can write to file
    if (!ProcessManager::isRoot()) return;

    _out << line.toStdString() << "\n";
}

////////////////////////////////////////////////////////////////////
