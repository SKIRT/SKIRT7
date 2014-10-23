/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include <fstream>
#include "Benchmark2DDustMix.hpp"
#include "FatalError.hpp"
#include "FilePaths.hpp"
#include "Log.hpp"
#include "NR.hpp"
#include "Units.hpp"

using namespace std;

//////////////////////////////////////////////////////////////////////

Benchmark2DDustMix::Benchmark2DDustMix()
{
}

//////////////////////////////////////////////////////////////////////

void Benchmark2DDustMix::setupSelfBefore()
{
    DustMix::setupSelfBefore();

    // create temporary vectors with the appropriate size
    const int Nlambda = 61;
    Array lambdav(Nlambda);
    Array Cabsv(Nlambda);
    Array Cscav(Nlambda);
    Array asymmparv(Nlambda);

    // read the raw data from the resource file into the temporary vectors
    QString filename = FilePaths::resource("DustMix/Benchmark2DDustMix.dat");
    ifstream file(filename.toLocal8Bit().constData());
    if (!file.is_open()) throw FATALERROR("Could not open the data file " + filename);
    find<Log>()->info("Reading dust mix properties from file " + filename + "...");
    string line;
    while (file.peek() == '#') getline(file,line);
    double lambda, Csca, Cext;
    for (int k=0; k<Nlambda; k++)
    {
        file >> lambda >> Csca >> Cext;
        lambdav[k] = lambda * 1e-6;      // lambda in file is in micron, we need lambda in m;
        Cabsv[k] = Cext-Csca;
        Cscav[k] = Csca;
        asymmparv[k] = 0.0;
    }
    file.close();
    find<Log>()->info("File " + filename + " closed.");

    // add a dust population with these properties
    // provide a dust mass that leads to reasonable kappa values
    int kV = NR::locate_clip(lambdav, Units::lambdaV());
    double Mdust = (Cabsv[kV] + Cscav[kV]) / Units::kappaV();
    addpopulation(Mdust, lambdav,Cabsv,Cscav,asymmparv);
}

//////////////////////////////////////////////////////////////////////
