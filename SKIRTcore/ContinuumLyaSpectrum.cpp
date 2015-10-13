/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "ContinuumLyaSpectrum.hpp"
#include "FatalError.hpp"
#include "WavelengthGrid.hpp"

using namespace std;

//////////////////////////////////////////////////////////////////////

ContinuumLyaSpectrum::ContinuumLyaSpectrum()
    : _Llambda(0)
{
}

////////////////////////////////////////////////////////////////////

void ContinuumLyaSpectrum::setupSelfBefore()
{
    LyaSpectrum::setupSelfBefore();

    // verify property values
    if (_Llambda <= 0) throw FATALERROR("the continuum level should be positive");

    WavelengthGrid* lambdagrid = find<WavelengthGrid>();
    double Nlambda = lambdagrid->Nlambda();
    Array Lv(Nlambda);
    for (int ell=0; ell<Nlambda; ell++)
        Lv[ell] = _Llambda * lambdagrid->dlambda(ell);  // monochromatic to bolometric luminosity
    setluminosities(Lv);
}

////////////////////////////////////////////////////////////////////

void ContinuumLyaSpectrum::setLevel(double value)
{
    _Llambda = value;
}

////////////////////////////////////////////////////////////////////

double ContinuumLyaSpectrum::level() const
{
    return _Llambda;
}

//////////////////////////////////////////////////////////////////////
