/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "FatalError.hpp"
#include "FilePaths.hpp"
#include "Image.hpp"
#include "Log.hpp"
#include "PhotonPackage.hpp"
#include "SingleFrameInstrument.hpp"
#include "Units.hpp"
#include "WavelengthGrid.hpp"

using namespace std;

////////////////////////////////////////////////////////////////////

SingleFrameInstrument::SingleFrameInstrument()
    : _Nxp(0), _fovxp(0), _xpc(0), _Nyp(0), _fovyp(0), _ypc(0)
{
}

////////////////////////////////////////////////////////////////////

void SingleFrameInstrument::setupSelfBefore()
{
    DistantInstrument::setupSelfBefore();

    // verify attribute values
    if (_Nxp <= 0 || _Nyp <= 0) throw FATALERROR("Number of pixels was not set");
    if (_fovxp <= 0 || _fovyp <= 0) throw FATALERROR("Field of view was not set");

    // calculate derived values
    _Nframep = _Nxp * _Nyp;
    _xpmin = _xpc - 0.5*_fovxp;
    _xpmax = _xpc + 0.5*_fovxp;
    _xpsiz = _fovxp/_Nxp;
    _ypmin = _ypc - 0.5*_fovyp;
    _ypmax = _ypc + 0.5*_fovyp;
    _ypsiz = _fovyp/_Nyp;
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

void SingleFrameInstrument::setFieldOfViewX(double value)
{
    _fovxp = value;
}

////////////////////////////////////////////////////////////////////

double SingleFrameInstrument::fieldOfViewX() const
{
    return _fovxp;
}

////////////////////////////////////////////////////////////////////

void SingleFrameInstrument::setCenterX(double value)
{
    _xpc = value;
}

////////////////////////////////////////////////////////////////////

double SingleFrameInstrument::centerX() const
{
    return _xpc;
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

void SingleFrameInstrument::setFieldOfViewY(double value)
{
    _fovyp = value;
}

////////////////////////////////////////////////////////////////////

double SingleFrameInstrument::fieldOfViewY() const
{
    return _fovyp;
}

////////////////////////////////////////////////////////////////////

void SingleFrameInstrument::setCenterY(double value)
{
    _ypc = value;
}

////////////////////////////////////////////////////////////////////

double SingleFrameInstrument::centerY() const
{
    return _ypc;
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
    int i = static_cast<int>(floor((xp-_xpmin)/_xpsiz));
    int j = static_cast<int>(floor((yp-_ypmin)/_ypsiz));
    if (i<0 || i>=_Nxp || j<0 || j>=_Nyp) return -1;
    else return i + _Nxp*j;
}

////////////////////////////////////////////////////////////////////

void SingleFrameInstrument::calibrateAndWriteDataCubes(QList< Array*> farrays, QStringList fnames)
{
    WavelengthGrid* lambdagrid = find<WavelengthGrid>();
    int Nlambda = lambdagrid->Nlambda();

    // calibration step 1: conversion from bolometric luminosities (units W) to monochromatic luminosities (units W/m)

    for (int ell=0; ell<Nlambda; ell++)
    {
        double dlambda = lambdagrid->dlambda(ell);
        for (int i=0; i<_Nxp; i++)
        {
            for (int j=0; j<_Nyp; j++)
            {
                size_t m = i + _Nxp*j + _Nframep*ell;
                foreach (Array* farr, farrays)
                {
                    if (farr->size()) (*farr)[m] /= dlambda;
                }
            }
        }
    }

    // calibration step 2: correction for the area of the pixels of the images; the units are now W/m/sr

    double xpsizang = 2.0*atan(_xpsiz/(2.0*_distance));
    double ypsizang = 2.0*atan(_ypsiz/(2.0*_distance));
    double area = xpsizang*ypsizang;
    foreach (Array* farr, farrays)
    {
        (*farr) /= area;
    }

    // calibration step 3: conversion of the flux per pixel from monochromatic luminosity units (W/m/sr)
    // to flux density units (W/m3/sr) by taking into account the distance

    double fourpid2 = 4.0*M_PI*_distance*_distance;
    foreach (Array* farr, farrays)
    {
        (*farr) /= fourpid2;
    }

    // conversion from program SI units (at this moment W/m3/sr) to the correct output units;
    // we use lambda*flambda for the surface brightness (in units like W/m2/arcsec2)

    Units* units = find<Units>();
    for (int ell=0; ell<Nlambda; ell++)
    {
        double lambda = lambdagrid->lambda(ell);
        for (int i=0; i<_Nxp; i++)
        {
            for (int j=0; j<_Nyp; j++)
            {
                size_t m = i + _Nxp*j + _Nframep*ell;
                foreach (Array* farr, farrays)
                {
                    if (farr->size()) (*farr)[m] = units->osurfacebrightness(lambda, (*farr)[m]);
                }
            }
        }
    }

    // Write a FITS file for each array
    for (int q = 0; q < farrays.size(); q++)
    {
        if (farrays[q]->size())
        {
            QString filename = _instrumentname + "_" + fnames[q];
            QString description = fnames[q] + " flux";

            // Create an image and save it
            Image image(this, _Nxp, _Nyp, Nlambda, _xpsiz, _ypsiz, _xpc, _ypc, "surfacebrightness");
            image.saveto(this, *(farrays[q]), filename, description);
        }
    }
}

////////////////////////////////////////////////////////////////////
