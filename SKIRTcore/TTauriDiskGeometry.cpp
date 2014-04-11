/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include <cmath>
#include "FatalError.hpp"
#include "Random.hpp"
#include "TTauriDiskGeometry.hpp"

using namespace std;

//////////////////////////////////////////////////////////////////////

TTauriDiskGeometry::TTauriDiskGeometry()
    : _Rinn(0), _Rout(0), _Rd(0), _zd(0), _rho0(0)
{
}

////////////////////////////////////////////////////////////////////

void TTauriDiskGeometry::setupSelfBefore()
{
    AxGeometry::setupSelfBefore();

    // verify property values
    if (_Rinn <= 0) throw FATALERROR("the inner radius of the disk should be positive");
    if (_Rout <= _Rinn) throw FATALERROR("the outer radius of the disk must be larger than the inner radius");
    if (_Rd <= 0) throw FATALERROR("the radial scale length Rd should be positive");
    if (_zd <= 0) throw FATALERROR("the axial scale height zd should be positive");

    // calculate cached values
    _rho0 = 17.0/32.0/M_PI / (_Rd*_Rd*_zd) /
            (pow(_Rout/_Rd,17.0/8.0) - pow(_Rinn/_Rd,17.0/8.0));
}

////////////////////////////////////////////////////////////////////

void TTauriDiskGeometry::setMinRadius(double value)
{
    _Rinn = value;
}

////////////////////////////////////////////////////////////////////

double TTauriDiskGeometry::minRadius() const
{
    return _Rinn;
}

////////////////////////////////////////////////////////////////////

void TTauriDiskGeometry::setMaxRadius(double value)
{
    _Rout = value;
}

////////////////////////////////////////////////////////////////////

double TTauriDiskGeometry::maxRadius() const
{
    return _Rout;
}

////////////////////////////////////////////////////////////////////

void TTauriDiskGeometry::setRadialScale(double value)
{
    _Rd = value;
}

////////////////////////////////////////////////////////////////////

double TTauriDiskGeometry::radialScale() const
{
    return _Rd;
}

////////////////////////////////////////////////////////////////////

void TTauriDiskGeometry::setAxialScale(double value)
{
    _zd = value;
}

////////////////////////////////////////////////////////////////////

double TTauriDiskGeometry::axialScale() const
{
    return _zd;
}

//////////////////////////////////////////////////////////////////////

double
TTauriDiskGeometry::density(double R, double z)
const
{
    if (R<_Rinn || R>_Rout) return 0.0;
    double h = _zd*pow(R/_Rd,1.125);
    return _rho0 / (R/_Rd) * exp(-M_PI/4.0*pow(z/h,2));
}

//////////////////////////////////////////////////////////////////////

Position
TTauriDiskGeometry::generatePosition()
const
{
    double phi = 2.0 * M_PI * _random->uniform();
    double tinn = pow(_Rinn,2.125);
    double tout = pow(_Rout,2.125);
    double R = pow(tinn+_random->uniform()*(tout-tinn),1.0/2.125);
    double h = _zd*pow(R/_Rd,1.125);
    double sigma = sqrt(2.0/M_PI)*h;
    double z = _random->gauss() * sigma;
    return Position(R,phi,z,Position::CYLINDRICAL);
}

//////////////////////////////////////////////////////////////////////

double
TTauriDiskGeometry::SigmaR()
const
{
    return _rho0 * _Rd * log(_Rout/_Rinn);
}

//////////////////////////////////////////////////////////////////////

double
TTauriDiskGeometry::SigmaZ()
const
{
    return 0.0;
}

//////////////////////////////////////////////////////////////////////
