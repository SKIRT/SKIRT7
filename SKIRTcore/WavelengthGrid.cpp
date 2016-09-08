/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include <cmath>
#include "FatalError.hpp"
#include "NR.hpp"
#include "WavelengthGrid.hpp"
#include "StaggeredAssigner.hpp"

using namespace std;

////////////////////////////////////////////////////////////////////

WavelengthGrid::WavelengthGrid()
    : _Nlambda(0), _assigner(0)
{
}

////////////////////////////////////////////////////////////////////

void WavelengthGrid::setupSelfBefore()
{
    SimulationItem::setupSelfBefore();
}

////////////////////////////////////////////////////////////////////

void WavelengthGrid::setupSelfAfter()
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

    if (find<PeerToPeerCommunicator>()->dataParallel())
        _assigner = new StaggeredAssigner(_Nlambda, this);
}

////////////////////////////////////////////////////////////////////

const ProcessAssigner* WavelengthGrid::assigner() const
{
    return _assigner;
}

////////////////////////////////////////////////////////////////////

int WavelengthGrid::Nlambda() const
{
    return _Nlambda;
}

////////////////////////////////////////////////////////////////////

double WavelengthGrid::lambda(int ell) const
{
    return _lambdav[ell];
}

////////////////////////////////////////////////////////////////////

double WavelengthGrid::dlambda(int ell) const
{
    return _dlambdav[ell];
}

////////////////////////////////////////////////////////////////////

double WavelengthGrid::lambdamin(int ell) const
{
    return (ell==0) ? _lambdav[0] : sqrt(_lambdav[ell-1]*_lambdav[ell]);
}

////////////////////////////////////////////////////////////////////

double WavelengthGrid::lambdamax(int ell) const
{
    return (ell==_Nlambda-1) ? _lambdav[_Nlambda-1] : sqrt(_lambdav[ell]*_lambdav[ell+1]);
}

////////////////////////////////////////////////////////////////////

int WavelengthGrid::nearest(double lambda) const
{
    int ell = NR::locate_fail(_lambdav,lambda);
    if (ell<0) return -1;
    double lambdac = sqrt(_lambdav[ell]*_lambdav[ell+1]);
    if (lambda<lambdac) return ell;
    else return ell+1;
}

////////////////////////////////////////////////////////////////////

const Array& WavelengthGrid::lambdav() const
{
    return _lambdav;
}

////////////////////////////////////////////////////////////////////

const Array& WavelengthGrid::dlambdav() const
{
    return _dlambdav;
}

////////////////////////////////////////////////////////////////////
