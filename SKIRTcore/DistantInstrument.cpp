/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include <fstream>
#include <iomanip>
#include "DistantInstrument.hpp"
#include "FatalError.hpp"
#include "FilePaths.hpp"
#include "Log.hpp"
#include "PeerToPeerCommunicator.hpp"
#include "PhotonPackage.hpp"
#include "Units.hpp"
#include "WavelengthGrid.hpp"

using namespace std;

////////////////////////////////////////////////////////////////////

DistantInstrument::DistantInstrument()
    : _distance(0), _azimuth(0), _inclination(0), _positionangle(0)
{
}

////////////////////////////////////////////////////////////////////

void DistantInstrument::setupSelfBefore()
{
    Instrument::setupSelfBefore();

    // verify attribute values
    if (_distance <= 0) throw FATALERROR("Distance was not set");

    // calculate sine and cosine for our angles
    _costheta = cos(_inclination);
    _sintheta = sin(_inclination);
    _cosphi = cos(_azimuth);
    _sinphi = sin(_azimuth);
    _cospa = cos(_positionangle);
    _sinpa = sin(_positionangle);

    // calculate relevant directions
    _bfkobs = Direction(_inclination,_azimuth);
    _bfkx = Direction( + _cosphi*_costheta*_sinpa - _sinphi*_cospa,
                       + _sinphi*_costheta*_sinpa + _cosphi*_cospa,
                       - _sintheta*_sinpa );
    _bfky = Direction( - _cosphi*_costheta*_cospa - _sinphi*_sinpa,
                       - _sinphi*_costheta*_cospa + _cosphi*_sinpa,
                       + _sintheta*_cospa );
}

////////////////////////////////////////////////////////////////////

void DistantInstrument::setDistance(double value)
{
    _distance = value;
}

////////////////////////////////////////////////////////////////////

double DistantInstrument::distance() const
{
    return _distance;
}

////////////////////////////////////////////////////////////////////

void DistantInstrument::setInclination(double value)
{
    _inclination = value;
}

////////////////////////////////////////////////////////////////////

double DistantInstrument::inclination() const
{
    return _inclination;
}

////////////////////////////////////////////////////////////////////

void DistantInstrument::setAzimuth(double value)
{
    _azimuth = value;
}

////////////////////////////////////////////////////////////////////

double DistantInstrument::azimuth() const
{
    return _azimuth;
}

////////////////////////////////////////////////////////////////////

void DistantInstrument::setPositionAngle(double value)
{
    _positionangle = value;
}

////////////////////////////////////////////////////////////////////

double DistantInstrument::positionAngle() const
{
    return _positionangle;
}

////////////////////////////////////////////////////////////////////

Direction DistantInstrument::bfkobs(const Position& /*bfr*/) const
{
    return _bfkobs;
}

////////////////////////////////////////////////////////////////////

Direction DistantInstrument::bfkx() const
{
    return _bfkx;
}

////////////////////////////////////////////////////////////////////

Direction DistantInstrument::bfky() const
{
    return _bfky;
}

////////////////////////////////////////////////////////////////////

void DistantInstrument::calibrateAndWriteSEDs(QList< Array* > Farrays, QStringList Fnames)
{
    PeerToPeerCommunicator* comm = find<PeerToPeerCommunicator>();
    if (comm->rank()) return;

    WavelengthGrid* lambdagrid = find<WavelengthGrid>();
    int Nlambda = find<WavelengthGrid>()->Nlambda();

    // calibration step 1: conversion from bolometric luminosities (units W) to monochromatic luminosities (units W/m)

    for (int ell=0; ell<Nlambda; ell++)
    {
        double dlambda = lambdagrid->dlambda(ell);
        foreach (Array* Farr, Farrays)
        {
            if (Farr->size()) (*Farr)[ell] /= dlambda;
        }
    }

    // calibration step 2: conversion of the integrated flux from monochromatic luminosity units (W/m) to
    //                     flux density units (W/m3) by taking into account the distance

    double fourpid2 = 4.0*M_PI*_distance*_distance;
    foreach (Array* Farr, Farrays)
    {
        (*Farr) /= fourpid2;
    }

    // write a text file for easy SED plotting

    Units* units = find<Units>();
    QString sedfilename = find<FilePaths>()->output(_instrumentname + "_sed.dat");
    find<Log>()->info("Writing SED to " + sedfilename + "...");
    ofstream sedfile(sedfilename.toLocal8Bit().constData());
    sedfile << "# column 1: lambda (" << units->uwavelength().toStdString() << ")\n";
    for (int q = 0; q < Farrays.size(); q++)
    {
        sedfile << "# column " << (q+2) << ": "
                << Fnames[q].toStdString() << "; "
                << units->sfluxdensity().toStdString() << " "
                << "(" << units->ufluxdensity().toStdString() << ")\n";
    }
    sedfile << scientific << setprecision(8);
    for (int ell=0; ell<Nlambda; ell++)
    {
        double lambda = lambdagrid->lambda(ell);
        sedfile << units->owavelength(lambda);
        foreach (Array* Farr, Farrays)
        {
            sedfile << '\t' << (Farr->size() ? units->ofluxdensity(lambda, (*Farr)[ell]) : 0.);
        }
        sedfile << endl;
    }
}

////////////////////////////////////////////////////////////////////
