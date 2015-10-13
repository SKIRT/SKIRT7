/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "FatalError.hpp"
#include "LyaSpectrum.hpp"
#include "Units.hpp"
#include "WavelengthGrid.hpp"

using namespace std;

//////////////////////////////////////////////////////////////////////

LyaSpectrum::LyaSpectrum()
{
}

//////////////////////////////////////////////////////////////////////

void LyaSpectrum::setupSelfAfter()
{
    SimulationItem::setupSelfAfter();
    size_t Nlambda = find<WavelengthGrid>()->Nlambda();
    if (_Lv.size() != Nlambda)
        throw FATALERROR("The luminosities in the Lyα emission spectrum have not been properly set");
}

//////////////////////////////////////////////////////////////////////

double LyaSpectrum::luminosity(int ell) const
{
    return _Lv[ell];
}

//////////////////////////////////////////////////////////////////////

const Array& LyaSpectrum::luminosities() const
{
    return _Lv;
}

//////////////////////////////////////////////////////////////////////

void LyaSpectrum::setluminosities(const Array &Lv)
{
    _Lv = Lv;
    double sum = _Lv.sum();
    if (sum<=0) throw FATALERROR("The total luminosity in the Lyα emission spectrum is zero or negative (" + QString::number(sum) + ")");
}

//////////////////////////////////////////////////////////////////////
