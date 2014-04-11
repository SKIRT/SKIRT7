/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include "FatalError.hpp"
#include "NR.hpp"
#include "OligoWavelengthGrid.hpp"

using namespace std;

////////////////////////////////////////////////////////////////////

OligoWavelengthGrid::OligoWavelengthGrid()
{
}

////////////////////////////////////////////////////////////////////

void OligoWavelengthGrid::setupSelfAfter()
{
    WavelengthGrid::setupSelfAfter();

    // calculate the wavelength bin widths (very small and independent of the other wavelengths)
    _dlambdav.resize(_Nlambda);
    for (int ell = 0; ell < _Nlambda; ell++) _dlambdav[ell] = 0.001*_lambdav[ell];
}

////////////////////////////////////////////////////////////////////

void OligoWavelengthGrid::setWavelengths(QList<double> value)
{
    // copy the wavelengths and sort them in ascending order
    NR::assign(_lambdav, value);
    NR::sort(_lambdav);
}

//////////////////////////////////////////////////////////////////////

QList<double> OligoWavelengthGrid::wavelengths() const
{
    QList<double> result;
    for (unsigned int ell = 0; ell < _lambdav.size(); ell++) result << _lambdav[ell];
    return result;
}

//////////////////////////////////////////////////////////////////////

bool OligoWavelengthGrid::issampledrange() const
{
    return false;
}

//////////////////////////////////////////////////////////////////////
