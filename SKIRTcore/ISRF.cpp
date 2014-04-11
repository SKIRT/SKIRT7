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
    int Nlambda = lambdagrid->Nlambda();

    const double Wv[] = {1e-14,1e-13,4e-13};
    const double Tv[] = {7500.0,4000.0,3000.0};
    const double lambda912 = 912e-10;
    const double lambda110 = 110e-9;
    const double lambda134 = 134e-9;
    const double lambda246 = 246e-9;
    const Array& lambdav = lambdagrid->lambdav();
    int ell912 = NR::locate_clip(lambdav,lambda912);
    int ell110 = NR::locate_clip(lambdav,lambda110);
    int ell134 = NR::locate_clip(lambdav,lambda134);
    int ell246 = NR::locate_clip(lambdav,lambda246);

    Array Jv(Nlambda);
    for (int ell=ell912+1; ell<=ell110; ell++)
        Jv[ell] = 38.57 * pow(lambdav[ell]*1e6,3.4172);
    for (int ell=ell110+1; ell<=ell134; ell++)
        Jv[ell] = 2.045e-2;
    for (int ell=ell134+1; ell<=ell246; ell++)
        Jv[ell] = 7.115e-4 * pow(lambdav[ell]*1e6,-1.6678);
    for (int ell=0; ell<=ell246; ell++)
        Jv[ell] *= 1e3;     // conversion from erg/cm2/s/micron to W/m3
    for (int i=0; i<3; i++)
    {
        PlanckFunction B(Tv[i]);
        for (int ell=ell246+1; ell<Nlambda; ell++)
            Jv[ell] += 4*M_PI* Wv[i] * B(lambdav[ell]);
    }
    Jv /= (4*M_PI);
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
