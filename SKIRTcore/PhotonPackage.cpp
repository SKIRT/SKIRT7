/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "PhotonPackage.hpp"
#include "AngularDistribution.hpp"

using namespace std;

////////////////////////////////////////////////////////////////////

PhotonPackage::PhotonPackage()
    : _L(0), _ell(0), _nscatt(0), _stellar(-1), _ad(0)
{
}

////////////////////////////////////////////////////////////////////

void PhotonPackage::launch(double L, int ell, Position bfr, Direction bfk)
{
    _L = L;
    _ell = ell;
    _bfr = bfr;
    _bfk_prev = _bfk = bfk;
    _nscatt = 0;
    _stellar = -1;
    _ad = 0;
    clearStokes();
}

////////////////////////////////////////////////////////////////////

void PhotonPackage::launchEmissionPeelOff(const PhotonPackage* pp, Direction bfk)
{
    _L = pp->_L;
    _ell = pp->_ell;
    _bfr = pp->_bfr;
    _bfk_prev = _bfk = bfk;
    _nscatt = 0;
    _stellar = pp->_stellar;
    _ad = 0;
    clearStokes();

    // apply emission direction bias if not isotropic
    if (pp->_ad) _L *= pp->_ad->probabilityForDirection(_bfr, _bfk);
}

////////////////////////////////////////////////////////////////////

void PhotonPackage::launchScatteringPeelOff(const PhotonPackage* pp, Direction bfk, double w)
{
    _L = pp->_L * w;
    _ell = pp->_ell;
    _bfr = pp->_bfr;
    _bfk = bfk;
    _bfk_prev = pp->_bfk;
    _nscatt = pp->_nscatt + 1;
    _stellar = pp->_stellar;
    _ad = 0;
    clearStokes();
}

////////////////////////////////////////////////////////////////////

void PhotonPackage::launchScatteringPeelOff(const PhotonPackage* pp, Position bfr, Direction bfk, double w)
{
    _L = pp->_L * w;
    _ell = pp->_ell;
    _bfr = bfr;
    _bfk = bfk;
    _bfk_prev = pp->_bfk;
    _nscatt = pp->_nscatt + 1;
    _stellar = pp->_stellar;
    _ad = 0;
    clearStokes();
}

////////////////////////////////////////////////////////////////////

void PhotonPackage::setStellarOrigin(int stellarCompIndex)
{
    _stellar = stellarCompIndex;
}

////////////////////////////////////////////////////////////////////

void PhotonPackage::setAngularDistribution(const AngularDistribution* ad)
{
    _ad = ad;
}

////////////////////////////////////////////////////////////////////

void PhotonPackage::propagate(double s)
{
    _bfr += s*_bfk;
}

////////////////////////////////////////////////////////////////////

void PhotonPackage::scatter(Direction bfk)
{
    _nscatt++;
    _bfk_prev = _bfk;
    _bfk = bfk;
    _ad = 0;
}

////////////////////////////////////////////////////////////////////

void PhotonPackage::setLuminosity(double L)
{
    _L = L;
}

////////////////////////////////////////////////////////////////////
