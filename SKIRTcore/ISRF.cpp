/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include <cmath>
#include <fstream>
#include "FatalError.hpp"
#include "FilePaths.hpp"
#include "ISRF.hpp"
#include "Log.hpp"
#include "NR.hpp"
#include "PlanckFunction.hpp"
#include "WavelengthGrid.hpp"

using namespace std;

////////////////////////////////////////////////////////////////////

Array ISRF::mathis(SimulationItem* simitem)
{
    WavelengthGrid* lambdagrid = simitem->find<WavelengthGrid>();
    const Array& lambdav = lambdagrid->lambdav();
    int Nlambda = lambdagrid->Nlambda();
    int ellA = NR::locate_clip(lambdav, 0.0912e-6);
    int ellB = NR::locate_clip(lambdav, 0.110e-6);
    int ellC = NR::locate_clip(lambdav, 0.134e-6);
    int ellD = NR::locate_clip(lambdav, 0.250e-6);

    const double Wv[] = {1e-14,1e-13,4e-13};
    const double Tv[] = {7500,4000,3000};

    Array Jv(Nlambda);
    for (int ell=ellA+1; ell<=ellB; ell++)
        Jv[ell] = 3069. * pow(lambdav[ell]*1e6,3.4172);
    for (int ell=ellB+1; ell<=ellC; ell++)
        Jv[ell] = 1.627;
    for (int ell=ellC+1; ell<=ellD; ell++)
        Jv[ell] = 0.0566 * pow(lambdav[ell]*1e6,-1.6678);
    for (int i=0; i<3; i++)
    {
        PlanckFunction B(Tv[i]);
        for (int ell=ellD+1; ell<Nlambda; ell++)
            Jv[ell] += Wv[i] * B(lambdav[ell]);
    }
    return Jv;
}

////////////////////////////////////////////////////////////////////

Array ISRF::kruegel(SimulationItem* simitem)
{
    // Read the data from the file into local vectors lambdav[k] and Jv[k]
    const int Nlambda = 435;
    QString filename = FilePaths::resource("ISRF/ISRF-Kruegel.dat");
    ifstream file(filename.toLocal8Bit().constData());
    if (! file.is_open()) throw FATALERROR("Could not open the data file " + filename);
    simitem->find<Log>()->info("Reading ISRF data from file " + filename + "...");
    Array lambdav(Nlambda);
    Array Jv(Nlambda);
    for (int k = 0; k<Nlambda; k++)
    {
        file >> lambdav[k] >> Jv[k];
    }
    file.close();
    simitem->find<Log>()->info("File " + filename + " closed.");

    // resample on the simulation's wavelength grid
    WavelengthGrid* lambdagrid = simitem->find<WavelengthGrid>();
    return NR::resample<NR::interpolate_loglog>(lambdagrid->lambdav(), lambdav, Jv);
}

////////////////////////////////////////////////////////////////////
