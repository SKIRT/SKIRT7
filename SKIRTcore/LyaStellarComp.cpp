/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "FatalError.hpp"
#include "Log.hpp"
#include "LyaStellarComp.hpp"
#include "LyaWavelengthGrid.hpp"
#include "Units.hpp"

using namespace std;

//////////////////////////////////////////////////////////////////////

LyaStellarComp::LyaStellarComp()
    : _spectrum(0)
{
}

////////////////////////////////////////////////////////////////////

void LyaStellarComp::setupSelfBefore()
{
    GeometricStellarComp::setupSelfBefore();

    // verify that we have all properties
    if (!_spectrum) throw FATALERROR("emission spectrum was not set");

    // verify that the wavelength grid (and thus the simulation) is of the correct type
    LyaWavelengthGrid* lambdagrid = find<LyaWavelengthGrid>();

    // provide room for the appropriate number of luminosities (to make base class happy)
    _Lv.resize(lambdagrid->Nlambda());
}

////////////////////////////////////////////////////////////////////

void LyaStellarComp::setupSelfAfter()
{
    GeometricStellarComp::setupSelfAfter();

    // actually calculate the luminosities (we need our LyaStellarComp child to be setup for this)
    int Nlambda = find<WavelengthGrid>()->Nlambda();
    for (int ell=0; ell<Nlambda; ell++)
        _Lv[ell] = _spectrum->luminosity(ell);
}

//////////////////////////////////////////////////////////////////////

void LyaStellarComp::setSpectrum(LyaSpectrum* value)
{
    if (_spectrum) delete _spectrum;
    _spectrum = value;
    if (_spectrum) _spectrum->setParent(this);
}

//////////////////////////////////////////////////////////////////////

LyaSpectrum* LyaStellarComp::spectrum() const
{
    return _spectrum;
}

//////////////////////////////////////////////////////////////////////
