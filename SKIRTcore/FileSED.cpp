/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include <fstream>
#include "FilePaths.hpp"
#include "FatalError.hpp"
#include "FileSED.hpp"
#include "Log.hpp"
#include "WavelengthGrid.hpp"

using namespace std;

//////////////////////////////////////////////////////////////////////

FileSED::FileSED()
{
}

////////////////////////////////////////////////////////////////////

void FileSED::setupSelfBefore()
{
    StellarSED::setupSelfBefore();

    // read the data from the specified file into local vectors lambdav[k] and jv[k]
    QString filename = find<FilePaths>()->input(_filename);
    ifstream file(filename.toLocal8Bit().constData());
    if (! file.is_open()) throw FATALERROR("Could not open the data file " + filename);
    find<Log>()->info("Reading SED data from file " + filename + "...");

    int Nlambda;
    file >> Nlambda;
    Array lambdav(Nlambda);
    Array jv(Nlambda);
    double lambda;
    for (int k=0; k<Nlambda; k++)
    {
        file >> lambda >> jv[k];
        lambdav[k] = lambda/1e6; // conversion from micron to m
    }
    file.close();
    find<Log>()->info("File " + filename + " closed.");

    // finish up
    setemissivities(lambdav,jv);
}

////////////////////////////////////////////////////////////////////

void FileSED::setFilename(QString value)
{
    _filename = value;
}

////////////////////////////////////////////////////////////////////

QString FileSED::filename() const
{
    return _filename;
}

//////////////////////////////////////////////////////////////////////
