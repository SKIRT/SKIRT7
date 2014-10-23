/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include <fstream>
#include "FatalError.hpp"
#include "FilePaths.hpp"
#include "Log.hpp"
#include "PegaseSED.hpp"

using namespace std;

//////////////////////////////////////////////////////////////////////

PegaseSED::PegaseSED()
    : _type(E)
{
}

//////////////////////////////////////////////////////////////////////

void PegaseSED::setupSelfBefore()
{
    StellarSED::setupSelfBefore();

    // determine the resource filename based on the spectral type
    QString type;
    switch (_type)
    {
    case E:  type = "E";  break;
    case S0: type = "S0"; break;
    case Sa: type = "Sa"; break;
    case Sb: type = "Sb"; break;
    case Sc: type = "Sc"; break;
    }
    QString filename = FilePaths::resource("SED/Pegase/PegaseSED_" + type + ".dat");

    // Read the data from the file into local vectors lambdav[k] and jv[k]
    const int Nlambda = 1298;
    ifstream file(filename.toLocal8Bit().constData());
    if (! file.is_open()) throw FATALERROR("Could not open the data file " + filename);
    find<Log>()->info("Reading SED data from file " + filename + "...");
    Array lambdav(Nlambda);
    Array jv(Nlambda);
    double lambda, j, dummy;
    for (int k = 0; k<Nlambda; k++)
    {
        file >> lambda >> j >> dummy;
        lambda *= 1e-6; // conversion from micron to m
        lambdav[k] = lambda;
        jv[k] = j;
    }
    file.close();
    find<Log>()->info("File " + filename + " closed.");

    // finish up
    setemissivities(lambdav,jv);
}

//////////////////////////////////////////////////////////////////////

void PegaseSED::setType(PegaseSED::SpectralType value)
{
    _type = value;
}

//////////////////////////////////////////////////////////////////////

PegaseSED::SpectralType PegaseSED::type() const
{
    return _type;
}

//////////////////////////////////////////////////////////////////////
