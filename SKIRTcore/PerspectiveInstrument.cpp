/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include <cmath>
#include "PerspectiveInstrument.hpp"
#include "FatalError.hpp"
#include "FilePaths.hpp"
#include "FITSInOut.hpp"
#include "LockFree.hpp"
#include "Log.hpp"
#include "PeerToPeerCommunicator.hpp"
#include "PhotonPackage.hpp"
#include "Units.hpp"
#include "WavelengthGrid.hpp"

using namespace std;

////////////////////////////////////////////////////////////////////

namespace
{
    double norm (double x, double y, double z)
    {
        return sqrt(x*x + y*y + z*z);
    }
}

////////////////////////////////////////////////////////////////////

PerspectiveInstrument::PerspectiveInstrument()
    : _Nx(0), _Ny(0), _Sx(0), _Vx(0), _Vy(0), _Vz(0), _Cx(0), _Cy(0), _Cz(0), _Ux(0), _Uy(0), _Uz(0), _Fe(0)
{
}

////////////////////////////////////////////////////////////////////

void PerspectiveInstrument::setupSelfBefore()
{
    Instrument::setupSelfBefore();

    // verify attribute values
    if (_Nx <= 0 || _Ny <= 0) throw FATALERROR("Number of pixels was not set");
    if (_Sx <= 0) throw FATALERROR("Viewport width was not set");
    if (_Ux == 0 && _Uy == 0 && _Uz == 0) throw FATALERROR("Upwards direction was not set");
    if (_Fe <= 0) throw FATALERROR("Focal length was not set");

    // unit vector in direction from crosshair position to viewport origin
    double G = norm(_Vx - _Cx, _Vy - _Cy, _Vz - _Cz);
    if (G < 1e-20) throw FATALERROR("Crosshair is too close to viewport origin");
    double a = (_Vx - _Cx) / G;
    double b = (_Vy - _Cy) / G;
    double c = (_Vz - _Cz) / G;

    // pixel width and height (assuming square pixels)
    _s = _Sx/_Nx;

    // eye position
    _Ex = _Vx + _Fe * a;
    _Ey = _Vy + _Fe * b;
    _Ez = _Vz + _Fe * c;

    // unit vectors along viewport's x and y axes
    Vec kn(_Vx-_Cx, _Vy-_Cy, _Vz-_Cz);
    Vec ku(_Ux, _Uy, _Uz);
    Vec ky = Vec::cross(kn,Vec::cross(ku,kn));
    Vec kx = Vec::cross(ky,kn);
    _bfkx = Direction(kx/kx.norm());
    _bfky = Direction(ky/ky.norm());

    // the perspective transformation

    // from world to eye coordinates
    _transform.translate(-_Ex, -_Ey, -_Ez);
    double v = sqrt(b*b+c*c);
    if (v > 0.3)
    {
        _transform.rotateX(c/v, -b/v);
        _transform.rotateY(v, -a);
        double k = (b*b+c*c)*_Ux - a*b*_Uy - a*c*_Uz;
        double l = c*_Uy - b*_Uz;
        double u = sqrt(k*k+l*l);
        _transform.rotateZ(l/u, -k/u);
    }
    else
    {
        v = sqrt(a*a+c*c);
        _transform.rotateY(c/v, -a/v);
        _transform.rotateX(v, -b);
        double k = c*_Ux - a*_Uz;
        double l = (a*a+c*c)*_Uy - a*b*_Ux - b*c*_Uz;
        double u = sqrt(k*k+l*l);
        _transform.rotateZ(l/u, -k/u);
    }
    _transform.scale(1., 1., -1.);

    // from eye to viewport coordinates
    _transform.perspectiveZ(_Fe);

    // from viewport to pixel coordinates
    _transform.scale(1./_s, 1./_s, 1.);
    _transform.translate(_Nx/2., _Ny/2., 0);

    // the data cube
    int Nlambda = find<WavelengthGrid>()->Nlambda();
    _ftotv.resize(Nlambda*_Nx*_Ny);
}

////////////////////////////////////////////////////////////////////

void PerspectiveInstrument::setPixelsX(int value)
{
    _Nx = value;
}

////////////////////////////////////////////////////////////////////

int PerspectiveInstrument::pixelsX() const
{
    return _Nx;
}

////////////////////////////////////////////////////////////////////

void PerspectiveInstrument::setPixelsY(int value)
{
    _Ny = value;
}

////////////////////////////////////////////////////////////////////

int PerspectiveInstrument::pixelsY() const
{
    return _Ny;
}

////////////////////////////////////////////////////////////////////

void PerspectiveInstrument::setWidth(double value)
{
    _Sx = value;
}

////////////////////////////////////////////////////////////////////

double PerspectiveInstrument::width() const
{
    return _Sx;
}

////////////////////////////////////////////////////////////////////

void PerspectiveInstrument::setViewX(double value)
{
    _Vx = value;
}

////////////////////////////////////////////////////////////////////

double PerspectiveInstrument::viewX() const
{
    return _Vx;
}

////////////////////////////////////////////////////////////////////

void PerspectiveInstrument::setViewY(double value)
{
    _Vy = value;
}

////////////////////////////////////////////////////////////////////

double PerspectiveInstrument::viewY() const
{
    return _Vy;
}

////////////////////////////////////////////////////////////////////

void PerspectiveInstrument::setViewZ(double value)
{
    _Vz = value;
}

////////////////////////////////////////////////////////////////////

double PerspectiveInstrument::viewZ() const
{
    return _Vz;
}

////////////////////////////////////////////////////////////////////

