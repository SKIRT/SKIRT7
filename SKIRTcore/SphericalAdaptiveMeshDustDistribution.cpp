/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "AdaptiveMeshFile.hpp"
#include "FatalError.hpp"
#include "FilePaths.hpp"
#include "Log.hpp"
#include "MeshDustComponent.hpp"
#include "NR.hpp"
#include "Random.hpp"
#include "SphericalAdaptiveMesh.hpp"
#include "SphericalAdaptiveMeshDustDistribution.hpp"

using namespace std;

//////////////////////////////////////////////////////////////////////

SphericalAdaptiveMeshDustDistribution::SphericalAdaptiveMeshDustDistribution()
    : _meshfile(0), _densityUnits(0), _rin(0), _rout(0), _mesh(0)
{
}

////////////////////////////////////////////////////////////////////

SphericalAdaptiveMeshDustDistribution::~SphericalAdaptiveMeshDustDistribution()
{
    delete _mesh;
}

//////////////////////////////////////////////////////////////////////

void SphericalAdaptiveMeshDustDistribution::setupSelfBefore()
{
    DustDistribution::setupSelfBefore();

    // verify property values
    if (_densityUnits <= 0) throw FATALERROR("Density units should be positive");
    if (_rin <= 0 || _rout <= _rin) throw FATALERROR("Domain size should be positive");
    if (_dcv.isEmpty()) throw FATALERROR("There are no dust components");
}

//////////////////////////////////////////////////////////////////////

void SphericalAdaptiveMeshDustDistribution::setupSelfAfter()
{
    DustDistribution::setupSelfAfter();

    // make a list of the field indices needed by any of our components
    QList<int> fieldIndices;
    foreach (MeshDustComponent* dc, _dcv)
    {
        fieldIndices << dc->densityIndex() << dc->multiplierIndex();
    }

    // import the adaptive mesh
    _mesh = new SphericalAdaptiveMesh(_meshfile, fieldIndices, _rin, _rout);
    find<Log>()->info("Adaptive mesh data was successfully imported: " + QString::number(_mesh->Ncells()) + " cells.");

    // add a density field for each of our components, so that the mesh holds the total density
    foreach (MeshDustComponent* dc, _dcv)
    {
        _mesh->addDensityDistribution(dc->densityIndex(), dc->multiplierIndex(), dc->densityFraction());
    }

    // construct a vector with the normalized cumulative masses
    NR::cdf(_cumrhov, _mesh->Ncells(), [this](int i){return _mesh->density(i)*_mesh->volume(i);} );
}

//////////////////////////////////////////////////////////////////////

void SphericalAdaptiveMeshDustDistribution::setAdaptiveMeshFile(AdaptiveMeshFile* value)
{
    if (_meshfile) delete _meshfile;
    _meshfile = value;
    if (_meshfile) _meshfile->setParent(this);
}

//////////////////////////////////////////////////////////////////////

AdaptiveMeshFile* SphericalAdaptiveMeshDustDistribution::adaptiveMeshFile() const
{
    return _meshfile;
}

//////////////////////////////////////////////////////////////////////

void SphericalAdaptiveMeshDustDistribution::setDensityUnits(double value)
{
    _densityUnits = value;
}

//////////////////////////////////////////////////////////////////////

double SphericalAdaptiveMeshDustDistribution::densityUnits() const
{
    return _densityUnits;
}

//////////////////////////////////////////////////////////////////////

void SphericalAdaptiveMeshDustDistribution::setInnerRadius(double value)
{
    _rin = value;
}

//////////////////////////////////////////////////////////////////////

double SphericalAdaptiveMeshDustDistribution::innerRadius() const
{
    return _rin;
}

//////////////////////////////////////////////////////////////////////

void SphericalAdaptiveMeshDustDistribution::setOuterRadius(double value)
{
    _rout = value;
}

//////////////////////////////////////////////////////////////////////

double SphericalAdaptiveMeshDustDistribution::outerRadius() const
{
    return _rout;
}

//////////////////////////////////////////////////////////////////////

void SphericalAdaptiveMeshDustDistribution::insertComponent(int index, MeshDustComponent* value)
{
    if (!value) throw FATALERROR("Dust component pointer shouldn't be null");
    value->setParent(this);
    _dcv.insert(index, value);
}

//////////////////////////////////////////////////////////////////////

void SphericalAdaptiveMeshDustDistribution::removeComponent(int index)
{
    delete _dcv.takeAt(index);
}

//////////////////////////////////////////////////////////////////////

QList<MeshDustComponent*> SphericalAdaptiveMeshDustDistribution::components() const
{
    return _dcv;
}

//////////////////////////////////////////////////////////////////////

int SphericalAdaptiveMeshDustDistribution::dimension() const
{
    return 3;
}

//////////////////////////////////////////////////////////////////////

int SphericalAdaptiveMeshDustDistribution::Ncomp() const
{
    return _dcv.size();
}

//////////////////////////////////////////////////////////////////////

DustMix* SphericalAdaptiveMeshDustDistribution::mix(int h) const
{
    return _dcv[h]->mix();
}

//////////////////////////////////////////////////////////////////////

double SphericalAdaptiveMeshDustDistribution::density(int h, Position bfr) const
{
    return _densityUnits * _mesh->density(h, bfr);
}

//////////////////////////////////////////////////////////////////////

double SphericalAdaptiveMeshDustDistribution::density(Position bfr) const
{
    return _densityUnits * _mesh->density(bfr);
}

//////////////////////////////////////////////////////////////////////

Position SphericalAdaptiveMeshDustDistribution::generatePosition() const
{
    Random* random = find<Random>();
    int m = NR::locate_clip(_cumrhov, random->uniform());
    return _mesh->randomPosition(random, m);
}

//////////////////////////////////////////////////////////////////////

double SphericalAdaptiveMeshDustDistribution::mass() const
{
    return _densityUnits * _mesh->integratedDensity();
}

//////////////////////////////////////////////////////////////////////

double SphericalAdaptiveMeshDustDistribution::SigmaX() const
{
    return _densityUnits * _mesh->SigmaX();
}

//////////////////////////////////////////////////////////////////////

double SphericalAdaptiveMeshDustDistribution::SigmaY() const
{
    return _densityUnits * _mesh->SigmaY();
}

//////////////////////////////////////////////////////////////////////

double SphericalAdaptiveMeshDustDistribution::SigmaZ() const
{
    return _densityUnits * _mesh->SigmaZ();
}

//////////////////////////////////////////////////////////////////////
