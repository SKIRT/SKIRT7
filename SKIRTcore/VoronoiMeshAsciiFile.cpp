/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include <QFile>
#include "VoronoiMeshAsciiFile.hpp"
#include "FatalError.hpp"
#include "FilePaths.hpp"
#include "Log.hpp"

////////////////////////////////////////////////////////////////////

VoronoiMeshAsciiFile::VoronoiMeshAsciiFile()
    : _coordinateUnits(0)
{
}

//////////////////////////////////////////////////////////////////////

void VoronoiMeshAsciiFile::setupSelfBefore()
{
    VoronoiMeshFile::setupSelfBefore();

    // verify property values
    if (_coordinateUnits <= 0) throw FATALERROR("Coordinate units should be positive");
}

//////////////////////////////////////////////////////////////////////

void VoronoiMeshAsciiFile::setCoordinateUnits(double value)
{
    _coordinateUnits = value;
}

//////////////////////////////////////////////////////////////////////

double VoronoiMeshAsciiFile::coordinateUnits() const
{
    return _coordinateUnits;
}

//////////////////////////////////////////////////////////////////////

void VoronoiMeshAsciiFile::open()
{
    // open the data file
    QString filepath = find<FilePaths>()->input(_filename);
    _infile.setFileName(filepath);
    if (!_infile.open(QIODevice::ReadOnly|QIODevice::Text))
        throw FATALERROR("Could not open the Voronoi mesh data file " + filepath);
    find<Log>()->info("Reading Voronoi mesh data from ASCII file " + filepath + "...");
    _columns.clear();
}

//////////////////////////////////////////////////////////////////////

void VoronoiMeshAsciiFile::close()
{
    _infile.close();
    _columns.clear();
}

//////////////////////////////////////////////////////////////////////

bool VoronoiMeshAsciiFile::read()
{
    // read the next line, splitting it in columns, and skip empty and comment lines
    while (!_infile.atEnd())
    {
        _columns = _infile.readLine().simplified().split(' ');
        if (!_columns[0].isEmpty() && !_columns[0].startsWith('#'))
        {
            return true;
        }
    }
    _columns.clear();
    return false;
}

//////////////////////////////////////////////////////////////////////

Vec VoronoiMeshAsciiFile::particle() const
{
    // verify index range
    if (_columns.size() < 3) throw FATALERROR("Insufficient number of particle coordinates in Voronoi mesh data");

    // get the coordinate values
    bool okx, oky, okz;
    double x = _columns.value(0).toDouble(&okx);
    double y = _columns.value(1).toDouble(&oky);
    double z = _columns.value(2).toDouble(&okz);
    if (!okx || !oky || !okz) throw FATALERROR("Invalid particle coordinate(s) in Voronoi mesh data");

    // convert to SI units
    return Vec(x*_coordinateUnits, y*_coordinateUnits, z*_coordinateUnits);
}

//////////////////////////////////////////////////////////////////////

double VoronoiMeshAsciiFile::value(int g) const
{
    // verify index range
    if (g < 0) throw FATALERROR("Field index out of range");
    if (g+3 >= _columns.size()) throw FATALERROR("Insufficient number of field values in Voronoi mesh data");

    // get the appropriate column value
    bool ok;
    double value = _columns.value(g+3).toDouble(&ok);
    if (!ok) throw FATALERROR("Invalid field value in Voronoi mesh data");
    return value;
}

//////////////////////////////////////////////////////////////////////
