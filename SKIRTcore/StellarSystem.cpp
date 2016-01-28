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
    : _emissionBias(0.5), _random(0)
{
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
        NR::cdf(_Xvv[ell], Ncomp, [this,ell](int h){return _scv[h]->luminosity(ell);} );
}

//////////////////////////////////////////////////////////////////////

void StellarSystem::insertComponent(int index, StellarComp* value)
{
    if (!value) throw FATALERROR("Stellar component pointer shouldn't be null");
    value->setParent(this);
    _scv.insert(index, value);
}

//////////////////////////////////////////////////////////////////////

void StellarSystem::removeComponent(int index)
{
    delete _scv.takeAt(index);
}

//////////////////////////////////////////////////////////////////////

QList<StellarComp*> StellarSystem::components() const
{
    return _scv;
}

////////////////////////////////////////////////////////////////////

void StellarSystem::setEmissionBias(double value)
{
    _emissionBias = value;
}

////////////////////////////////////////////////////////////////////

double StellarSystem::emissionBias() const
{
    return _emissionBias;
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

int StellarSystem::Ncomp() const
{
    return _scv.size();
}

//////////////////////////////////////////////////////////////////////

void StellarSystem::launch(PhotonPackage* pp, int ell, double L) const
{
    int h = 0;
    int N = Ncomp();
    double Lmean = _Lv[ell]/N; // the mean luminosity emitted from each stellar component
    double X = _random->uniform();
    if (X<_emissionBias)
    {
        // rescale the deviate from [0,_emissionBias[ to [0,N[
        h = max(0,min(N-1,static_cast<int>(N*X/_emissionBias)));
    }
    else
    {
        // rescale the deviate from [_emissionBias,1[ to [0,1[
        h = NR::locate_clip(_Xvv[ell],(X-_emissionBias)/(1.0-_emissionBias));
    }
    StellarComp* sc = _scv[h];
    double Lh = sc->luminosity(ell);
    double weight = 1.0/(1.0-_emissionBias+_emissionBias*Lmean/Lh);
    sc->launch(pp,ell,L*weight);
    pp->setStellarOrigin(h);
}

//////////////////////////////////////////////////////////////////////
