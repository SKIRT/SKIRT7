 /*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include <cmath>
#include <iostream>
#include <valarray>
#include <vector>
#include "FilePaths.hpp"
#include "Log.hpp"
#include "ReadFitsGeometry.hpp"
#include "FITSInOut.hpp"
#include "FatalError.hpp"
#include "Random.hpp"
#include "NR.hpp"
#include "SpecialFunctions.hpp"

using namespace std;

////////////////////////////////////////////////////////////////////

ReadFitsGeometry::ReadFitsGeometry()
    : _pix(0), _pa(0), _incl(0), _nx(0), _ny(0), _xc(0), _yc(0), _hz(0)
{
}

////////////////////////////////////////////////////////////////////

void ReadFitsGeometry::setupSelfBefore()
{
    GenGeometry::setupSelfBefore();

    // Verify property values
    if (_pix <= 0) throw FATALERROR("Pixel scale should be positive");
    if (_incl < 0) throw FATALERROR("Inclination should be between 0 and 90 degrees");
    if (_incl > 180) throw FATALERROR("Inclination should be between 0 and 90 degrees");
    if (_nx < 0) throw FATALERROR("Number of x pixels should be positive");
    if (_ny < 0) throw FATALERROR("Number of y pixels should be positive");
    if (_xc < 0) throw FATALERROR("Central x position should be positive");
    if (_yc < 0) throw FATALERROR("Central y position should be positive");
    if (_hz <= 0) throw FATALERROR("Axial scale height hz should be positive");

    QString filename = FilePaths::resource(_filename);
    Array fitsImage;
    int _nz = 1;

    // Read the input file
    find<Log>()->info("Reading FITS file");
    QString filepath = find<FilePaths>()->input(_filename);
    FITSInOut::read(filepath,fitsImage,_nx,_ny,_nz);

    _xpmax = ((_nx-_xc)*_pix);
    _xpmin = -_xc*_pix;
    _ypmax = ((_ny-_yc)*_pix);
    _ypmin = -_yc*_pix;

    find<Log>()->info("Reading FITS file:OK");

    // Construct a vector with the normalized cumulative luminosities
    _Lv.resize(_nx*_ny);

    for (int k=0; k<_nx*_ny; k++)
        _Lv[k] = fitsImage[k];

    _Xv.resize(_nx*_ny+1);

    double sum = 0.0;

    for (int k=0; k<=_nx*_ny; ++k)
        _Xv[k] = (k==0) ? 0.0 : _Xv[k-1]+_Lv[k-1];

    for (int k=0; k<_nx*_ny; k++)
        sum += _Lv[k];

    for (int k=0; k<_nx*_ny; k++)
        _Lv[k] /= sum;

    for (int k=0; k<=_nx*_ny; k++)
        _Xv[k] /= sum;
}

////////////////////////////////////////////////////////////////////

void ReadFitsGeometry::setFilename(QString value)
{
    _filename = value;
}

////////////////////////////////////////////////////////////////////

QString ReadFitsGeometry::filename() const
{
    return _filename;
}

////////////////////////////////////////////////////////////////////

void ReadFitsGeometry::setPixelScale(double value)
{
    _pix = value;
}

////////////////////////////////////////////////////////////////////

double ReadFitsGeometry::pixelScale() const
{
    return _pix;
}

////////////////////////////////////////////////////////////////////

void ReadFitsGeometry::setPositionAngle(double value)
{
    _pa = value;
}

////////////////////////////////////////////////////////////////////

double ReadFitsGeometry::positionAngle() const
{
    return _pa;
}

////////////////////////////////////////////////////////////////////

void ReadFitsGeometry::setInclination(double value)
{
    _incl = value;
}

////////////////////////////////////////////////////////////////////

double ReadFitsGeometry::inclination() const
{
    return _incl;
}

////////////////////////////////////////////////////////////////////

void ReadFitsGeometry::setAxialScale(double value)
{
    _hz = value;
}

////////////////////////////////////////////////////////////////////

double ReadFitsGeometry::axialScale() const
{
    return _hz;
}
////////////////////////////////////////////////////////////////////

void ReadFitsGeometry::setXelements(int value)
{
    _nx = value;
}

////////////////////////////////////////////////////////////////////

int ReadFitsGeometry::xelements() const
{
    return _nx;
}
////////////////////////////////////////////////////////////////////

void ReadFitsGeometry::setYelements(int value)
{
    _ny = value;
}

////////////////////////////////////////////////////////////////////

int ReadFitsGeometry::yelements() const
{
    return _ny;
}
////////////////////////////////////////////////////////////////////

void ReadFitsGeometry::setXcenter(double value)
{
    _xc = value;
}

////////////////////////////////////////////////////////////////////

double ReadFitsGeometry::xcenter() const
{
    return _xc;
}
////////////////////////////////////////////////////////////////////

void ReadFitsGeometry::setYcenter(double value)
{
    _yc = value;
}

////////////////////////////////////////////////////////////////////

double ReadFitsGeometry::ycenter() const
{
    return _yc;
}

////////////////////////////////////////////////////////////////////

double
ReadFitsGeometry::density(Position bfr)
const
{
    double x,y,z;
    bfr.cartesian(x,y,z);

    // Calculate xp and yp based on x, y, _pa, _inclination and _pixelscale
    double cospa = cos(_pa);
    double sinpa = sin(_pa);
    double cosi = cos(_incl);
    double sini = sin(_incl);
    double xp = (sinpa*cosi*x) + (cospa*y) - (sinpa*sini*z);
    double yp =  - (cospa*cosi*x) + (sinpa*y) + (cospa*sini*z);

    if ( (xp<_xpmin) || (xp>_xpmax)
        || (yp<_ypmin) || (yp>_ypmax) ) return 0.0;
    int i = static_cast<int>(floor(xp-_xpmin)/_pix);
    int j = static_cast<int>(floor(yp-_ypmin)/_pix);
    int k = j*_nx + i;

    return _Lv[k] * exp(-fabs(z)/_hz) /(2*_hz)/(_pix*_pix);
}

////////////////////////////////////////////////////////////////////

Position
ReadFitsGeometry::generatePosition()
const
{
    double X = _random->uniform();
    int k = NR::locate(_Xv,X);
    int i = k%_nx;
    int j = (k-i)/_nx;
    double xp = _xpmin + (i+_random->uniform())*_pix;
    double yp = _ypmin + (j+_random->uniform())*_pix;
    double cospa = cos(_pa);
    double sinpa = sin(_pa);

    double x = (sinpa*xp) - (cospa*yp);
    double y = (cospa*xp) + (sinpa*yp);
    X = _random->uniform();
    double z = 0.0;
    if (X<=0.5)
    {
        z = _hz*log(2.0*X);
        return Position(x,y,z);
    }
    else
    {
        z = - _hz*log(2.0*(1.0-X));
        return Position(x,y,z);
    }

    return Position();
}

////////////////////////////////////////////////////////////////////

double
ReadFitsGeometry::SigmaX()
const
{
    double sum = 0.0;

    for (int l=0; l<_nx; l++)
    {
        double xp = _xpmin + (l*_pix);
        double yp = 0.0;
        int i = static_cast<int>(floor((xp-_xpmin)/_pix));
        int j = static_cast<int>(floor((yp-_ypmin)/_pix));
        int k = j*_nx + i;
        sum += _Lv[k];
    }

    return sum / (2.0*_hz)/_pix;
}

////////////////////////////////////////////////////////////////////

double
ReadFitsGeometry::SigmaY()
const
{
    double sum = 0.0;

    for (int l=0; l<_ny; l++)
    {
        double xp = 0.0;
        double yp =_ypmin + (l*_pix);
        int i = static_cast<int>(floor((xp-_xpmin)/_pix));
        int j = static_cast<int>(floor((yp-_ypmin)/_pix));
        int k = j*_nx + i;
        sum += _Lv[k];
    }

    return sum / (2.0*_hz)/_pix;
}

////////////////////////////////////////////////////////////////////

double
ReadFitsGeometry::SigmaZ()
const
{
    double cospa = cos(_pa);
    double sinpa = sin(_pa);
    double cosi = cos(_incl);

    double xp = (cospa*0) - (sinpa*0*cosi);
    double yp = (sinpa*0) + (cospa*0*cosi);

    int i = static_cast<int>(floor((xp-_xpmin)/_pix));
    int j = static_cast<int>(floor((yp-_ypmin)/_pix));
    int k = j*_nx + i;

    return _Lv[k]/(_pix*_pix);
}

////////////////////////////////////////////////////////////////////
