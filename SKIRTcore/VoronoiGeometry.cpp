/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include "FatalError.hpp"
#include "Log.hpp"
#include "NR.hpp"
#include "Random.hpp"
#include "VoronoiMesh.hpp"
#include "VoronoiMeshFile.hpp"
#include "VoronoiGeometry.hpp"

using namespace std;

//////////////////////////////////////////////////////////////////////

VoronoiGeometry::VoronoiGeometry()
    : _meshfile(0), _densityIndex(-1), _multiplierIndex(-1), _xmax(0), _ymax(0), _zmax(0), _mesh(0)
{
}

////////////////////////////////////////////////////////////////////

VoronoiGeometry::~VoronoiGeometry()
{
    delete _mesh;
}

////////////////////////////////////////////////////////////////////

void VoronoiGeometry::setupSelfBefore()
{
    GenGeometry::setupSelfBefore();

    // verify property values
    if (_xmax <= 0 || _ymax <= 0 || _zmax <= 0) throw FATALERROR("Domain size should be positive");
    if (_densityIndex < 0) throw FATALERROR("Column index for density must be specified");

    // import the Voronoi mesh
    _mesh = new VoronoiMesh(_meshfile, QList<int>() << _densityIndex << _multiplierIndex,
                             Box(-_xmax,-_ymax,-_zmax, _xmax,_ymax,_zmax));
    _mesh->addDensityDistribution(_densityIndex, _multiplierIndex);
    find<Log>()->info("Voronoi mesh data was successfully imported: " + QString::number(_mesh->Ncells()) + " cells.");

    // construct a vector with the normalized cumulative masses
    NR::cdf(_cumrhov, _mesh->Ncells(), [this](int i){return _mesh->density(i)*_mesh->volume(i);} );
}

//////////////////////////////////////////////////////////////////////

void VoronoiGeometry::setVoronoiMeshFile(VoronoiMeshFile* value)
{
    if (_meshfile) delete _meshfile;
    _meshfile = value;
    if (_meshfile) _meshfile->setParent(this);
}

//////////////////////////////////////////////////////////////////////

VoronoiMeshFile* VoronoiGeometry::voronoiMeshFile() const
{
    return _meshfile;
}

////////////////////////////////////////////////////////////////////

void VoronoiGeometry::setDensityIndex(int value)
{
    _densityIndex = value;
}

////////////////////////////////////////////////////////////////////

int VoronoiGeometry::densityIndex() const
{
    return _densityIndex;
}

////////////////////////////////////////////////////////////////////

void VoronoiGeometry::setMultiplierIndex(int value)
{
    _multiplierIndex = value;
}

////////////////////////////////////////////////////////////////////

int VoronoiGeometry::multiplierIndex() const
{
    return _multiplierIndex;
}

//////////////////////////////////////////////////////////////////////

void VoronoiGeometry::setExtentX(double value)
{
    _xmax = value;
}

//////////////////////////////////////////////////////////////////////

double VoronoiGeometry::extentX() const
{
    return _xmax;
}

//////////////////////////////////////////////////////////////////////

void VoronoiGeometry::setExtentY(double value)
{
    _ymax = value;
}

//////////////////////////////////////////////////////////////////////

double VoronoiGeometry::extentY() const
{
    return _ymax;
}

//////////////////////////////////////////////////////////////////////

void VoronoiGeometry::setExtentZ(double value)
{
    _zmax = value;
}

//////////////////////////////////////////////////////////////////////

double VoronoiGeometry::extentZ() const
{
    return _zmax;
}

//////////////////////////////////////////////////////////////////////

double VoronoiGeometry::density(Position bfr) const
{
    return _mesh->density(bfr) / _mesh->integratedDensity();
}

//////////////////////////////////////////////////////////////////////

Position VoronoiGeometry::generatePosition() const
{
    int m = NR::locate_clip(_cumrhov, _random->uniform());
    return _mesh->randomPosition(_random, m);
}

//////////////////////////////////////////////////////////////////////

double VoronoiGeometry::SigmaX() const
{
    return _mesh->SigmaX() / _mesh->integratedDensity();
}

//////////////////////////////////////////////////////////////////////

double VoronoiGeometry::SigmaY() const
{
    return _mesh->SigmaY() / _mesh->integratedDensity();
}

//////////////////////////////////////////////////////////////////////

double VoronoiGeometry::SigmaZ() const
{
    return _mesh->SigmaZ() / _mesh->integratedDensity();
}

///////////////////////////////////////////////////////////////////////////////

VoronoiMesh* VoronoiGeometry::mesh() const
{
    return _mesh;
}

//////////////////////////////////////////////////////////////////////

int VoronoiGeometry::numParticles() const
{
    return _mesh->Ncells();
}

//////////////////////////////////////////////////////////////////////

Vec VoronoiGeometry::particleCenter(int index) const
{
    return _mesh->particlePosition(index);
}

//////////////////////////////////////////////////////////////////////
