/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include <cmath>
#include <fstream>
#include "BruzualCharlotSEDFamily.hpp"
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
    const int Nlambda = 1221;
    const int Nt = 221;
    const int NZ = 6;
}

/////////////////////////////////////////////////////////////////////////

BruzualCharlotSEDFamily::BruzualCharlotSEDFamily()
{
}

///////////////////////////////////////////////////////////////////

BruzualCharlotSEDFamily::BruzualCharlotSEDFamily(SimulationItem* parent)
{
    setParent(parent);
    setup();
}

///////////////////////////////////////////////////////////////////

void BruzualCharlotSEDFamily::setupSelfBefore()
{
    SEDFamily::setupSelfBefore();

    // local constants for units
    const double Lsun = Units::Lsun();
    const double Angstrom = 1e-10;

    // Prepare the vectors for the Bruzual & Charlot library SEDs
    _lambdav.resize(Nlambda);
    _tv.resize(Nt);
    _Zv.resize(NZ);
    _jvv.resize(Nt,NZ,Nlambda);

    // Fill the metallicity vector
    vector<QString> Zcodev(NZ);
    _Zv[0] = 0.0001;     Zcodev[0] = "m22";
    _Zv[1] = 0.0004;     Zcodev[1] = "m32";
    _Zv[2] = 0.004;      Zcodev[2] = "m42";
    _Zv[3] = 0.008;      Zcodev[3] = "m52";
    _Zv[4] = 0.02;	     Zcodev[4] = "m62";
    _Zv[5] = 0.05;	     Zcodev[5] = "m72";

    // Read the wavelength, age and emissivity vectors from the Bruzual & Charlot library
    for (int m=0; m<NZ; m++)
    {
        QString bcfilename = FilePaths::resource("SED/BruzualCharlot/chabrier/bc2003_lr_"
                                                 + Zcodev[m] + "_chab_ssp.ised_ASCII");
        ifstream bcfile(bcfilename.toLocal8Bit().constData());
        if (! bcfile.is_open()) throw FATALERROR("Could not open the data file " + bcfilename);

        find<Log>()->info("Reading SED data from file " + bcfilename + "...");
        int iNt, iNlambda;
        bcfile >> iNt;
        if (iNt != Nt)
            throw FATALERROR("iNt is not equal to Nt");
        for (int p=0; p<Nt; p++)
        {
            double t;
            bcfile >> t;
            _tv[p] = t;     // age in file in yr, we want in yr
        }
        string dummy;
        for (int l=0; l<6; l++)
            getline(bcfile,dummy); // skip six lines...
        bcfile >> iNlambda;
        if (iNlambda != Nlambda)
            throw FATALERROR("iNlambda is not equal to Nlambda");
        for (int k=0; k<Nlambda; k++)
        {
            double lambda;
            bcfile >> lambda;
            _lambdav[k] = lambda * Angstrom;   // lambda in file in A, we want in m
        }
        for (int p=0; p<Nt; p++)
        {
            Array& jv = _jvv(p,m);
            bcfile >> iNlambda;
            if (iNlambda != Nlambda)
                throw FATALERROR("iNlambda is not equal to Nlambda");
            for (int k=0; k<Nlambda; k++)
            {
                double j;
                bcfile >> j;
                jv[k] = j * Lsun/Angstrom;   // emissivity in file in Lsun/A, we want in W/m.
            }
            int idummy;
            bcfile >> idummy;
            for (int k=0; k<idummy; k++)
            {
                double dummy;
                bcfile >> dummy;
            }
        }
        bcfile.close();
        find<Log>()->info("File " + bcfilename + " closed.");
    }

    // cache the simulation's wavelength grid
    _lambdagrid = find<WavelengthGrid>();
}

//////////////////////////////////////////////////////////////////////

Array BruzualCharlotSEDFamily::luminosities(double M, double Z, double t, double z) const
{
    // find the appropriate SED from interpolating in the BC library
    int mL, mR;
    double hZ = 0.0;
    if (Z<=_Zv[0])
        mL = mR = 0;
    else if (Z>=_Zv[NZ-1])
        mL = mR = NZ-1;
    else
    {
        mL = NR::locate_clip(_Zv,Z);
        mR = mL+1;
        double ZL = _Zv[mL];
        double ZR = _Zv[mR];
        hZ = (Z-ZL)/(ZR-ZL);
    }
    int pL, pR;
    double ht = 0.0;
    if (t<=_tv[0])
        pL = pR = 0;
    else if (t>=_tv[Nt-1])
        pL = pR = Nt-1;
    else
    {
        pL = NR::locate_clip(_tv,t);
        pR = pL+1;
        double tL = _tv[pL];
        double tR = _tv[pR];
        ht = (t-tL)/(tR-tL);
    }
    const Array& jLLv = _jvv(pL,mL);
    const Array& jLRv = _jvv(pL,mR);
    const Array& jRLv = _jvv(pR,mL);
    const Array& jRRv = _jvv(pR,mR);
    Array jv(Nlambda);
    for (int k=0; k<Nlambda; k++)
        jv[k] = (1.0-ht)*(1.0-hZ)*jLLv[k]
                + (1.0-ht)*hZ*jLRv[k]
                + ht*(1.0-hZ)*jRLv[k]
                + ht*hZ*jRRv[k];

    // resample to the possibly redshifted simulation wavelength grid,
    // convert emissivities to luminosities (i.e. multiply by the wavelength bins),
    // multiply by the mass of the population (in solar masses),
    // and return the result
    return NR::resample<NR::interpolate_loglog>(_lambdagrid->lambdav()*(1-z), _lambdav, jv)
                                    * _lambdagrid->dlambdav() * M;
}

//////////////////////////////////////////////////////////////////////

int BruzualCharlotSEDFamily::nparams() const
{
    return 3;
}

//////////////////////////////////////////////////////////////////////

Array BruzualCharlotSEDFamily::luminosities_generic(const Array& params, int skipvals, double z) const
{
    return luminosities(params[skipvals], params[skipvals+1], params[skipvals+2], z);
}

//////////////////////////////////////////////////////////////////////

double BruzualCharlotSEDFamily::mass_generic(const Array& params, int skipvals) const
{
    return params[skipvals];
}

//////////////////////////////////////////////////////////////////////

QString BruzualCharlotSEDFamily::sourceName() const
{
    return "star";
}

//////////////////////////////////////////////////////////////////////

QString BruzualCharlotSEDFamily::sourceDescription() const
{
    return "star";
}

//////////////////////////////////////////////////////////////////////
