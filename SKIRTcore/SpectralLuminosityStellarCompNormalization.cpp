/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "FatalError.hpp"
#include "SpectralLuminosityStellarCompNormalization.hpp"
#include "SunSED.hpp"
#include "Units.hpp"
#include "WavelengthGrid.hpp"

using namespace std;

////////////////////////////////////////////////////////////////////

SpectralLuminosityStellarCompNormalization::SpectralLuminosityStellarCompNormalization()
    : _lambda(0), _Llambda(0), _dlambda(0), _ell(0)
{
}

////////////////////////////////////////////////////////////////////

void SpectralLuminosityStellarCompNormalization::setupSelfBefore()
{
    StellarCompNormalization::setupSelfBefore();

    if (_Llambda <= 0) throw FATALERROR("The spectral luminosity at the given wavelength should be positive");

    // remember the wavelength index corresponding to the specified wavelength
    _ell = find<WavelengthGrid>()->nearest(_lambda);
    if (_ell < 0) throw FATALERROR("The given wavelength is outside of the simulation's wavelength grid");

    // remember the width of the corresponding wavelength bin
    WavelengthGrid* lambdagrid = find<WavelengthGrid>();
    _dlambda = lambdagrid->dlambda(_ell);
}

////////////////////////////////////////////////////////////////////

void SpectralLuminosityStellarCompNormalization::setWavelength(double value)
{
    _lambda = value;
}

////////////////////////////////////////////////////////////////////

double SpectralLuminosityStellarCompNormalization::wavelength() const
{
    return _lambda;
}

////////////////////////////////////////////////////////////////////

void SpectralLuminosityStellarCompNormalization::setLuminosity(double value)
{
    _Llambda = value;
}

////////////////////////////////////////////////////////////////////

double SpectralLuminosityStellarCompNormalization::luminosity() const
{
    return _Llambda;
}

////////////////////////////////////////////////////////////////////

double SpectralLuminosityStellarCompNormalization::totluminosity(SED* sed) const
{
    return _Llambda * _dlambda / sed->luminosity(_ell);
}

//////////////////////////////////////////////////////////////////////
