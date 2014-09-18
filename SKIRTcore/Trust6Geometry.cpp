/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include <cmath>
#include "FatalError.hpp"
#include "Random.hpp"
#include "SpecialFunctions.hpp"
#include "Trust6Geometry.hpp"
#include "Units.hpp"

using namespace std;

//////////////////////////////////////////////////////////////////////

Trust6Geometry::Trust6Geometry()
{
}

//////////////////////////////////////////////////////////////////////

void Trust6Geometry::setupSelfBefore()
{
    AxGeometry::setupSelfBefore();

    // set the values for the main parameters

    _Rmaxv.resize(3,0.0);
    _zminv.resize(3,0.0);
    _zmaxv.resize(3,0.0);
    _rhov.resize(3,0.0);
    _Mv.resize(3,0.0);

    double AU = Units::AU();
    _Rmaxv[0] = 400.0*AU;
    _zminv[0] = -90.0*AU;
    _zmaxv[0] = 230.0*AU;
    _rhov[0] = 1e-21;
    _Rmaxv[1] = 400.0*AU;
    _zminv[1] = -230.0*AU;
    _zmaxv[1] = -220.0*AU;
    _rhov[1] = 1e-18;
    _Rmaxv[2] = 100.0*AU;
    _zminv[2] = -80.0*AU;
    _zmaxv[2] = -70.0*AU;
    _rhov[2] = 1e-15;

    // determine the proper density normalizations factor

    for (int i=0; i<3; i++)
        _Mv[i] = _rhov[i] * M_PI * _Rmaxv[i] * _Rmaxv[i] * (_zmaxv[i]-_zminv[i]);
    double M = _Mv[0] + _Mv[1] + _Mv[2];
    for (int i=0; i<3; i++)
    {
        _rhov[i] /= M;
        _Mv[i] /= M;
    }
}

//////////////////////////////////////////////////////////////////////

double
Trust6Geometry::density(double R, double z)
const
{
    double rho = 0.0;
    for (int i=0; i<3; i++)
        if (R<_Rmaxv[i] && z>_zminv[i] && z<_zmaxv[i]) rho += _rhov[i];
    return rho;
}

//////////////////////////////////////////////////////////////////////

Position
Trust6Geometry::generatePosition()
const
{
    // determine the component from with the position is generated

    double X = _random->uniform();
    int i = 0;
    if (X<_Mv[0])
        i = 0;
    else if (X<_Mv[0]+_Mv[1])
        i = 1;
    else
        i = 2;

    // determine the actual position

    double R = _Rmaxv[i] * sqrt(_random->uniform());
    double phi = 2.0*M_PI*_random->uniform();
    double z = _zminv[i] + _random->uniform()*(_zmaxv[i]-_zminv[i]);
    return Position(R, phi, z, Position::CYLINDRICAL);
}

//////////////////////////////////////////////////////////////////////

double
Trust6Geometry::SigmaR()
const
{
    double Sigma = 0.0;
    for (int i=0; i<3; i++)
        if (_zminv[i]<0.0 && _zmaxv[i]>0.0) Sigma += _rhov[i]*_Rmaxv[i];
    return Sigma;
}

//////////////////////////////////////////////////////////////////////

double
Trust6Geometry::SigmaZ()
const
{
    double Sigma = 0.0;
    for (int i=0; i<3; i++)
        Sigma += _rhov[i] * (_zmaxv[i]-_zminv[i]);
    return Sigma;
}

//////////////////////////////////////////////////////////////////////
