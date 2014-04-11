/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include <QFile>
#include "AdaptiveMeshAsciiFile.hpp"
#include "FatalError.hpp"
#include "FilePaths.hpp"
#include "Log.hpp"

////////////////////////////////////////////////////////////////////

AdaptiveMeshAsciiFile::AdaptiveMeshAsciiFile()
{
}

//////////////////////////////////////////////////////////////////////

void AdaptiveMeshAsciiFile::open()
{
    // open the data file
    QString filepath = find<FilePaths>()->input(_filename);
    _infile.setFileName(filepath);
    if (!_infile.open(QIODevice::ReadOnly|QIODevice::Text))
        throw FATALERROR("Could not open the adaptive mesh data file " + filepath);
    find<Log>()->info("Reading adaptive mesh data from ASCII file " + filepath + "...");
    _columns.clear();
    _isNonLeaf = false;
}

//////////////////////////////////////////////////////////////////////

void AdaptiveMeshAsciiFile::close()
{
    _infile.close();
    _columns.clear();
    _isNonLeaf = false;
}

//////////////////////////////////////////////////////////////////////

bool AdaptiveMeshAsciiFile::read()
{
    // read the next line, splitting it in columns, and skip empty and comment lines
    while (!_infile.atEnd())
    {
        _columns = _infile.readLine().simplified().split(' ');
        if (!_columns[0].isEmpty() && !_columns[0].startsWith('#'))
        {
            // remember the node type
            _isNonLeaf = _columns[0].startsWith('!');
            if (_isNonLeaf)
            {
                // remove the exclamation mark
                if (_columns[0].size() > 1) _columns[0].remove(0,1);
                else _columns.removeFirst();
            }
            return true;
        }
    }
    _columns.clear();
    _isNonLeaf = false;
    return false;
}

//////////////////////////////////////////////////////////////////////

bool AdaptiveMeshAsciiFile::isNonLeaf() const
{
    return _isNonLeaf;
}

//////////////////////////////////////////////////////////////////////

void AdaptiveMeshAsciiFile::numChildNodes(int &nx, int &ny, int &nz) const
{
    // get the column values; missing or illegal values default to zero
    nx = _columns.value(0).toInt();
    ny = _columns.value(1).toInt();
    nz = _columns.value(2).toInt();

    // we expect three positive integers
    if (nx<1 || ny<1 || nz<1) throw FATALERROR("Invalid nonleaf line in mesh data");
}

//////////////////////////////////////////////////////////////////////

double AdaptiveMeshAsciiFile::value(int g) const
{
    // verify index range
    if (g < 0) throw FATALERROR("Field index out of range");
    if (g >= _columns.size()) throw FATALERROR("Insufficient number of field values in mesh data");

    // get the appropriate column value
    bool ok;
    double value = _columns.value(g).toDouble(&ok);
    if (!ok) throw FATALERROR("Invalid leaf line in mesh data");
    return value;
}

//////////////////////////////////////////////////////////////////////
