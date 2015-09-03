/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include <cmath>
#include "StokesVector.hpp"

using namespace std;

//////////////////////////////////////////////////////////////////////

void StokesVector::setPolarized(double I, double Q, double U, double V, Direction n)
{
    if (I!=0.0)
    {
        _Q = Q/I;
        _U = U/I;
        _V = V/I;
        _normal = n;
        _polarized = true;
    }
    else
    {
        setUnpolarized();
    }
}

//////////////////////////////////////////////////////////////////////

double StokesVector::totalPolarizationDegree() const
{
    return sqrt(_Q*_Q+_U*_U+_V*_V);
}

//////////////////////////////////////////////////////////////////////

double StokesVector::linearPolarizationDegree() const
{
    return sqrt(_Q*_Q+_U*_U);
}

//////////////////////////////////////////////////////////////////////

double StokesVector::polarizationAngle() const
{
    if (_U==0 && _Q==0)
        return 0;
    else
        return 0.5*atan2(_U,_Q);
}

//////////////////////////////////////////////////////////////////////

void StokesVector::rotateStokes(double phi, Direction k)
{
    // if this is the first scattering: generate normal to scattering plane
    if (!_polarized)
    {
        double kx, ky, kz, nx, ny, nz;
        k.cartesian(kx, ky, kz);
        // this is the Bianchi formula as used in Direction Random::direction(Direction bfk, double costheta)
        // with phi = 0 and theta = 90 deg.
        if(kz*kz > 0.999999)
        {
            nx = 1;
        }
        else
        {
            nz = sqrt((1.0-kz)*(1.0+kz));
            nx = -kx*kz/nz;
            ny = -ky*kz/nz;
        }
        _normal.set(nx, ny, nz);
        _polarized = true;
    }

    // rotate the Q and U in the new reference frame
    double cos2phi = cos(2.0*phi);
    double sin2phi = sin(2.0*phi);
    double Q =  cos2phi*_Q + sin2phi*_U;
    double U = -sin2phi*_Q + cos2phi*_U;
    _Q = Q;
    _U = U;

    // rotate the stored scattering plane to obtain the new scattering plane
    _normal = Direction(_normal*cos(phi) + Vec::cross(k, _normal)*sin(phi));

}

//////////////////////////////////////////////////////////////////////

void StokesVector::applyMueller(double S11, double S12, double S33, double S34)
{
    double I =  S11*1. + S12*_Q;
    double Q =  S12*1. + S11*_Q;
    double U =  S33*_U + S34*_V;
    double V = -S34*_U + S33*_V;
    setPolarized(I, Q, U, V, _normal);
}

//////////////////////////////////////////////////////////////////////