void PerspectiveInstrument::setCrossX(double value)
{
    _Cx = value;
}

////////////////////////////////////////////////////////////////////

double PerspectiveInstrument::crossX() const
{
    return _Cx;
}

////////////////////////////////////////////////////////////////////

void PerspectiveInstrument::setCrossY(double value)
{
    _Cy = value;
}

////////////////////////////////////////////////////////////////////

double PerspectiveInstrument::crossY() const
{
    return _Cy;
}

////////////////////////////////////////////////////////////////////

void PerspectiveInstrument::setCrossZ(double value)
{
    _Cz = value;
}

////////////////////////////////////////////////////////////////////

double PerspectiveInstrument::crossZ() const
{
    return _Cz;
}

////////////////////////////////////////////////////////////////////

void PerspectiveInstrument::setUpX(double value)
{
    _Ux = value;
}

////////////////////////////////////////////////////////////////////

double PerspectiveInstrument::upX() const
{
    return _Ux;
}

////////////////////////////////////////////////////////////////////

void PerspectiveInstrument::setUpY(double value)
{
    _Uy = value;
}

////////////////////////////////////////////////////////////////////

double PerspectiveInstrument::upY() const
{
    return _Uy;
}

////////////////////////////////////////////////////////////////////

void PerspectiveInstrument::setUpZ(double value)
{
    _Uz = value;
}

////////////////////////////////////////////////////////////////////

double PerspectiveInstrument::upZ() const
{
    return _Uz;
}

////////////////////////////////////////////////////////////////////

void PerspectiveInstrument::setFocal(double value)
{
    _Fe = value;
}

////////////////////////////////////////////////////////////////////

double PerspectiveInstrument::focal() const
{
    return _Fe;
}

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

Direction PerspectiveInstrument::bfkobs(const Position& bfr) const
{
    // distance from launch to eye
    double Px, Py, Pz;
    bfr.cartesian(Px, Py, Pz);
    double D = norm(_Ex - Px, _Ey - Py, _Ez - Pz);

    // if the distance is very small, return something silly - the photon package is behind the viewport anyway
    if (D < 1e-20) return Direction();

    // otherwise return a unit vector in the direction from launch to eye
    return Direction((_Ex - Px)/D, (_Ey - Py)/D, (_Ez - Pz)/D);
}

////////////////////////////////////////////////////////////////////

Direction PerspectiveInstrument::bfkx() const
{
    return _bfkx;
}

////////////////////////////////////////////////////////////////////

Direction PerspectiveInstrument::bfky() const
{
    return _bfky;
}

////////////////////////////////////////////////////////////////////

void PerspectiveInstrument::detect(PhotonPackage* pp)
{
    // get the position
    double x, y, z;
    pp->position().cartesian(x,y,z);

    // transform from world to pixel coordinates
    double xp, yp, zp, wp;
    _transform.transform(x, y, z, 1.,  xp, yp, zp, wp);
    int i = xp/wp;
    int j = yp/wp;

    // ignore photon packages arriving outside the viewport, originating from behind the viewport,
    // or originating from very close to the viewport
    if (i>=0 && i<_Nx && j>=0 && j<_Ny && zp > _s/10.)
    {
        double d = zp;

        // determine the photon package's luminosity, attenuated for the absorption along its path to the instrument
        double taupath = opticalDepth(pp,d);
        double L = pp->luminosity() * exp(-taupath);

        // adjust the luminosity for the distance from the launch position to the instrument
        double r = _s / (2.*d);
        double rar = r / atan(r);
        L *= rar*rar;

        // add the adjusted luminosity to the appropriate pixel in the data cube
        int ell = pp->ell();
        int m = i + _Nx*j + _Nx*_Ny*ell;
        LockFree::add(_ftotv[m], L);
    }
}

////////////////////////////////////////////////////////////////////

void PerspectiveInstrument::write()
{
    Units* units = find<Units>();
    WavelengthGrid* lambdagrid = find<WavelengthGrid>();
    int Nlambda = find<WavelengthGrid>()->Nlambda();

    // Put the data cube in a list of f-array pointers, as the sumResults function requires
    QList< Array* > farrays;
    farrays << &_ftotv;

    // Sum the flux arrays element-wise across the different processes
    sumResults(farrays);

    // From here on, only the root process should continue
    PeerToPeerCommunicator* comm = find<PeerToPeerCommunicator>();
    if (comm->rank()) return;

    // multiply each sample by lambda/dlamdba and by the constant factor 1/(4 pi s^2)
    // to obtain the surface brightness and convert to output units (such as W/m2/arcsec2)

    double front = 1. / (4.*M_PI*_s*_s);
    for (int ell=0; ell<Nlambda; ell++)
    {
        double lambda = lambdagrid->lambda(ell);
        double dlambda = lambdagrid->dlambda(ell);
        for (int i=0; i<_Nx; i++)
        {
            for (int j=0; j<_Ny; j++)
            {
                int m = i + _Nx*j + _Nx*_Ny*ell;
                _ftotv[m] = units->osurfacebrightness(lambda, _ftotv[m]*front/dlambda);
            }
        }
    }

    // write a FITS file containing the data cube

    QString filename = find<FilePaths>()->output(_instrumentname + "_total.fits");
    find<Log>()->info("Writing total flux to FITS file " + filename + "...");
    FITSInOut::write(filename, _ftotv, _Nx, _Ny, Nlambda,
                   units->olength(_s), units->olength(_s),
                   units->usurfacebrightness(), units->ulength());
}

////////////////////////////////////////////////////////////////////
