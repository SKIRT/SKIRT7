/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include "DustSystem.hpp"
#include "PeelOffPhotonPackage.hpp"

using namespace std;

////////////////////////////////////////////////////////////////////

PeelOffPhotonPackage::
PeelOffPhotonPackage(bool ynstellar,
                     int ell,
                     Position bfr,
                     Direction bfk,
                     double L,
                     int nscatt,
                     DustSystem *ds) :
    PhotonPackage(ynstellar,ell,bfr,bfk,L,nscatt)
{
    _ds = ds;
}

////////////////////////////////////////////////////////////////////

double
PeelOffPhotonPackage::opticaldepth(double distance)
const
{
    if (_ds)
        return _ds->opticaldepth(_ell,_bfr,_bfk,distance);
    else
        return 0.0;
}

////////////////////////////////////////////////////////////////////
