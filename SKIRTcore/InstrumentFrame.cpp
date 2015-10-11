/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "FatalError.hpp"
#include "FilePaths.hpp"
#include "FITSInOut.hpp"
#include "Image.hpp"
#include "InstrumentFrame.hpp"
#include "Log.hpp"
#include "LockFree.hpp"
#include "PhotonPackage.hpp"
#include "MultiFrameInstrument.hpp"
#include "StellarSystem.hpp"
#include "Units.hpp"
#include "WavelengthGrid.hpp"

using namespace std;

////////////////////////////////////////////////////////////////////

InstrumentFrame::InstrumentFrame()
    : _Nxp(0), _fovxp(0), _xpc(0), _Nyp(0), _fovyp(0), _ypc(0)
{
}

////////////////////////////////////////////////////////////////////

void InstrumentFrame::setupSelfBefore()
{
    SimulationItem::setupSelfBefore();

    // verify attribute values
    if (_Nxp <= 0 || _Nyp <= 0) throw FATALERROR("Number of pixels was not set");
    if (_fovxp <= 0 || _fovyp <= 0) throw FATALERROR("Field of view was not set");

    // calculate derived values
    _Nframep = _Nxp * _Nyp;
    _xpmin = _xpc - 0.5*_fovxp;
    _xpmax = _xpc + 0.5*_fovxp;
    _xpres = _fovxp/_Nxp;
    _ypmin = _ypc - 0.5*_fovyp;
    _ypmax = _ypc + 0.5*_fovyp;
    _ypres = _fovyp/_Nyp;

    // copy information from parent instrument
    _instrument = find<MultiFrameInstrument>();
    _writeTotal = _instrument->writeTotal();
    _writeStellarComps = _instrument->writeStellarComps();
    _distance = _instrument->distance();
    double inclination = _instrument->inclination();
    double azimuth = _instrument->azimuth();
    double positionangle = _instrument->positionAngle();
    _costheta = cos(inclination);
    _sintheta = sin(inclination);
    _cosphi = cos(azimuth);
    _sinphi = sin(azimuth);
    _cospa = cos(positionangle);
    _sinpa = sin(positionangle);

    // initialize pixel frame(s)
    if (_writeTotal) _ftotv.resize(_Nframep);
    if (_writeStellarComps) _fcompvv.resize(find<StellarSystem>()->Ncomp(), _Nframep);
}

////////////////////////////////////////////////////////////////////

void InstrumentFrame::setPixelsX(int value)
{
    _Nxp = value;
}

////////////////////////////////////////////////////////////////////

int InstrumentFrame::pixelsX() const
{
    return _Nxp;
}

////////////////////////////////////////////////////////////////////

void InstrumentFrame::setFieldOfViewX(double value)
{
    _fovxp = value;
}

////////////////////////////////////////////////////////////////////

double InstrumentFrame::fieldOfViewX() const
{
    return _fovxp;
}

////////////////////////////////////////////////////////////////////

void InstrumentFrame::setCenterX(double value)
{
    _xpc = value;
}

////////////////////////////////////////////////////////////////////

double InstrumentFrame::centerX() const
{
    return _xpc;
}

////////////////////////////////////////////////////////////////////

void InstrumentFrame::setPixelsY(int value)
{
    _Nyp = value;
}

////////////////////////////////////////////////////////////////////

int InstrumentFrame::pixelsY() const
{
    return _Nyp;
}

////////////////////////////////////////////////////////////////////

void InstrumentFrame::setFieldOfViewY(double value)
{
    _fovyp = value;
}

////////////////////////////////////////////////////////////////////

double InstrumentFrame::fieldOfViewY() const
{
    return _fovyp;
}

////////////////////////////////////////////////////////////////////

void InstrumentFrame::setCenterY(double value)
{
    _ypc = value;
}

////////////////////////////////////////////////////////////////////

double InstrumentFrame::centerY() const
{
    return _ypc;
}

////////////////////////////////////////////////////////////////////

int InstrumentFrame::pixelondetector(const PhotonPackage* pp) const
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
    if (xp<_xpmin || xp>=_xpmax || yp<_ypmin || yp > _ypmax) return -1;
    int i = static_cast<int>(floor((xp-_xpmin)/_xpres));
    int j = static_cast<int>(floor((yp-_ypmin)/_ypres));
    return i + _Nxp*j;
}

////////////////////////////////////////////////////////////////////

void InstrumentFrame::detect(PhotonPackage* pp)
{
    int l = pixelondetector(pp);
    if (l >= 0)
    {
        double L = pp->luminosity();
        double taupath = _instrument->opticalDepth(pp);
        double extf = exp(-taupath);
        double Lextf = L*extf;

        if (_writeTotal) LockFree::add(_ftotv[l], Lextf);
        if (_writeStellarComps && pp->isStellar()) LockFree::add(_fcompvv(pp->stellarCompIndex(),l), Lextf);
    }
}

////////////////////////////////////////////////////////////////////

void InstrumentFrame::calibrateAndWriteData(int ell)
{
    // lists of f-array pointers, and the corresponding file names
    QList< Array* > farrays;
    QStringList fnames;

    if (_writeTotal)
    {
        farrays << &_ftotv;
        fnames << "total";
    }
    if (_writeStellarComps)
    {
        int Ncomp = find<StellarSystem>()->Ncomp();
        for (int k=0; k<Ncomp; k++)
        {
            farrays << &(_fcompvv[k]);
            fnames << "stellar_" + QString::number(k);
        }
    }

    // Sum the flux arrays element-wise across the different processes
    _instrument->sumResults(farrays);

    // calibrate and output the arrays
    calibrateAndWriteDataFrames(ell, farrays, fnames);
}

////////////////////////////////////////////////////////////////////

void InstrumentFrame::calibrateAndWriteDataFrames(int ell, QList<Array*> farrays, QStringList fnames)
{
    Units* units = find<Units>();
    WavelengthGrid* lambdagrid = find<WavelengthGrid>();

    // conversion from bolometric luminosities (units W) to monochromatic luminosities (units W/m)
    // --> divide by delta-lambda
    double dlambda = lambdagrid->dlambda(ell);

    // correction for the area of the pixels of the images; the units are now W/m/sr
    // --> divide by area
    double xpresang = 2.0*atan(_xpres/(2.0*_distance));
    double ypresang = 2.0*atan(_ypres/(2.0*_distance));
    double area = xpresang*ypresang;

    // calibration step 3: conversion of the flux per pixel from monochromatic luminosity units (W/m/sr)
    // to flux density units (W/m3/sr) by taking into account the distance
    // --> divide by fourpid2
    double fourpid2 = 4.0*M_PI*_distance*_distance;

    // conversion from program SI units (at this moment W/m3/sr) to the correct output units
    // --> multiply by unit conversion factor
    double unitfactor = units->osurfacebrightness(lambdagrid->lambda(ell), 1.);

    // Perform the conversion, in place
    foreach (Array* farr, farrays)
    {
        (*farr) *= (unitfactor / (dlambda * area * fourpid2));
    }

    // Write a FITS file for each array
    for (int q = 0; q < farrays.size(); q++)
    {
        QString filename = _instrument->instrumentName() + "_" + fnames[q] + "_" + QString::number(ell);
        QString description = fnames[q] + " flux " + QString::number(ell);

        // Create the image and save it
        Image image(this, _Nxp, _Nyp, 1, _xpres, _ypres, _xpc, _ypc, "surfacebrightness");
        image.saveto(this, *(farrays[q]), filename, description);
    }
}

////////////////////////////////////////////////////////////////////
