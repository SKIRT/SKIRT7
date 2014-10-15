/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include <cmath>
#include "StokesVector.hpp"

using namespace std;

//////////////////////////////////////////////////////////////////////

void StokesVector::setStokes(double I, double Q, double U, double V)
{
    if (I!=0.0)
    {
        _Q = Q/I;
        _U = U/I;
        _V = V/I;
    }
    else
    {
        _Q = 0;
        _U = 0;
        _V = 0;
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

void StokesVector::rotateStokes(double alpha)
{
    double cosalpha = cos(2.0*alpha);
    double sinalpha = sin(2.0*alpha);
    double Q = cosalpha*_Q - sinalpha*_U;
    double U = sinalpha*_Q + cosalpha*_U;
    _Q = Q;
    _U = U;
}

//////////////////////////////////////////////////////////////////////

void StokesVector::applyMueller(double S11, double S12, double S33, double S34)
{
    double I =  S11*1. + S12*_Q;
    double Q =  S12*1. + S11*_Q;
    double U =  S33*_U + S34*_V;
    double V = -S34*_U + S33*_V;
    setStokes(I, Q, U, V);
}

//////////////////////////////////////////////////////////////////////
