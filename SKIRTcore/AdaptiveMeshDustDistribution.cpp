/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "AdaptiveMesh.hpp"
#include "AdaptiveMeshDustDistribution.hpp"
#include "AdaptiveMeshFile.hpp"
#include "FatalError.hpp"
#include "FilePaths.hpp"
#include "Log.hpp"
#include "MeshDustComponent.hpp"
#include "NR.hpp"
#include "Random.hpp"

using namespace std;

//////////////////////////////////////////////////////////////////////

AdaptiveMeshDustDistribution::AdaptiveMeshDustDistribution()
    : _meshfile(0), _densityUnits(0), _xmax(0), _ymax(0), _zmax(0), _mesh(0)
{
}

////////////////////////////////////////////////////////////////////

AdaptiveMeshDustDistribution::~AdaptiveMeshDustDistribution()
{
    delete _mesh;
}

//////////////////////////////////////////////////////////////////////

void AdaptiveMeshDustDistribution::setupSelfBefore()
{
    DustDistribution::setupSelfBefore();

    // verify property values
    if (_densityUnits <= 0) throw FATALERROR("Density units should be positive");
    if (_xmax <= 0 || _ymax <= 0 || _zmax <= 0) throw FATALERROR("Domain size should be positive");
    if (_dcv.isEmpty()) throw FATALERROR("There are no dust components");
}

//////////////////////////////////////////////////////////////////////

void AdaptiveMeshDustDistribution::setupSelfAfter()
{
    DustDistribution::setupSelfAfter();

    // make a list of the field indices needed by any of our components
    QList<int> fieldIndices;
    foreach (MeshDustComponent* dc, _dcv)
    {
        fieldIndices << dc->densityIndex() << dc->multiplierIndex();
    }

    // import the adaptive mesh
    _mesh = new AdaptiveMesh(_meshfile, fieldIndices, Box(-_xmax,-_ymax,-_zmax, _xmax,_ymax,_zmax));
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

void AdaptiveMeshDustDistribution::setAdaptiveMeshFile(AdaptiveMeshFile* value)
{
    if (_meshfile) delete _meshfile;
    _meshfile = value;
    if (_meshfile) _meshfile->setParent(this);
}

//////////////////////////////////////////////////////////////////////

AdaptiveMeshFile* AdaptiveMeshDustDistribution::adaptiveMeshFile() const
{
    return _meshfile;
}

//////////////////////////////////////////////////////////////////////

void AdaptiveMeshDustDistribution::setDensityUnits(double value)
{
    _densityUnits = value;
}

//////////////////////////////////////////////////////////////////////

double AdaptiveMeshDustDistribution::densityUnits() const
{
    return _densityUnits;
}

//////////////////////////////////////////////////////////////////////

void AdaptiveMeshDustDistribution::setExtentX(double value)
{
    _xmax = value;
}

//////////////////////////////////////////////////////////////////////

double AdaptiveMeshDustDistribution::extentX() const
{
    return _xmax;
}

//////////////////////////////////////////////////////////////////////

void AdaptiveMeshDustDistribution::setExtentY(double value)
{
    _ymax = value;
}

//////////////////////////////////////////////////////////////////////

double AdaptiveMeshDustDistribution::extentY() const
{
    return _ymax;
}

//////////////////////////////////////////////////////////////////////

void AdaptiveMeshDustDistribution::setExtentZ(double value)
{
    _zmax = value;
}

//////////////////////////////////////////////////////////////////////

double AdaptiveMeshDustDistribution::extentZ() const
{
    return _zmax;
}

//////////////////////////////////////////////////////////////////////

void AdaptiveMeshDustDistribution::insertComponent(int index, MeshDustComponent* value)
{
    if (!value) throw FATALERROR("Dust component pointer shouldn't be null");
    value->setParent(this);
    _dcv.insert(index, value);
}

//////////////////////////////////////////////////////////////////////

void AdaptiveMeshDustDistribution::removeComponent(int index)
{
    delete _dcv.takeAt(index);
}

//////////////////////////////////////////////////////////////////////

QList<MeshDustComponent*> AdaptiveMeshDustDistribution::components() const
{
    return _dcv;
}

//////////////////////////////////////////////////////////////////////

int AdaptiveMeshDustDistribution::dimension() const
{
    return 3;
}

//////////////////////////////////////////////////////////////////////

int AdaptiveMeshDustDistribution::Ncomp() const
{
    return _dcv.size();
}

//////////////////////////////////////////////////////////////////////

DustMix* AdaptiveMeshDustDistribution::mix(int h) const
{
    return _dcv[h]->mix();
}

//////////////////////////////////////////////////////////////////////

double AdaptiveMeshDustDistribution::density(int h, Position bfr) const
{
    return _densityUnits * _mesh->density(h, bfr);
}

//////////////////////////////////////////////////////////////////////

double AdaptiveMeshDustDistribution::density(Position bfr) const
{
    return _densityUnits * _mesh->density(bfr);
}

//////////////////////////////////////////////////////////////////////

Position AdaptiveMeshDustDistribution::generatePosition() const
{
    Random* random = find<Random>();
    int m = NR::locate_clip(_cumrhov, random->uniform());
    return _mesh->randomPosition(random, m);
}

//////////////////////////////////////////////////////////////////////

double AdaptiveMeshDustDistribution::mass() const
{
    return _densityUnits * _mesh->integratedDensity();
}

//////////////////////////////////////////////////////////////////////

double AdaptiveMeshDustDistribution::SigmaX() const
{
    return _densityUnits * _mesh->SigmaX();
}

//////////////////////////////////////////////////////////////////////

double AdaptiveMeshDustDistribution::SigmaY() const
{
    return _densityUnits * _mesh->SigmaY();
}

//////////////////////////////////////////////////////////////////////

double AdaptiveMeshDustDistribution::SigmaZ() const
{
    return _densityUnits * _mesh->SigmaZ();
}

//////////////////////////////////////////////////////////////////////

AdaptiveMesh* AdaptiveMeshDustDistribution::mesh() const
{
    return _mesh;
}

//////////////////////////////////////////////////////////////////////
