/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include "FatalError.hpp"
#include "FilePaths.hpp"
#include "FITSInOut.hpp"
#include "Log.hpp"
#include "PhotonPackage.hpp"
#include "SingleFrameInstrument.hpp"
#include "Units.hpp"
#include "WavelengthGrid.hpp"

using namespace std;

////////////////////////////////////////////////////////////////////

SingleFrameInstrument::SingleFrameInstrument()
    : _Nxp(0), _xpmax(0), _Nyp(0), _ypmax(0)
{
}

////////////////////////////////////////////////////////////////////

void SingleFrameInstrument::setupSelfBefore()
{
    DistantInstrument::setupSelfBefore();

    // verify attribute values
    if (_Nxp <= 0 || _Nyp <= 0) throw FATALERROR("Number of pixels was not set");
    if (_xpmax <= 0 || _ypmax <= 0) throw FATALERROR("Maximum extent was not set");

    // calculate derived values
    _xpres = 2.0*_xpmax/(_Nxp-1);
    _ypres = 2.0*_ypmax/(_Nyp-1);
    _xpmin = -_xpmax;
    _ypmin = -_ypmax;
}

////////////////////////////////////////////////////////////////////

void SingleFrameInstrument::setPixelsX(int value)
{
    _Nxp = value;
}

////////////////////////////////////////////////////////////////////

int SingleFrameInstrument::pixelsX() const
{
    return _Nxp;
}

////////////////////////////////////////////////////////////////////

void SingleFrameInstrument::setExtentX(double value)
{
    _xpmax = value;
}

////////////////////////////////////////////////////////////////////

double SingleFrameInstrument::extentX() const
{
    return _xpmax;
}

////////////////////////////////////////////////////////////////////

void SingleFrameInstrument::setPixelsY(int value)
{
    _Nyp = value;
}

////////////////////////////////////////////////////////////////////

int SingleFrameInstrument::pixelsY() const
{
    return _Nyp;
}

////////////////////////////////////////////////////////////////////

void SingleFrameInstrument::setExtentY(double value)
{
    _ypmax = value;
}

////////////////////////////////////////////////////////////////////

double SingleFrameInstrument::extentY() const
{
    return _ypmax;
}

////////////////////////////////////////////////////////////////////

int SingleFrameInstrument::pixelondetector(const PhotonPackage* pp) const
{
    // get the position
    double x, y, z;
    pp->position().cartesian(x,y,z);

    // transform to detector coordinates using inclination, azimuth, and position angle
    double xpp = - _sinphi*x + _cosphi*y;
    double ypp = - _cosphi*_costheta*x - _sinphi*_costheta*y + _sintheta*z;
    double xp = _cospa * xpp - _sinpa * ypp;
    double yp = _sinpa * xpp + _cospa * ypp;

    // scale and round to pixel index
    int i = static_cast<int>(floor(((xp-_xpmin)/_xpres)+0.5));
    int j = static_cast<int>(floor(((yp-_ypmin)/_ypres)+0.5));
    if (i<0 || i>=_Nxp || j<0 || j>=_Nyp) return -1;
    else return i + _Nxp*j;
}

////////////////////////////////////////////////////////////////////

void SingleFrameInstrument::calibrateAndWriteDataCubes(QList< Array*> farrays, QStringList fnames)
{
    WavelengthGrid* lambdagrid = find<WavelengthGrid>();
    int Nlambda = lambdagrid->Nlambda();
    Units* units = find<Units>();
    double c = units->c();

    // calibration step 1: conversion from bolometric luminosities (units W)
    // to monochromatic luminosities (units W/Hz)

    for (int ell=0; ell<Nlambda; ell++)
    {
        double lambda = lambdagrid->lambda(ell);
        double dlambda = lambdagrid->dlambda(ell);
        for (int i=0; i<_Nxp; i++)
        {
            for (int j=0; j<_Nyp; j++)
            {
                int m = i + _Nxp*j + _Nxp*_Nyp*ell;
                foreach (Array* farr, farrays)
                {
                    (*farr)[m] *= lambda*lambda/c/dlambda;
                }
            }
        }
    }

    // calibration step 2: correction for the area of the pixels of the
    // images and the distance; the units are now W/m2/Hz/sr

    double xpresang = 2.0*atan(_xpres/(2.0*_distance));
    double ypresang = 2.0*atan(_ypres/(2.0*_distance));
    double area = xpresang*ypresang;
    double fourpid2 = 4.0*M_PI*_distance*_distance;
    foreach (Array* farr, farrays)
    {
        (*farr) /= fourpid2*area;
    }

    // calibration step 3: conversion from program SI units (W/m2/Hz/sr) to the
    // correct output units (typically MJy/sr)

    for (int ell=0; ell<Nlambda; ell++)
    {
        for (int i=0; i<_Nxp; i++)
        {
            for (int j=0; j<_Nyp; j++)
            {
                int m = i + _Nxp*j + _Nxp*_Nyp*ell;
                foreach (Array* farr, farrays)
                {
                    (*farr)[m] = units->osurfacebrightness((*farr)[m]);
                }
            }
        }
    }

    // write a FITS file for each array

    QString filename = find<FilePaths>()->output(_instrumentname);
    for (int q = 0; q < farrays.size(); q++)
    {
        QString fitsfilename = filename + "_" + fnames[q] + ".fits";
        find<Log>()->info("Writing " + fnames[q] + " flux to FITS file " + fitsfilename + "...");
        FITSInOut::write(fitsfilename, *(farrays[q]), _Nxp, _Nyp, Nlambda,
                       units->olength(_xpres), units->olength(_ypres),
                       units->usurfacebrightness(), units->ulength());
    }
}

////////////////////////////////////////////////////////////////////
