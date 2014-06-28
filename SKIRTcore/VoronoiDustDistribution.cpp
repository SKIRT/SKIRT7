/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include "FatalError.hpp"
#include "Log.hpp"
#include "MeshDustComponent.hpp"
#include "NR.hpp"
#include "Random.hpp"
#include "VoronoiDustDistribution.hpp"
#include "VoronoiMesh.hpp"
#include "VoronoiMeshFile.hpp"

using namespace std;

//////////////////////////////////////////////////////////////////////

VoronoiDustDistribution::VoronoiDustDistribution()
    : _meshfile(0), _densityUnits(0), _xmax(0), _ymax(0), _zmax(0), _mesh(0)
{
}

////////////////////////////////////////////////////////////////////

VoronoiDustDistribution::~VoronoiDustDistribution()
{
    delete _mesh;
}

//////////////////////////////////////////////////////////////////////

void VoronoiDustDistribution::setupSelfBefore()
{
    DustDistribution::setupSelfBefore();

    // verify property values
    if (_densityUnits <= 0) throw FATALERROR("Density units should be positive");
    if (_xmax <= 0 || _ymax <= 0 || _zmax <= 0) throw FATALERROR("Domain size should be positive");
    if (_dcv.isEmpty()) throw FATALERROR("There are no dust components");
}

//////////////////////////////////////////////////////////////////////

void VoronoiDustDistribution::setupSelfAfter()
{
    DustDistribution::setupSelfAfter();

    // make a list of the field indices needed by any of our components
    QList<int> fieldIndices;
    foreach (MeshDustComponent* dc, _dcv)
    {
        fieldIndices << dc->densityIndex() << dc->multiplierIndex();
    }

    // import the Voronoi mesh
    _mesh = new VoronoiMesh(_meshfile, fieldIndices, Box(-_xmax,-_ymax,-_zmax, _xmax,_ymax,_zmax));
    find<Log>()->info("Voronoi mesh data was successfully imported: " + QString::number(_mesh->Ncells()) + " cells.");

    // add a density field for each of our components, so that the mesh holds the total density
    foreach (MeshDustComponent* dc, _dcv)
    {
        _mesh->addDensityDistribution(dc->densityIndex(), dc->multiplierIndex(), dc->densityFraction());
    }

    // construct a vector with the normalized cumulative masses
    NR::cdf(_cumrhov, _mesh->Ncells(), [this](int i){return _mesh->density(i)*_mesh->volume(i);} );
}

//////////////////////////////////////////////////////////////////////

void VoronoiDustDistribution::setVoronoiMeshFile(VoronoiMeshFile* value)
{
    if (_meshfile) delete _meshfile;
    _meshfile = value;
    if (_meshfile) _meshfile->setParent(this);
}

//////////////////////////////////////////////////////////////////////

VoronoiMeshFile* VoronoiDustDistribution::voronoiMeshFile() const
{
    return _meshfile;
}

//////////////////////////////////////////////////////////////////////

void VoronoiDustDistribution::setDensityUnits(double value)
{
    _densityUnits = value;
}

//////////////////////////////////////////////////////////////////////

double VoronoiDustDistribution::densityUnits() const
{
    return _densityUnits;
}

//////////////////////////////////////////////////////////////////////

void VoronoiDustDistribution::setExtentX(double value)
{
    _xmax = value;
}

//////////////////////////////////////////////////////////////////////

double VoronoiDustDistribution::extentX() const
{
    return _xmax;
}

//////////////////////////////////////////////////////////////////////

void VoronoiDustDistribution::setExtentY(double value)
{
    _ymax = value;
}

//////////////////////////////////////////////////////////////////////

double VoronoiDustDistribution::extentY() const
{
    return _ymax;
}

//////////////////////////////////////////////////////////////////////

void VoronoiDustDistribution::setExtentZ(double value)
{
    _zmax = value;
}

//////////////////////////////////////////////////////////////////////

double VoronoiDustDistribution::extentZ() const
{
    return _zmax;
}

//////////////////////////////////////////////////////////////////////

void VoronoiDustDistribution::addComponent(MeshDustComponent* value)
{
    if (!value) throw FATALERROR("Dust component pointer shouldn't be null");
    value->setParent(this);
    _dcv << value;
}

//////////////////////////////////////////////////////////////////////

QList<MeshDustComponent*> VoronoiDustDistribution::components() const
{
    return _dcv;
}

//////////////////////////////////////////////////////////////////////

int VoronoiDustDistribution::dimension() const
{
    return 3;
}

//////////////////////////////////////////////////////////////////////

int VoronoiDustDistribution::Ncomp() const
{
    return _dcv.size();
}

//////////////////////////////////////////////////////////////////////

DustMix* VoronoiDustDistribution::mix(int h) const
{
    return _dcv[h]->mix();
}

//////////////////////////////////////////////////////////////////////

double VoronoiDustDistribution::density(int h, Position bfr) const
{
    return _densityUnits * _mesh->density(h, bfr);
}

//////////////////////////////////////////////////////////////////////

double VoronoiDustDistribution::density(Position bfr) const
{
    return _densityUnits * _mesh->density(bfr);
}

//////////////////////////////////////////////////////////////////////

Position VoronoiDustDistribution::generatePosition() const
{
    Random* random = find<Random>();
    int m = NR::locate_clip(_cumrhov, random->uniform());
    return _mesh->randomPosition(random, m);
}

//////////////////////////////////////////////////////////////////////

double VoronoiDustDistribution::mass() const
{
    return _densityUnits * _mesh->integratedDensity();
}

//////////////////////////////////////////////////////////////////////

double VoronoiDustDistribution::SigmaX() const
{
    return _densityUnits * _mesh->SigmaX();
}

//////////////////////////////////////////////////////////////////////

double VoronoiDustDistribution::SigmaY() const
{
    return _densityUnits * _mesh->SigmaY();
}

//////////////////////////////////////////////////////////////////////

double VoronoiDustDistribution::SigmaZ() const
{
    return _densityUnits * _mesh->SigmaZ();
}

//////////////////////////////////////////////////////////////////////

VoronoiMesh* VoronoiDustDistribution::mesh() const
{
    return _mesh;
}

//////////////////////////////////////////////////////////////////////

int VoronoiDustDistribution::numParticles() const
{
    return _mesh->Ncells();
}

//////////////////////////////////////////////////////////////////////

Vec VoronoiDustDistribution::particleCenter(int index) const
{
    return _mesh->particlePosition(index);
}

//////////////////////////////////////////////////////////////////////
