/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "BolLuminosityStellarCompNormalization.hpp"
#include "FatalError.hpp"
#include "Units.hpp"

////////////////////////////////////////////////////////////////////

BolLuminosityStellarCompNormalization::BolLuminosityStellarCompNormalization()
    : _Ltot_Lsun(0), _Ltot_W(0)
{
}

////////////////////////////////////////////////////////////////////

void BolLuminosityStellarCompNormalization::setupSelfBefore()
{
    StellarCompNormalization::setupSelfBefore();

    if (_Ltot_Lsun <= 0) throw FATALERROR("The bolometric luminosity should be positive");

    // convert to program units
    _Ltot_W = _Ltot_Lsun * Units::Lsun();
}

////////////////////////////////////////////////////////////////////

void BolLuminosityStellarCompNormalization::setLuminosity(double value)
{
    _Ltot_Lsun = value;
}

////////////////////////////////////////////////////////////////////

double BolLuminosityStellarCompNormalization::luminosity() const
{
    return _Ltot_Lsun;
}

////////////////////////////////////////////////////////////////////

double BolLuminosityStellarCompNormalization::totluminosity(SED* /*sed*/) const
{
    return _Ltot_W;
}

//////////////////////////////////////////////////////////////////////
