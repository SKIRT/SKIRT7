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
    : _pix(0), _positionangle(0), _inclination(0), _nx(0), _ny(0), _xc(0), _yc(0), _hz(0)
{
}

////////////////////////////////////////////////////////////////////

void ReadFitsGeometry::setupSelfBefore()
{
    GenGeometry::setupSelfBefore();

    // Read the input file
    find<Log>()->info("Reading FITS file");
    int nx = 0, ny = 0, nz = 0;
    QString filepath = find<FilePaths>()->input(_filename);
    FITSInOut::read(filepath, _Lv, nx, ny, nz);

    // Verify property values
    if (_pix <= 0) throw FATALERROR("Pixel scale should be positive");
    if (_inclination < 0 || _inclination > 180) throw FATALERROR("Inclination should be between 0 and 180 degrees");
    if (_nx <= 0) throw FATALERROR("Number of x pixels should be positive");
    if (_ny <= 0) throw FATALERROR("Number of y pixels should be positive");
    if (_xc <= 0) throw FATALERROR("Central x position should be positive");
    if (_yc <= 0) throw FATALERROR("Central y position should be positive");
    if (_hz <= 0) throw FATALERROR("Axial scale height hz should be positive");
    if (_nx != nx) throw FATALERROR("Number of x pixels does not correspond with the number of x pixels of the image");
    if (_ny != ny) throw FATALERROR("Number of y pixels does not correspond with the number of y pixels of the image");
    if (nz != 1) throw FATALERROR("FITS image contains multiple frames");

    // Normalize the luminosities
    _Lv /= _Lv.sum();

    // Construct a vector with the normalized cumulative luminosities
    NR::cdf(_Xv, _Lv);

    // Calculate useful quantities
    _xpmax = ((_nx-_xc)*_pix);
    _xpmin = -_xc*_pix;
    _ypmax = ((_ny-_yc)*_pix);
    _ypmin = -_yc*_pix;
    _cospa = cos(_positionangle);
    _sinpa = sin(_positionangle);
    _cosi = cos(_inclination);
    _sini = sin(_inclination);
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
    _positionangle = value;
}

////////////////////////////////////////////////////////////////////

double ReadFitsGeometry::positionAngle() const
{
    return _positionangle;
}

////////////////////////////////////////////////////////////////////

void ReadFitsGeometry::setInclination(double value)
{
    _inclination = value;
}

////////////////////////////////////////////////////////////////////

double ReadFitsGeometry::inclination() const
{
    return _inclination;
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

    // Calculate xp and yp based on x, y, _positionangle and _inclination
    double xp = (_sinpa * _cosi * x) + (_cospa * y) - (_sinpa * _sini * z);
    double yp =  - (_cospa * _cosi * x) + (_sinpa * y) + (_cospa * _sini * z);

    if ( (xp<_xpmin) || (xp>_xpmax) || (yp<_ypmin) || (yp>_ypmax) ) return 0.0;

    // Find the corresponding pixel in the image
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

    double x = (_sinpa * xp) - (_cospa * yp);
    double y = (_cospa * xp) + (_sinpa * yp);
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
    double xp = 0.0;
    double yp = 0.0;

    int i = static_cast<int>(floor((xp-_xpmin)/_pix));
    int j = static_cast<int>(floor((yp-_ypmin)/_pix));
    int k = j*_nx + i;

    return _Lv[k]/(_pix*_pix);
}

////////////////////////////////////////////////////////////////////
