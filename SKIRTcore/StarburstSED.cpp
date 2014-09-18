/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include <cmath>
#include <fstream>
#include "FatalError.hpp"
#include "FilePaths.hpp"
#include "Log.hpp"
#include "NR.hpp"
#include "StarburstSED.hpp"

using namespace std;

//////////////////////////////////////////////////////////////////////

StarburstSED::StarburstSED()
    : _Z(0)
{
}

void StarburstSED::setupSelfBefore()
{
    StellarSED::setupSelfBefore();

    // open the resource file and skip the header
    QString filename = FilePaths::resource("SED/Starburst/StarburstSED.dat");
    ifstream file(filename.toLocal8Bit().constData());
    if (! file.is_open()) throw FATALERROR("Could not open the data file " + filename);
    find<Log>()->info("Reading SED data from file " + filename + "...");
    string line;
    for (int i=0; i<6; i++) getline(file,line);

    // get the metallicity range and the number of wavelengths
    int NZ, Nlambda;
    file >> NZ >> Nlambda;
    Array Zv(NZ);
    for (int l=0; l<NZ; l++) file >> Zv[l];
    int lL = NR::locate_fail(Zv,_Z);
    if (lL < 0) throw FATALERROR("The metallicity Z should be between "
                                 + QString::number(Zv[0]) + " and " + QString::number(Zv[NZ-1]));
    double ZL = Zv[lL];
    double ZR = Zv[lL+1];

    // read the data from the file into local vectors
    Array lambdav(Nlambda);
    Array jv(Nlambda);
    Array logjLv(Nlambda);
    Array logjRv(Nlambda);
    double lambda, dummy;
    for (int k=0; k<Nlambda; k++)
    {
        file >> lambda;
        for (int l=0; l<lL; l++) file >> dummy;
        file >> logjLv[k] >> logjRv[k];
        for (int l=lL+2; l<NZ; l++) file >> dummy;
        lambdav[k] = lambda/1e10; // conversion from A to m
    }
    file.close();
    find<Log>()->info("File " + filename + " closed.");

    // interpolate linearly in log space
    for (int k=0; k<Nlambda; k++)
        jv[k] = pow(10., NR::interpolate_linlin(_Z, ZL, ZR, logjLv[k], logjRv[k]));

    // finish up
    setemissivities(lambdav,jv);
}

////////////////////////////////////////////////////////////////////

void StarburstSED::setMetallicity(double value)
{
    _Z = value;
}

////////////////////////////////////////////////////////////////////

double StarburstSED::metallicity() const
{
    return _Z;
}

//////////////////////////////////////////////////////////////////////
