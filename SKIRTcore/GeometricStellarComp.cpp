/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include "FatalError.hpp"
#include "PhotonPackage.hpp"
#include "GeometricStellarComp.hpp"
#include "Geometry.hpp"
#include "WavelengthGrid.hpp"

using namespace std;

//////////////////////////////////////////////////////////////////////

GeometricStellarComp::GeometricStellarComp()
    : _geom(0)
{
}

//////////////////////////////////////////////////////////////////////

void GeometricStellarComp::setupSelfBefore()
{
    StellarComp::setupSelfBefore();

    // verify that we have a geometry
    if (!_geom) throw FATALERROR("Geometry was not set");
}

//////////////////////////////////////////////////////////////////////

void GeometricStellarComp::setupSelfAfter()
{
    StellarComp::setupSelfAfter();

    // the subclass should have added the appropriate number of luminosities to the vector
    int nLum = _Lv.size();
    if (nLum != find<WavelengthGrid>()->Nlambda())
        throw FATALERROR("The number of luminosities differs from the number of wavelengths in the grid");
}

//////////////////////////////////////////////////////////////////////

void GeometricStellarComp::setGeometry(Geometry* value)
{
    if (_geom) delete _geom;
    _geom = value;
    if (_geom) _geom->setParent(this);
}

////////////////////////////////////////////////////////////////////

Geometry* GeometricStellarComp::geometry() const
{
    return _geom;
}

//////////////////////////////////////////////////////////////////////

int GeometricStellarComp::dimension() const
{
    return _geom->dimension();
}

//////////////////////////////////////////////////////////////////////

double GeometricStellarComp::luminosity(int ell) const
{
    return _Lv[ell];
}

//////////////////////////////////////////////////////////////////////

void GeometricStellarComp::launch(PhotonPackage* pp, int ell, double L) const
{
    Position bfr = _geom->generatePosition();
    Direction bfk = _geom->generateDirection(bfr);
    pp->launch(L,ell,bfr,bfk);
    pp->setAngularDistribution(_geom);
}

//////////////////////////////////////////////////////////////////////
