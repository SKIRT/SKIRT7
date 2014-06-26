////////////////////////////////////////////////////////////////////

#include "FatalError.hpp"
#include "NR.hpp"
#include "PhotonPackage.hpp"
#include "Random.hpp"
#include "StellarComp.hpp"
#include "StellarSystem.hpp"
#include "WavelengthGrid.hpp"

using namespace std;

//////////////////////////////////////////////////////////////////////

StellarSystem::StellarSystem()
    : _random(0)
{
}

//////////////////////////////////////////////////////////////////////

void StellarSystem::addComponent(StellarComp* value)
{
    if (!value) throw FATALERROR("Stellar component pointer shouldn't be null");
    value->setParent(this);
    _scv << value;
}

//////////////////////////////////////////////////////////////////////

QList<StellarComp*> StellarSystem::components() const
{
    return _scv;
}

//////////////////////////////////////////////////////////////////////

void StellarSystem::setupSelfBefore()
{
    SimulationItem::setupSelfBefore();

    // verify that there is at least one component
    if (_scv.isEmpty()) throw FATALERROR("There are no stellar components");

    // cache the random generator
    _random = find<Random>();
}

//////////////////////////////////////////////////////////////////////

void StellarSystem::setupSelfAfter()
{
    SimulationItem::setupSelfAfter();

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
        NR::cdf(_Xvv[ell], Ncomp, [this,ell](int h){return _scv[h]->luminosity(ell);} );
    }
}

//////////////////////////////////////////////////////////////////////

double StellarSystem::luminosity(int ell) const
{
    return _Lv[ell];
}

//////////////////////////////////////////////////////////////////////

int StellarSystem::dimension() const
{
    int result = 1;
    foreach (StellarComp* sc, _scv) result = max(result, sc->dimension());
    return result;
}

//////////////////////////////////////////////////////////////////////

void StellarSystem::launch(PhotonPackage* pp, int ell, double L) const
{
    int h = NR::locate_clip(_Xvv[ell], _random->uniform());
    _scv[h]->launch(pp,ell,L);
}

//////////////////////////////////////////////////////////////////////
