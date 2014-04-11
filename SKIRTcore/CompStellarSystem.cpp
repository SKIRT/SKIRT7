/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include "CompStellarSystem.hpp"
#include "FatalError.hpp"
#include "NR.hpp"
#include "PhotonPackage.hpp"
#include "Random.hpp"
#include "StellarComp.hpp"
#include "WavelengthGrid.hpp"

using namespace std;

//////////////////////////////////////////////////////////////////////

CompStellarSystem::CompStellarSystem()
    : _random(0)
{
}

//////////////////////////////////////////////////////////////////////

void CompStellarSystem::addComponent(StellarComp* value)
{
    if (!value) throw FATALERROR("Stellar component pointer shouldn't be null");
    value->setParent(this);
    _scv << value;
}

//////////////////////////////////////////////////////////////////////

QList<StellarComp*> CompStellarSystem::components() const
{
    return _scv;
}

//////////////////////////////////////////////////////////////////////

void CompStellarSystem::setupSelfBefore()
{
    StellarSystem::setupSelfBefore();

    // verify that there is at least one component
    if (_scv.isEmpty()) throw FATALERROR("There are no stellar components");

    // cache the random generator
    _random = find<Random>();
}

//////////////////////////////////////////////////////////////////////

void CompStellarSystem::setupSelfAfter()
{
    StellarSystem::setupSelfAfter();

    int Nlambda = find<WavelengthGrid>()->Nlambda();
    int Ncomp = _scv.size();

    // Fill the vector _Lv with the luminosity for every wavelength bin

    _Lv.resize(Nlambda);
    for (int ell=0; ell<Nlambda; ell++)
        for (int h=0; h<Ncomp; h++)
            _Lv[ell] += _scv[h]->luminosity(ell);

    // Fill the vectors _Xvv with the normalized cumulative luminosities (per wavelength bin)

    _Xvv.resize(Nlambda,0);
    for (int ell=0; ell<Nlambda; ell++)
    {
        Array Lv(Ncomp);
        for (int h=0; h<Ncomp; h++) Lv[h] = _scv[h]->luminosity(ell);
        NR::cdf(_Xvv[ell], Lv);
    }
}

//////////////////////////////////////////////////////////////////////

double CompStellarSystem::luminosity(int ell) const
{
    return _Lv[ell];
}

//////////////////////////////////////////////////////////////////////

int CompStellarSystem::dimension() const
{
    int result = 1;
    foreach (StellarComp* sc, _scv) result = qMax(result, sc->dimension());
    return result;
}

//////////////////////////////////////////////////////////////////////

void CompStellarSystem::launch(PhotonPackage* pp, int ell, double L) const
{
    int h = NR::locate_clip(_Xvv[ell], _random->uniform());
    _scv[h]->launch(pp,ell,L);
}

//////////////////////////////////////////////////////////////////////
