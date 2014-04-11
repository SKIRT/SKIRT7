/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include <cmath>
#include <fstream>
#include "FatalError.hpp"
#include "FilePaths.hpp"
#include "KuruczSED.hpp"
#include "Log.hpp"
#include "NR.hpp"

using namespace std;

////////////////////////////////////////////////////////////////////

KuruczSED::KuruczSED()
    : _Z(0), _Teff(0), _g(0)
{
}

////////////////////////////////////////////////////////////////////

void KuruczSED::setupSelfBefore()
{
    StellarSED::setupSelfBefore();

    // base directory for Kurucz resource library
    QString filename = FilePaths::resource("SED/Kurucz/");

    // determine first portion of filenames based on desired metallicity
    if (_Z < -4.75) filename += "km50/km50_";
    else if (_Z < -4.25) filename += "km45/km45_";
    else if (_Z < -3.75) filename += "km40/km40_";
    else if (_Z < -3.25) filename += "km35/km35_";
    else if (_Z < -2.75) filename += "km30/km30_";
    else if (_Z < -2.25) filename += "km25/km25_";
    else if (_Z < -1.75) filename += "km20/km20_";
    else if (_Z < -1.25) filename += "km15/km15_";
    else if (_Z < -0.75) filename += "km10/km10_";
    else if (_Z < -0.40) filename += "km05/km05_";
    else if (_Z < -0.25) filename += "km03/km03_";
    else if (_Z < -0.15) filename += "km02/km02_";
    else if (_Z < -0.05) filename += "km01/km01_";
    else if (_Z < 0.05) filename += "kp00/kp00_";
    else if (_Z < 0.15) filename += "kp01/kp01_";
    else if (_Z < 0.25) filename += "kp02/kp02_";
    else if (_Z < 0.40) filename += "kp03/kp03_";
    else if (_Z < 0.75) filename += "kp05/kp05_";
    else filename += "kp10/kp10_";

    // determine full filenames bracketing the desired effective temperature
    if (_Teff<3500 || _Teff>10000) throw FATALERROR("the effective temperature should be between 3500 and 10000");
    double TeffL = floor(_Teff/250.)*250.;
    if (TeffL==10000.) TeffL -= 250;  // include the rightmost margin in the last bin
    double TeffR = TeffL + 250.;
    QString filenameL = filename + QString::number(TeffL) + ".dat";
    QString filenameR = filename + QString::number(TeffR) + ".dat";

    // open both files
    ifstream fileL(filenameL.toLocal8Bit().constData());
    if (! fileL.is_open()) throw FATALERROR("Could not open the data file " + filenameL);
    find<Log>()->info("Reading SED data from file " + filenameL + "...");
    ifstream fileR(filenameR.toLocal8Bit().constData());
    if (! fileR.is_open()) throw FATALERROR("Could not open the data file " + filenameR);
    find<Log>()->info("Reading SED data from file " + filenameR + "...");

    // determine the flux choice index within each file depending on desired gravity
    int mchoice;
    if (_g<0.0)
        mchoice = 0;
    else if (_g>5.0)
        mchoice = 10;
    else
        mchoice = static_cast<int>(floor(2.0*_g+0.5));

    // construct two bracketing SEDs from the files
    int Nlambda = 1221;
    Array lambdav(Nlambda);
    Array jRv(Nlambda);
    Array jLv(Nlambda);
    int numberL, numberR;
    double lambdaL, lambdaR;
    Array fluxLgv(11);
    Array fluxRgv(11);
    for (int k=0; k<1221; k++)
    {
        fileL >> numberL >> lambdaL;
        fileR >> numberR >> lambdaR;
        for (int m=0; m<11; m++)
        {
            fileL >> fluxLgv[m];
            fileR >> fluxRgv[m];
        }
        if (lambdaL != lambdaR) throw FATALERROR("Values for lambdaL and lambdaR should be equal");
        lambdav[k] = lambdaL/1e10; // conversion from Angstrom to m
        jLv[k] = fluxLgv[mchoice];
        jRv[k] = fluxRgv[mchoice];
    }

    // close the files
    fileL.close();
    find<Log>()->info("File " + filenameL + " closed.");
    fileR.close();
    find<Log>()->info("File " + filenameR + " closed.");

    // determine the jv[k] vector by linear interpolation
    Array jv(Nlambda);
    for (int k=0; k<Nlambda; k++)
        jv[k] = jLv[k] + (_Teff-TeffL)/(TeffR-TeffL)*(jRv[k]-jLv[k]);

    // finish up
    setemissivities(lambdav,jv);
}

////////////////////////////////////////////////////////////////////

void KuruczSED::setMetallicity(double value)
{
    _Z = value;
}

////////////////////////////////////////////////////////////////////

double KuruczSED::metallicity() const
{
    return _Z;
}

////////////////////////////////////////////////////////////////////

void KuruczSED::setTemperature(double value)
{
    _Teff = value;
}

////////////////////////////////////////////////////////////////////

double KuruczSED::temperature() const
{
    return _Teff;
}

////////////////////////////////////////////////////////////////////

void KuruczSED::setGravity(double value)
{
    _g = value;
}

////////////////////////////////////////////////////////////////////

double KuruczSED::gravity() const
{
    return _g;
}

////////////////////////////////////////////////////////////////////
