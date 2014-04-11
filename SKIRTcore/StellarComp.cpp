/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include "FatalError.hpp"
#include "PhotonPackage.hpp"
#include "Random.hpp"
#include "StellarComp.hpp"
#include "Geometry.hpp"
#include "WavelengthGrid.hpp"

using namespace std;

//////////////////////////////////////////////////////////////////////

StellarComp::StellarComp()
    : _geom(0)
{
}

//////////////////////////////////////////////////////////////////////

void StellarComp::setupSelfBefore()
{
    SimulationItem::setupSelfBefore();

    // verify that we have a geometry
    if (!_geom) throw FATALERROR("Geometry was not set");
}

//////////////////////////////////////////////////////////////////////

void StellarComp::setupSelfAfter()
{
    SimulationItem::setupSelfAfter();

    // the subclass should have added the appropriate number of luminosities to the vector
    int nLum = _Lv.size();
    if (nLum != find<WavelengthGrid>()->Nlambda())
        throw FATALERROR("The number of luminosities differs from the number of wavelengths in the grid");
}

//////////////////////////////////////////////////////////////////////

void StellarComp::setGeometry(Geometry* value)
{
    if (_geom) delete _geom;
    _geom = value;
    if (_geom) _geom->setParent(this);
}

////////////////////////////////////////////////////////////////////

Geometry* StellarComp::geometry() const
{
    return _geom;
}

//////////////////////////////////////////////////////////////////////

double
StellarComp::luminosity(int ell)
const
{
    return _Lv[ell];
}

//////////////////////////////////////////////////////////////////////

int StellarComp::dimension()
{
    return _geom->dimension();
}

//////////////////////////////////////////////////////////////////////

void StellarComp::launch(PhotonPackage* pp, int ell, double L) const
{
    Position bfr = _geom->generatePosition();
    Direction bfk = _geom->generateDirection(bfr);
    pp->set(true,ell,bfr,bfk,L,0);
    pp->setAngularDistribution(_geom);
}

//////////////////////////////////////////////////////////////////////
