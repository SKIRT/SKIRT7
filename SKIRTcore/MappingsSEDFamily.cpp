/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include <cmath>
#include <fstream>
#include "MappingsSEDFamily.hpp"
#include "FatalError.hpp"
#include "FilePaths.hpp"
#include "Log.hpp"
#include "NR.hpp"
#include "Units.hpp"
#include "WavelengthGrid.hpp"

using namespace std;

//////////////////////////////////////////////////////////////////////

namespace
{
    // number of items in the library read by the constructor
    const int Nlambda = 1800;
    const int NZrel = 5;
    const int NlogC = 6;
    const int Nlogp = 5;
}

//////////////////////////////////////////////////////////////////////

MappingsSEDFamily::MappingsSEDFamily(SimulationItem* item)
{
    // Prepare the vectors for the MAPPINGS III library SEDs
    _lambdav.resize(Nlambda);
    _Zrelv.resize(NZrel);
    _logCv.resize(NlogC);
    _logpv.resize(Nlogp);
    _j0vv.resize(NZrel,NlogC,Nlogp,Nlambda);
    _j1vv.resize(NZrel,NlogC,Nlogp,Nlambda);

    vector<QString> Zrelnamev(NZrel);
    _Zrelv[0] = 0.05;     Zrelnamev[0] = "Z005";
    _Zrelv[1] = 0.20;     Zrelnamev[1] = "Z020";
    _Zrelv[2] = 0.40;     Zrelnamev[2] = "Z040";
    _Zrelv[3] = 1.00;     Zrelnamev[3] = "Z100";
    _Zrelv[4] = 2.00;     Zrelnamev[4] = "Z200";
    vector<QString> logCnamev(NlogC);
    _logCv[0] = 4.0;      logCnamev[0] = "C40";
    _logCv[1] = 4.5;      logCnamev[1] = "C45";
    _logCv[2] = 5.0;      logCnamev[2] = "C50";
    _logCv[3] = 5.5;      logCnamev[3] = "C55";
    _logCv[4] = 6.0;      logCnamev[4] = "C60";
    _logCv[5] = 6.5;      logCnamev[5] = "C65";
    vector<QString> logpnamev(Nlogp);
    _logpv[0] = 4.0;      logpnamev[0] = "p4";
    _logpv[1] = 5.0;      logpnamev[1] = "p5";
    _logpv[2] = 6.0;      logpnamev[2] = "p6";
    _logpv[3] = 7.0;      logpnamev[3] = "p7";
    _logpv[4] = 8.0;      logpnamev[4] = "p8";

    // Read in the emissivity vectors
    double lambda, j0, j1;
    for (int i=0; i<NZrel; i++)
        for (int j=0; j<NlogC; j++)
            for (int k=0; k<Nlogp; k++)
            {
                Array& j0v = _j0vv(i,j,k);
                Array& j1v = _j1vv(i,j,k);
                QString filename = FilePaths::resource("SED/Mappings/Mappings_")
                                   + Zrelnamev[i] + "_" + logCnamev[j] + "_" + logpnamev[k] + ".dat";
                ifstream file(filename.toLocal8Bit().constData());
                if (! file.is_open()) throw FATALERROR("Could not open the data file " + filename);
                item->find<Log>()->info("Reading SED data from file " + filename + "...");
                for (int l=0; l<Nlambda; l++)
                {
                    file >> lambda >> j0 >> j1;
                    _lambdav[l] = lambda;
                    j0v[l] = j0;
                    j1v[l] = j1;
                }
                file.close();
                item->find<Log>()->info("File " + filename + " closed.");
            }

    // cache the simulation's wavelength grid
    _lambdagrid = item->find<WavelengthGrid>();
}

//////////////////////////////////////////////////////////////////////

