/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include <fstream>
#include "FatalError.hpp"
#include "FilePaths.hpp"
#include "FileWavelengthGrid.hpp"
#include "Log.hpp"
#include "NR.hpp"

using namespace std;

////////////////////////////////////////////////////////////////////

FileWavelengthGrid::FileWavelengthGrid()
{
}

////////////////////////////////////////////////////////////////////

void FileWavelengthGrid::setupSelfBefore()
{
    PanWavelengthGrid::setupSelfBefore();

    QString filename = find<FilePaths>()->input(_filename);
    ifstream file(filename.toLocal8Bit().constData());
    if (! file.is_open()) throw FATALERROR("Could not open the data file " + filename);
    find<Log>()->info("Reading wavelength grid data from file " + filename + "...");

    file >> _Nlambda;
    _lambdav.resize(_Nlambda);
    for (int k=0; k<_Nlambda; k++)
    {
        file >> _lambdav[k];
        _lambdav[k] /= 1e6;   // conversion from micron to m
    }
    file.close();
    find<Log>()->info("File " + filename + " closed.");

    NR::sort(_lambdav);
}

//////////////////////////////////////////////////////////////////////

void FileWavelengthGrid::setFilename(QString value)
{
    _filename = value;
}

//////////////////////////////////////////////////////////////////////

QString FileWavelengthGrid::filename() const
{
    return _filename;
}

//////////////////////////////////////////////////////////////////////
