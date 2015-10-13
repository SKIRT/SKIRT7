/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "LineLyaSpectrum.hpp"
#include "FatalError.hpp"
#include "NR.hpp"
#include "Units.hpp"
#include "WavelengthGrid.hpp"

using namespace std;

//////////////////////////////////////////////////////////////////////

LineLyaSpectrum::LineLyaSpectrum()
    : _v(0), _L(0)
{
}

////////////////////////////////////////////////////////////////////

void LineLyaSpectrum::setupSelfBefore()
{
    LyaSpectrum::setupSelfBefore();

    // verify property values
    if (_L <= 0) throw FATALERROR("the continuum level should be positive");

    WavelengthGrid* lambdagrid = find<WavelengthGrid>();
    Units* units = find<Units>();
    double lambda0 = units->lambdaLya();
    double c = units->c();
    double Nlambda = lambdagrid->Nlambda();
    Array Lv(Nlambda);
    for (int ell=0; ell<Nlambda; ell++) Lv[ell] = 0;
    double lambdaline = lambda0*(1.0-_v/c);
    int ell = NR::locate_fail(lambdagrid->lambdav(),lambdaline);
    if (ell==-1) throw FATALERROR("the line shift does not fit within the wavelength grid");
    Lv[ell] = _L;
    setluminosities(Lv);
}

////////////////////////////////////////////////////////////////////

void LineLyaSpectrum::setVelocityShift(double value)
{
    _v = value;
}

////////////////////////////////////////////////////////////////////

double LineLyaSpectrum::velocityShift() const
{
    return _v;
}

////////////////////////////////////////////////////////////////////

void LineLyaSpectrum::setLineLuminosity(double value)
{
    _L = value;
}

////////////////////////////////////////////////////////////////////

double LineLyaSpectrum::lineLuminosity() const
{
    return _L;
}

//////////////////////////////////////////////////////////////////////