Array
MappingsSEDFamily::luminosities(double SFR, double Z, double logC, double pressure, double fPDR) const
{
    // convert the input parameters to the parameters that are assumed in MAPPINGS III.
    // * the metallicity is converted from an absolute value Z to a value Zrel relative to the
    //   sun, where Zsun = 0.0122 as in Asplund et al. (2005). The same value is used in the MAPPINGS III
    //   models of Groves et al. (2008).
    // * the pressure is converted from the actual pressure in SI units (i.e. in Pa = N/m^2) to
    //   log(p/k), with k Boltzmann's constant, and in units of K/cm^3
    double Zrel = Z/0.0122;
    double logp = log10(pressure/Units::k()*1e-6);

    // ensure that the value of the parameters is within the boundaries of the parameter space
    Zrel = max(Zrel,0.05);
    Zrel = min(Zrel,2.0-1e-8);
    logC = max(logC,4.0);
    logC = min(logC,6.5-1e-8);
    logp = max(logp,4.0);
    logp = min(logp,8.0-1e-8);

    // find the appropriate SED from interpolating in the library
    int i = NR::locate_clip(_Zrelv,Zrel);
    double hZrel = (Zrel-_Zrelv[i])/(_Zrelv[i+1]-_Zrelv[i]);
    int j = NR::locate_clip(_logCv,logC);
    double hlogC = (logC-_logCv[j])/(_logCv[j+1]-_logCv[j]);
    int k = NR::locate_clip(_logpv,logp);
    double hlogp = (logp-_logpv[k])/(_logpv[k+1]-_logpv[k]);
    const Array& j0LLLv = _j0vv(i  , j  , k  );
    const Array& j0RLLv = _j0vv(i+1, j  , k  );
    const Array& j0LRLv = _j0vv(i  , j+1, k  );
    const Array& j0RRLv = _j0vv(i+1, j+1, k  );
    const Array& j0LLRv = _j0vv(i  , j  , k+1);
    const Array& j0RLRv = _j0vv(i+1, j  , k+1);
    const Array& j0LRRv = _j0vv(i  , j+1, k+1);
    const Array& j0RRRv = _j0vv(i+1, j+1, k+1);
    const Array& j1LLLv = _j1vv(i  , j  , k  );
    const Array& j1RLLv = _j1vv(i+1, j  , k  );
    const Array& j1LRLv = _j1vv(i  , j+1, k  );
    const Array& j1RRLv = _j1vv(i+1, j+1, k  );
    const Array& j1LLRv = _j1vv(i  , j  , k+1);
    const Array& j1RLRv = _j1vv(i+1, j  , k+1);
    const Array& j1LRRv = _j1vv(i  , j+1, k+1);
    const Array& j1RRRv = _j1vv(i+1, j+1, k+1);

    Array jv(Nlambda);
    for (int k=0; k<Nlambda; k++)
    {
        double j0 = (1.0-hZrel)*(1.0-hlogC)*(1.0-hlogp)*j0LLLv[k]
                    + hZrel*(1.0-hlogC)*(1.0-hlogp)*j0RLLv[k]
                    + (1.0-hZrel)*hlogC*(1.0-hlogp)*j0LRLv[k]
                    + hZrel*hlogC*(1.0-hlogp)*j0RRLv[k]
                    + (1.0-hZrel)*(1.0-hlogC)*hlogp*j0LLRv[k]
                    + hZrel*(1.0-hlogC)*hlogp*j0RLRv[k]
                    + (1.0-hZrel)*hlogC*hlogp*j0LRRv[k]
                    + hZrel*hlogC*hlogp*j0RRRv[k];
        double j1 = (1.0-hZrel)*(1.0-hlogC)*(1.0-hlogp)*j1LLLv[k]
                    + hZrel*(1.0-hlogC)*(1.0-hlogp)*j1RLLv[k]
                    + (1.0-hZrel)*hlogC*(1.0-hlogp)*j1LRLv[k]
                    + hZrel*hlogC*(1.0-hlogp)*j1RRLv[k]
                    + (1.0-hZrel)*(1.0-hlogC)*hlogp*j1LLRv[k]
                    + hZrel*(1.0-hlogC)*hlogp*j1RLRv[k]
                    + (1.0-hZrel)*hlogC*hlogp*j1LRRv[k]
                    + hZrel*hlogC*hlogp*j1RRRv[k];
        jv[k] = (1.0-fPDR)*j0 + fPDR*j1;
    }

    // resample to the simulation wavelength grid,
    // convert emissivities to luminosities (i.e. multiply by the wavelength bins),
    // multiply by the SFR (the MAPPINGSIII templates correspond to a SFR of 1 Msun/yr)
    // and return the result
    return NR::resample<NR::interpolate_loglog>(_lambdagrid->lambdav(), _lambdav, jv) * _lambdagrid->dlambdav() * SFR;
}

//////////////////////////////////////////////////////////////////////
