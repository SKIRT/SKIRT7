/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include <fstream>
#include "FatalError.hpp"
#include "FilePaths.hpp"
#include "MarastonSED.hpp"
#include "Log.hpp"
#include "NR.hpp"

using namespace std;

//////////////////////////////////////////////////////////////////////

MarastonSED::MarastonSED()
    : _tau(0), _Z(0)
{
}

//////////////////////////////////////////////////////////////////////

void MarastonSED::setupSelfBefore()
{
    StellarSED::setupSelfBefore();

    // verify that property values are in the grid
    if (_tau<1e-6 || _tau>=15.0) throw FATALERROR("the age tau should be between 1e-6 and 15");
    if (_Z<0.0001 || _Z>=0.07) throw FATALERROR("the metallicity Z should be between 0.0001 and 0.07");
    if (_Z<0.001 && _tau<1.0) throw FATALERROR("For metallicity Z<0.001, age tau should be larger than 1");
    if (_Z>0.04 && _tau<1.0) throw FATALERROR("For metallicity Z>0.04, age tau should be larger than 1");

    // fill vector with metallicities
    Array Zv(6);
    Zv[0] = 0.0001;
    Zv[1] = 0.001;
    Zv[2] = 0.01;
    Zv[3] = 0.02;
    Zv[4] = 0.04;
    Zv[5] = 0.07;

    // determine the bracketing sed filenames based on desired metallicity
    int mL = NR::locate_clip(Zv,_Z);
    double ZL = Zv[mL];
    double ZR = Zv[mL+1];
    QString fileLname = FilePaths::resource("SED/Maraston/sed.ssz");
    QString fileRname = fileLname;
    int NlinesL = 0, NlinesR = 0;
    if (mL==0) {
        fileLname += "10m4.rhb";
        fileRname += "0001.rhb";
        NlinesL = 19536;
        NlinesR = 81807;
    }
    else if (mL==1) {
        fileLname += "0001.rhb";
        fileRname += "001.rhb";
        NlinesL = 19536;
        NlinesR = 81807;
    }
    else if (mL==2) {
        fileLname += "001.rhb";
        fileRname += "002.rhb";
        NlinesL = 81807;
        NlinesR = 81807;
    }
    else if (mL==3) {
        fileLname += "002.rhb";
        fileRname += "004.rhb";
        NlinesL = 81807;
        NlinesR = 81807;
    }
    else if (mL==4) {
        fileLname += "004.rhb";
        fileRname += "007.rhb";
        NlinesL = 81807;
        NlinesR = 19536;
    }

    // fill vector with ages
    QString filename = FilePaths::resource("SED/Maraston/ages.dat");
    ifstream file(filename.toLocal8Bit().constData());
    if (! file.is_open()) throw FATALERROR("Could not open the data file " + filename);
    find<Log>()->info("Reading SED data from file " + filename + "...");
    int Ntau = 67;
    Array tauv(Ntau);
    for (int l=0; l<Ntau; l++) file >> tauv[l];
    file.close();
    find<Log>()->info("File " + filename + " closed.");

    // determine the bracketing ages
    int lL = NR::locate_clip(tauv,_tau);
    double tauL = tauv[lL];
    double tauR = tauv[lL+1];

    // initialize the wavelength and flux vectors
    const int Nlambda = 1221;
    Array lambdav(Nlambda);
    Array jLLv(Nlambda);
    Array jLRv(Nlambda);
    Array jRLv(Nlambda);
    Array jRRv(Nlambda);
    double age, ZH, lambda, j;

    // read the fluxes from the left sed file
    ifstream fileL(fileLname.toLocal8Bit().constData());
    if (! fileL.is_open()) throw FATALERROR("Could not open the data file " + fileLname);
    find<Log>()->info("Reading SED data from file " + fileLname + "...");
    for (int k=0; k<NlinesL; k++)
    {
        fileL >> age >> ZH >> lambda >> j;
        if (k<Nlambda)
            lambdav[k] = lambda*1e-10; // conversion from Angstrom to m
        if (age==tauL)
            jLLv[k%Nlambda] = j;
        else if (age==tauR)
            jLRv[k%Nlambda] = j;
    }
    fileL.close();
    find<Log>()->info("File " + fileLname + " closed.");

    // read the fluxes from the right sed file
    ifstream fileR(fileLname.toLocal8Bit().constData());
    if (! fileR.is_open()) throw FATALERROR("Could not open the data file " + fileRname);
    find<Log>()->info("Reading SED data from file " + fileRname + "...");
    for (int k=0; k<NlinesR; k++)
    {
        fileR >> age >> ZH >> lambda >> j;
        if (age==tauL)
            jRLv[k%Nlambda] = j;
        else if (age==tauR)
            jRRv[k%Nlambda] = j;
    }
    fileR.close();
    find<Log>()->info("File " + fileRname + " closed.");

    // interpolate
    Array jv(Nlambda);
    double p = (_Z-ZL)/(ZR-ZL);
    double q = (_tau-tauL)/(tauR-tauL);
    for (int k=0; k<Nlambda; k++)
        jv[k] = (1.0-p)*(1.0-q)*jLLv[k]
                + p*(1.0-q)*jRLv[k]
                + (1.0-p)*q*jLRv[k]
                + p*q*jRRv[k];

    // finish up
    setemissivities(lambdav,jv);
}

////////////////////////////////////////////////////////////////////

void MarastonSED::setAge(double value)
{
    _tau = value;
}

////////////////////////////////////////////////////////////////////

double MarastonSED::age() const
{
    return _tau;
}

////////////////////////////////////////////////////////////////////

void MarastonSED::setMetallicity(double value)
{
    _Z = value;
}

////////////////////////////////////////////////////////////////////

double MarastonSED::metallicity() const
{
    return _Z;
}

////////////////////////////////////////////////////////////////////
