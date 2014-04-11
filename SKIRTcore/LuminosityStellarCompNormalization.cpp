/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include "FatalError.hpp"
#include "LuminosityStellarCompNormalization.hpp"
#include "SunSED.hpp"
#include "Units.hpp"
#include "WavelengthGrid.hpp"

using namespace std;

////////////////////////////////////////////////////////////////////

LuminosityStellarCompNormalization::LuminosityStellarCompNormalization()
    : _X(V), _ell(0), _LX_Lsun(0), _LX_Wm(0)
{
}

////////////////////////////////////////////////////////////////////

void LuminosityStellarCompNormalization::setupSelfBefore()
{
    StellarCompNormalization::setupSelfBefore();

    if (_LX_Lsun <= 0) throw FATALERROR("The luminosity in the given band should be positive");

    // remember the wavelength index corresponding to the specified band
    _ell = find<WavelengthGrid>()->nearest(effectivewavelength(_X));
    if (_ell < 0) throw FATALERROR("The given band is outside of the simulation's wavelength grid");

    // convert to program units
    _LX_Wm = _LX_Lsun * Units::bolluminositysun() * SunSED::solarluminosity(this, _ell);
}

////////////////////////////////////////////////////////////////////

void LuminosityStellarCompNormalization::setBand(LuminosityStellarCompNormalization::Band value)
{
    _X = value;
}

////////////////////////////////////////////////////////////////////

LuminosityStellarCompNormalization::Band LuminosityStellarCompNormalization::band() const
{
    return _X;
}

////////////////////////////////////////////////////////////////////

void LuminosityStellarCompNormalization::setLuminosity(double value)
{
    _LX_Lsun = value;
}

////////////////////////////////////////////////////////////////////

double LuminosityStellarCompNormalization::luminosity() const
{
    return _LX_Lsun;
}

////////////////////////////////////////////////////////////////////

double LuminosityStellarCompNormalization::totluminosity(SED* sed) const
{
    return _LX_Wm / sed->luminosity(_ell);
}

////////////////////////////////////////////////////////////////////

double LuminosityStellarCompNormalization::effectivewavelength(LuminosityStellarCompNormalization::Band X)
{
    switch (X)
    {
    case FUV: return 152e-9;
    case NUV: return 231e-9;
    case U: return 365e-9;
    case B: return 445e-9;
    case V: return 551e-9;
    case R: return 658e-9;
    case I: return 806e-9;
    case J: return 1.22e-6;
    case H: return 1.63e-6;
    case K: return 2.19e-6;
    default:
        throw FATALERROR("Unknown broadband");
    }
}

//////////////////////////////////////////////////////////////////////
