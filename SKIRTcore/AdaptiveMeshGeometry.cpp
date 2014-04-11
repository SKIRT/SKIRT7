/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include "FatalError.hpp"
#include "Log.hpp"
#include "NR.hpp"
#include "Random.hpp"
#include "AdaptiveMesh.hpp"
#include "AdaptiveMeshFile.hpp"
#include "AdaptiveMeshGeometry.hpp"

using namespace std;

//////////////////////////////////////////////////////////////////////

AdaptiveMeshGeometry::AdaptiveMeshGeometry()
    : _meshfile(0), _densityIndex(-1), _multiplierIndex(-1), _xmax(0), _ymax(0), _zmax(0), _mesh(0)
{
}

////////////////////////////////////////////////////////////////////

AdaptiveMeshGeometry::~AdaptiveMeshGeometry()
{
    delete _mesh;
}

////////////////////////////////////////////////////////////////////

void AdaptiveMeshGeometry::setupSelfBefore()
{
    GenGeometry::setupSelfBefore();

    // verify property values
    if (_xmax <= 0 || _ymax <= 0 || _zmax <= 0) throw FATALERROR("Domain size should be positive");
    if (_densityIndex < 0) throw FATALERROR("Column index for density must be specified");

    // import the adaptive mesh
    _mesh = new AdaptiveMesh(_meshfile, QList<int>() << _densityIndex << _multiplierIndex,
                             Box(-_xmax,-_ymax,-_zmax, _xmax,_ymax,_zmax));
    _mesh->addDensityDistribution(_densityIndex, _multiplierIndex);
    find<Log>()->info("Adaptive mesh data was successfully imported: " + QString::number(_mesh->Ncells()) + " cells.");

    // construct a vector with the normalized cumulative densities
    NR::cdf(_cumrhov, _mesh, &AdaptiveMesh::Ncells, &AdaptiveMesh::density);
}

//////////////////////////////////////////////////////////////////////

void AdaptiveMeshGeometry::setAdaptiveMeshFile(AdaptiveMeshFile* value)
{
    if (_meshfile) delete _meshfile;
    _meshfile = value;
    if (_meshfile) _meshfile->setParent(this);
}

//////////////////////////////////////////////////////////////////////

AdaptiveMeshFile* AdaptiveMeshGeometry::adaptiveMeshFile() const
{
    return _meshfile;
}

////////////////////////////////////////////////////////////////////

void AdaptiveMeshGeometry::setDensityIndex(int value)
{
    _densityIndex = value;
}

////////////////////////////////////////////////////////////////////

int AdaptiveMeshGeometry::densityIndex() const
{
    return _densityIndex;
}

////////////////////////////////////////////////////////////////////

void AdaptiveMeshGeometry::setMultiplierIndex(int value)
{
    _multiplierIndex = value;
}

////////////////////////////////////////////////////////////////////

int AdaptiveMeshGeometry::multiplierIndex() const
{
    return _multiplierIndex;
}

//////////////////////////////////////////////////////////////////////

void AdaptiveMeshGeometry::setExtentX(double value)
{
    _xmax = value;
}

//////////////////////////////////////////////////////////////////////

double AdaptiveMeshGeometry::extentX() const
{
    return _xmax;
}

//////////////////////////////////////////////////////////////////////

void AdaptiveMeshGeometry::setExtentY(double value)
{
    _ymax = value;
}

//////////////////////////////////////////////////////////////////////

double AdaptiveMeshGeometry::extentY() const
{
    return _ymax;
}

//////////////////////////////////////////////////////////////////////

void AdaptiveMeshGeometry::setExtentZ(double value)
{
    _zmax = value;
}

//////////////////////////////////////////////////////////////////////

double AdaptiveMeshGeometry::extentZ() const
{
    return _zmax;
}

//////////////////////////////////////////////////////////////////////

double AdaptiveMeshGeometry::density(Position bfr) const
{
    return _mesh->density(bfr) / _mesh->integratedDensity();
}

//////////////////////////////////////////////////////////////////////

Position AdaptiveMeshGeometry::generatePosition() const
{
    int m = NR::locate_clip(_cumrhov, _random->uniform());
    return _mesh->randomPosition(_random, m);
}

//////////////////////////////////////////////////////////////////////

double AdaptiveMeshGeometry::SigmaX() const
{
    return _mesh->SigmaX() / _mesh->integratedDensity();
}

//////////////////////////////////////////////////////////////////////

double AdaptiveMeshGeometry::SigmaY() const
{
    return _mesh->SigmaY() / _mesh->integratedDensity();
}

//////////////////////////////////////////////////////////////////////

double AdaptiveMeshGeometry::SigmaZ() const
{
    return _mesh->SigmaZ() / _mesh->integratedDensity();
}

//////////////////////////////////////////////////////////////////////

AdaptiveMesh* AdaptiveMeshGeometry::mesh() const
{
    return _mesh;
}

///////////////////////////////////////////////////////////////////////////////
