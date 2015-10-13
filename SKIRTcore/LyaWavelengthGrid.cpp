/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "FatalError.hpp"
#include "NR.hpp"
#include "LyaWavelengthGrid.hpp"
#include "Units.hpp"

using namespace std;

////////////////////////////////////////////////////////////////////

LyaWavelengthGrid::LyaWavelengthGrid()
{
}

////////////////////////////////////////////////////////////////////

void LyaWavelengthGrid::setupSelfBefore()
{
    WavelengthGrid::setupSelfBefore();

    Units* units = find<Units>();
    double lambda0 = units->lambdaLya();
    double c = units->c();
    Array vv;
    NR::lingrid(vv, _vmin, _vmax, _Nlambda-1);
    _lambdav.resize(_Nlambda);
    for (int ell=0; ell<_Nlambda; ell++)
        _lambdav[ell] = lambda0 * (1.0-vv[_Nlambda-1-ell]/c);

    // calculate the wavelength bin widths (very small and independent of the other wavelengths)
    _dlambdav.resize(_Nlambda);
    for (int ell = 0; ell < _Nlambda; ell++)
        _dlambdav[ell] = 0.001*_lambdav[ell];
}

////////////////////////////////////////////////////////////////////

void LyaWavelengthGrid::setMinVelocity(double value)
{
    _vmin = value;
}

////////////////////////////////////////////////////////////////////

double LyaWavelengthGrid::minVelocity() const
{
    return _vmin;
}

////////////////////////////////////////////////////////////////////

void LyaWavelengthGrid::setMaxVelocity(double value)
{
    _vmax = value;
}

////////////////////////////////////////////////////////////////////

double LyaWavelengthGrid::maxVelocity() const
{
    return _vmax;
}

////////////////////////////////////////////////////////////////////

void LyaWavelengthGrid::setPoints(int value)
{
    _Nlambda = value;
}

////////////////////////////////////////////////////////////////////

int LyaWavelengthGrid::points() const
{
    return _Nlambda;
}

//////////////////////////////////////////////////////////////////////

bool LyaWavelengthGrid::issampledrange() const
{
    return false;
}

//////////////////////////////////////////////////////////////////////
