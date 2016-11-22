/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include <fstream>
#include <vector>
#include "FatalError.hpp"
#include "FilePaths.hpp"
#include "FileMesh.hpp"
#include "Log.hpp"
#include "NR.hpp"

using namespace std;

////////////////////////////////////////////////////////////////////

FileMesh::FileMesh()
{
}

#include <iostream>

////////////////////////////////////////////////////////////////////

void FileMesh::setupSelfBefore()
{
    MoveableMesh::setupSelfBefore();

    // read the points from the file
    QString filename = find<FilePaths>()->input(_filename);
    ifstream file(filename.toLocal8Bit().constData());
    if (! file.is_open()) throw FATALERROR("Could not open the data file " + filename);
    find<Log>()->info("Reading mesh data from file " + filename + "...");
    vector<double> points;
    while (true)
    {
        double point;
        file >> point;
        if (!file) break;
        points.push_back(point);
    }
    file.close();
    find<Log>()->info("File " + filename + " closed.");

    // insert zero point if needed and check basic requirements
    NR::sort(points);
    if (points.size() < 1) throw FATALERROR("The mesh data file has no points: " + filename);
    if (points.front() < 0.) throw FATALERROR("The mesh data file has negative points: " + filename);
    if (points.front() != 0.) points.insert(points.begin(), 0.);
    if (points.size() < 2 || points.back() == 0.)
        throw FATALERROR("The mesh data file has no positive points" + filename);

    // assign and scale mesh points
    NR::assign(_mesh, points);
    _mesh /= points.back();

    // set correct number of bins
    setNumBins(_mesh.size()-1);
}

//////////////////////////////////////////////////////////////////////

void FileMesh::setFilename(QString value)
{
    _filename = value;
}

//////////////////////////////////////////////////////////////////////

QString FileMesh::filename() const
{
    return _filename;
}

//////////////////////////////////////////////////////////////////////

Array FileMesh::mesh() const
{
    return _mesh;
}

//////////////////////////////////////////////////////////////////////
