/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include "FatalError.hpp"
#include "PanStellarComp.hpp"
#include "PanWavelengthGrid.hpp"
#include "StellarCompNormalization.hpp"
#include "StellarSED.hpp"

using namespace std;

//////////////////////////////////////////////////////////////////////

PanStellarComp::PanStellarComp()
    : _sed(0), _norm(0)
{
}

////////////////////////////////////////////////////////////////////

void PanStellarComp::setupSelfBefore()
{
    GeometricStellarComp::setupSelfBefore();

    // verify that we have all properties
    if (!_sed) throw FATALERROR("SED was not set");
    if (!_norm) throw FATALERROR("Normalization was not set");

    // verify that the wavelength grid (and thus the simulation) is of the Pan type
    PanWavelengthGrid* lambdagrid = find<PanWavelengthGrid>();

    // provide room for the appropriate number of luminosities (to make base class happy)
    _Lv.resize(lambdagrid->Nlambda());
}

////////////////////////////////////////////////////////////////////

void PanStellarComp::setupSelfAfter()
{
    GeometricStellarComp::setupSelfAfter();

    // actually calculate the luminosities (we need our SED child to be setup for this)
    int Nlambda = find<WavelengthGrid>()->Nlambda();
    for (int ell=0; ell<Nlambda; ell++)
        _Lv[ell] = _norm->totluminosity(_sed) * _sed->luminosity(ell);
}

////////////////////////////////////////////////////////////////////

void PanStellarComp::setSed(StellarSED* value)
{
    if (_sed) delete _sed;
    _sed = value;
    if (_sed) _sed->setParent(this);
}

////////////////////////////////////////////////////////////////////

StellarSED* PanStellarComp::sed() const
{
    return _sed;
}

////////////////////////////////////////////////////////////////////

void PanStellarComp::setNormalization(StellarCompNormalization* value)
{
    if (_norm) delete _norm;
    _norm = value;
    if (_norm) _norm->setParent(this);
}

////////////////////////////////////////////////////////////////////

StellarCompNormalization* PanStellarComp::normalization() const
{
    return _norm;
}

////////////////////////////////////////////////////////////////////
