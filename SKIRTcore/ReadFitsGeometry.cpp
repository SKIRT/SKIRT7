 /*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include <cmath>
#include "ReadFitsGeometry.hpp"
#include "FatalError.hpp"
#include "Log.hpp"
#include "NR.hpp"
#include "Image.hpp"
#include "Random.hpp"

using namespace std;

////////////////////////////////////////////////////////////////////

ReadFitsGeometry::ReadFitsGeometry()
    : _deltay(0), _positionangle(0), _inclination(0), _Nx(0), _Ny(0), _xc(0), _yc(0), _hz(0)
{
}

////////////////////////////////////////////////////////////////////

void ReadFitsGeometry::setupSelfBefore()
{
    GenGeometry::setupSelfBefore();

    // Verify user-defined properties
    if (_deltay <= 0) throw FATALERROR("Pixel scale should be positive");
    if (_inclination < 0 || _inclination > M_PI/2.0) throw FATALERROR("Inclination should be between 0 and 90 degrees");
    if (_Nx <= 0) throw FATALERROR("Number of x pixels should be positive");
    if (_Ny <= 0) throw FATALERROR("Number of y pixels should be positive");
    if (_xc <= 0) throw FATALERROR("Central x position should be positive");
    if (_yc <= 0) throw FATALERROR("Central y position should be positive");
    if (_hz <= 0) throw FATALERROR("Axial scale height hz should be positive");

    // Read the input file
    _image.import(this, _filename);

    // Verify the image properties
    if (_Nx != _image.xsize()) throw FATALERROR("Number of x pixels does not correspond with the number of x pixels of the image");
    if (_Ny != _image.ysize()) throw FATALERROR("Number of y pixels does not correspond with the number of y pixels of the image");
    if (_image.numframes() != 1) throw FATALERROR("FITS image contains multiple frames");

    // Normalize the luminosities
    _image /= _image.sum();

    // Construct a vector with the normalized cumulative luminosities
    NR::cdf(_Xv, _image.data());

    // Calculate the boundaries of the image in physical coordinates
    _xmax = ((_Nx-_xc)*_deltay);
    _xmin = -_xc*_deltay;
    _ymax = ((_Ny-_yc)*_deltay);
    _ymin = -_yc*_deltay;

    // Calculate the sines and cosines of the position angle and inclination
    _cospa = cos(_positionangle);
    _sinpa = sin(_positionangle);
    _cosi = cos(_inclination);
    _sini = sin(_inclination);

    // Calculate the physical pixel size in the x direction of the galactic plane
    _deltax = _deltay / _cosi;

    // Calculate the coordinates of the 4 corners of the image in the rotated plane
    _C1x = _xmax;
    _C1y = _ymax;
    _C2x = _xmin;
    _C2y = _ymax;
    _C3x = _xmin;
    _C3y = _ymin;
    _C4x = _xmax;
    _C4y = _ymin;
    derotate(_C1x, _C1y);
    derotate(_C2x, _C2y);
    derotate(_C3x, _C3y);
    derotate(_C4x, _C4y);
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
    _deltay = value;
}

////////////////////////////////////////////////////////////////////

double ReadFitsGeometry::pixelScale() const
{
    return _deltay;
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
    _Nx = value;
}

////////////////////////////////////////////////////////////////////

int ReadFitsGeometry::xelements() const
{
    return _Nx;
}

////////////////////////////////////////////////////////////////////

void ReadFitsGeometry::setYelements(int value)
{
    _Ny = value;
}

////////////////////////////////////////////////////////////////////

int ReadFitsGeometry::yelements() const
{
    return _Ny;
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

    // Project and rotate the x and y coordinates
    project(x);
    rotate(x,y);

    // Find the corresponding pixel in the image
    int i = static_cast<int>(floor(x-_xmin)/_deltay);
    int j = static_cast<int>(floor(y-_ymin)/_deltay);
    if (i<0 || i>=_Nx || j<0 || j>=_Ny) return 0.0;

    // Return the density
    return _image(i,j) * exp(-fabs(z)/_hz) /(2*_hz)/(_deltax*_deltay);
}

////////////////////////////////////////////////////////////////////

Position
ReadFitsGeometry::generatePosition()
const
{
    // Draw a random position in the plane of the galaxy based on the
    // cumulative luminosities per pixel
    double X1 = _random->uniform();
    int k = NR::locate(_Xv,X1);
    int i = k%_Nx;
    int j = (k-i)/_Nx;

    // Determine the x and y coordinate in the plane of the image
    double x = _xmin + (i+_random->uniform())*_deltay;
    double y = _ymin + (j+_random->uniform())*_deltay;

    // Derotate and deproject the x and y coordinates
    derotate(x,y);
    deproject(x);

    // Draw a random position along the minor axis
    double X2 = _random->uniform();
    double z = (X2<=0.5) ? _hz*log(2.0*X2) : -_hz*log(2.0*(1.0-X2));

    // Return the position
    return Position(x,y,z);
}

////////////////////////////////////////////////////////////////////

double
ReadFitsGeometry::SigmaX()
const
{
    const int NSAMPLES = 10000;
    double sum = 0;

    // Find the maximum and minimum possible x value
    double xmax = max({_C1x, _C2x, _C3x, _C4x});
    double xmin = min({_C1x, _C2x, _C3x, _C4x});
    deproject(xmax);
    deproject(xmin);

    // For each position, get the density and add it to the total
    for (int k = 0; k < NSAMPLES; k++)
    {
        sum += density(Position(xmin + k*(xmax-xmin)/NSAMPLES, 0, 0));
    }

    // Return the x-axis surface density
    return (sum/NSAMPLES)*(xmax-xmin);
}

////////////////////////////////////////////////////////////////////

double
ReadFitsGeometry::SigmaY()
const
{
    const int NSAMPLES = 10000;
    double sum = 0;

    // Find the maximum and minimum possible y value
    double ymax = max({_C1y, _C2y, _C3y, _C4y});
    double ymin = min({_C1y, _C2y, _C3y, _C4y});

    // For each position, get the density and add it to the total
    for (int k = 0; k < NSAMPLES; k++)
    {
        sum += density(Position(0, ymin + k*(ymax-ymin)/NSAMPLES, 0));
    }

    // Return the y-axis surface density
    return (sum/NSAMPLES)*(ymax-ymin);
}

////////////////////////////////////////////////////////////////////

double
ReadFitsGeometry::SigmaZ()
const
{
    // Get the index of the luminosity vector for the center of the galaxy (-_xpmin,-_ypmin)
    int i = static_cast<int>(floor((-_xmin)/_deltay));
    int j = static_cast<int>(floor((-_ymin)/_deltay));

    // Return the z-axis surface density
    return _image(i,j) / (_deltay*_deltay);
}

////////////////////////////////////////////////////////////////////

void
ReadFitsGeometry::rotate(double &x, double &y)
const
{
    // Cache the original values of x and y
    double xorig = x;
    double yorig = y;

    // Calculate the coordinates in the plane of the image
    x = (_sinpa * xorig)  + (_cospa * yorig);
    y = (-_cospa * xorig) + (_sinpa * yorig);
}

////////////////////////////////////////////////////////////////////

void
ReadFitsGeometry::derotate(double &x, double &y)
const
{
    // Cache the original values of x and y
    double xorig = x;
    double yorig = y;

    // Calculate the coordinates in the rotated plane
    x = (_sinpa * xorig) - (_cospa * yorig);
    y = (_cospa * xorig) + (_sinpa * yorig);
}

////////////////////////////////////////////////////////////////////

void
ReadFitsGeometry::project(double &x)
const
{
    x = x*_cosi;
}

////////////////////////////////////////////////////////////////////

void
ReadFitsGeometry::deproject(double &x)
const
{
    x = x/_cosi;
}

////////////////////////////////////////////////////////////////////
