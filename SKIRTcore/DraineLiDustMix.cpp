/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include <fstream>
#include "DraineLiDustMix.hpp"
#include "FatalError.hpp"
#include "FilePaths.hpp"
#include "Log.hpp"
#include "Units.hpp"

using namespace std;

//////////////////////////////////////////////////////////////////////

DraineLiDustMix::DraineLiDustMix()
{
}

//////////////////////////////////////////////////////////////////////

void DraineLiDustMix::setupSelfBefore()
{
    DustMix::setupSelfBefore();

    // create temporary vectors with the appropriate size
    const int Nlambda = 800;
    Array lambdav(Nlambda);
    Array sigmaabsv(Nlambda);
    Array sigmascav(Nlambda);
    Array asymmparv(Nlambda);

    // read the raw data from the resource file into the temporary vectors
    QString filename = FilePaths::resource("DustMix/DraineLiDustMix.dat");
    ifstream file(filename.toLocal8Bit().constData());
    if (!file.is_open()) throw FATALERROR("Could not open the data file " + filename);
    find<Log>()->info("Reading dust mix properties from file " + filename + "...");
    string line;
    while (file.peek() == '#') getline(file,line);
    double lambda, sigmaabs, sigmasca, em, albedo, asymmpar;
    for (int k=0; k<Nlambda; k++)
    {
        file >> lambda >> sigmaabs >> sigmasca >> em >> albedo >> asymmpar;
        lambdav[k] = lambda * 1e-6;     // lambda in file is in micron, we need lambda in m
        sigmaabsv[k] = sigmaabs * 1e-4; // sigmaabs in file is in cm^2 H^-1, we need sigmaabs in m^2 H^-1
        sigmascav[k] = sigmasca * 1e-4; // sigmasca in file is in cm^2 H^-1, we need sigmasca in m^2 H^-1
        asymmparv[k] = asymmpar;
    }
    file.close();
    find<Log>()->info("File " + filename + " closed.");

    // calculate the dust mass per H nucleon (cross sections in file are also per nucleon)
    const double MdustoverMH = 5.4e-4 + 5.4e-4 + 1.8e-4 + 2.33e-3 + 8.27e-3;
    const double MdustoverH = MdustoverMH * Units::massproton();

    // add a dust population with these properties
    addpopulation(MdustoverH, lambdav,sigmaabsv,sigmascav,asymmparv);
}

//////////////////////////////////////////////////////////////////////
