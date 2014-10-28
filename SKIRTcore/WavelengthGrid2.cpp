/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include <cmath>
#include "FatalError.hpp"
#include "NR.hpp"
#include "WavelengthGrid2.hpp"

using namespace std;

////////////////////////////////////////////////////////////////////

WavelengthGrid2::WavelengthGrid2()
    : _Nlambda(0)
{
}

////////////////////////////////////////////////////////////////////

void WavelengthGrid2::setupSelfBefore()
{
    SimulationItem::setupSelfBefore();
}

////////////////////////////////////////////////////////////////////

void WavelengthGrid2::setupSelfAfter()
{
    SimulationItem::setupSelfAfter();

    // the subclass should have added wavelengths to the vector
    _Nlambda = _lambdav.size();
    if (_Nlambda < 1) throw FATALERROR("There must be at least one wavelength in the grid");

    // wavelengths should be positive and sorted in ascending order
    if (_lambdav[0] <= 0.0) throw FATALERROR("All wavelengths should be positive");
    for (int ell = 1; ell < _Nlambda; ell++)
    {
        if (_lambdav[ell] <= _lambdav[ell-1]) throw FATALERROR("Wavelengths should be sorted in ascending order");
    }
}

////////////////////////////////////////////////////////////////////

int WavelengthGrid2::Nlambda() const
{
    return _Nlambda;
}

////////////////////////////////////////////////////////////////////

double WavelengthGrid2::lambda(int ell) const
{
    return _lambdav[ell];
}

////////////////////////////////////////////////////////////////////

double WavelengthGrid2::dlambda(int ell) const
{
    return _dlambdav[ell];
}

////////////////////////////////////////////////////////////////////

double WavelengthGrid2::lambdamin(int ell) const
{
    return (ell==0) ? _lambdav[0] : sqrt(_lambdav[ell-1]*_lambdav[ell]);
}

////////////////////////////////////////////////////////////////////

double WavelengthGrid2::lambdamax(int ell) const
{
    return (ell==_Nlambda-1) ? _lambdav[_Nlambda-1] : sqrt(_lambdav[ell]*_lambdav[ell+1]);
}

//////////////////////////////////////////////////////////////////////

int WavelengthGrid2::nearest(double lambda) const
{
    int ell = NR::locate_fail(_lambdav,lambda);
    if (ell<0) return -1;
    double lambdac = sqrt(_lambdav[ell]*_lambdav[ell+1]);
    if (lambda<lambdac) return ell;
    else return ell+1;
}

////////////////////////////////////////////////////////////////////

const Array& WavelengthGrid2::lambdav() const
{
    return _lambdav;
}

////////////////////////////////////////////////////////////////////

const Array& WavelengthGrid2::dlambdav() const
{
    return _dlambdav;
}

//////////////////////////////////////////////////////////////////////
