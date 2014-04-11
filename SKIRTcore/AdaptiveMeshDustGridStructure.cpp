/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include "AdaptiveMesh.hpp"
#include "AdaptiveMeshDustGridStructure.hpp"
#include "AdaptiveMeshInterface.hpp"
#include "DustDistribution.hpp"
#include "DustGridPath.hpp"
#include "DustGridPlotFile.hpp"
#include "FatalError.hpp"
#include "Log.hpp"

using namespace std;

//////////////////////////////////////////////////////////////////////

AdaptiveMeshDustGridStructure::AdaptiveMeshDustGridStructure()
    : _mesh(0)
{
}

////////////////////////////////////////////////////////////////////

void AdaptiveMeshDustGridStructure::setupSelfBefore()
{
    GenDustGridStructure::setupSelfBefore();

    // get the adaptive mesh and tell it to construct neighbor information for its cells
    DustDistribution* dd = find<DustDistribution>();
    AdaptiveMeshInterface* interface = dd->interface<AdaptiveMeshInterface>();
    if (!interface) throw FATALERROR("Can't find an adaptive mesh in the simulation hierarchy");
    _mesh = interface->mesh();
    find<Log>()->info("Adding neighbor information to adaptive mesh...");
    _mesh->addNeighbors();

    // calculate the normalization factor imposed by the dust distribution
    // we need this to directly compute cell densities for the DustGridDensityInterface
    _nf = dd->mass() / _mesh->integratedDensity();

    // set the total number of cells
    _Ncells = _mesh->Ncells();
}

//////////////////////////////////////////////////////////////////////

double AdaptiveMeshDustGridStructure::xmax() const
{
    return _mesh->extent().xmax();
}

//////////////////////////////////////////////////////////////////////

double AdaptiveMeshDustGridStructure::ymax() const
{
    return _mesh->extent().ymax();
}

//////////////////////////////////////////////////////////////////////

double AdaptiveMeshDustGridStructure::zmax() const
{
    return _mesh->extent().zmax();
}

//////////////////////////////////////////////////////////////////////

double AdaptiveMeshDustGridStructure::volume(int m) const
{
    return _mesh->volume(m);
}

//////////////////////////////////////////////////////////////////////

int AdaptiveMeshDustGridStructure::whichcell(Position bfr) const
{
    return _mesh->cellIndex(bfr);
}

//////////////////////////////////////////////////////////////////////

Position AdaptiveMeshDustGridStructure::centralPositionInCell(int m) const
{
    return _mesh->centralPosition(m);
}

//////////////////////////////////////////////////////////////////////

Position AdaptiveMeshDustGridStructure::randomPositionInCell(int m) const
{
    return _mesh->randomPosition(_random, m);
}

double AdaptiveMeshDustGridStructure::density(int h, int m) const
{
    return _nf * _mesh->density(h, m);
}

//////////////////////////////////////////////////////////////////////

DustGridPath AdaptiveMeshDustGridStructure::path(Position bfr, Direction bfk) const
{
    return _mesh->path(bfr, bfk);
}

//////////////////////////////////////////////////////////////////////

void AdaptiveMeshDustGridStructure::write_xy(DustGridPlotFile* outfile) const
{
    // Output the domain
    double xmin, ymin, zmin, xmax, ymax, zmax;
    _mesh->extent().extent(xmin, ymin, zmin, xmax, ymax, zmax);
    outfile->writeRectangle(xmin, ymin, xmax, ymax);

    // Output all leaf cells that intersect the coordinate plane
    double eps = 1e-8*(zmax-zmin);
    for (int m=0; m<_Ncells; m++)
    {
        _mesh->extent(m).extent(xmin, ymin, zmin, xmax, ymax, zmax);
        if (zmin < eps && zmax > -eps)
        {
            outfile->writeRectangle(xmin, ymin, xmax, ymax);
        }
    }
}

//////////////////////////////////////////////////////////////////////

void AdaptiveMeshDustGridStructure::write_xz(DustGridPlotFile* outfile) const
{
    // Output the domain
    double xmin, ymin, zmin, xmax, ymax, zmax;
    _mesh->extent().extent(xmin, ymin, zmin, xmax, ymax, zmax);
    outfile->writeRectangle(xmin, zmin, xmax, zmax);

    // Output all leaf cells that intersect the coordinate plane
    double eps = 1e-8*(ymax-ymin);
    for (int m=0; m<_Ncells; m++)
    {
        _mesh->extent(m).extent(xmin, ymin, zmin, xmax, ymax, zmax);
        if (ymin < eps && ymax > -eps)
        {
            outfile->writeRectangle(xmin, zmin, xmax, zmax);
        }
    }
}

//////////////////////////////////////////////////////////////////////

void AdaptiveMeshDustGridStructure::write_yz(DustGridPlotFile* outfile) const
{
    // Output the domain
    double xmin, ymin, zmin, xmax, ymax, zmax;
    _mesh->extent().extent(xmin, ymin, zmin, xmax, ymax, zmax);
    outfile->writeRectangle(ymin, zmin, ymax, zmax);

    // Output all leaf cells that intersect the coordinate plane
    double eps = 1e-8*(xmax-xmin);
    for (int m=0; m<_Ncells; m++)
    {
        _mesh->extent(m).extent(xmin, ymin, zmin, xmax, ymax, zmax);
        if (xmin < eps && xmax > -eps)
        {
            outfile->writeRectangle(ymin, zmin, ymax, zmax);
        }
    }
}

//////////////////////////////////////////////////////////////////////

void AdaptiveMeshDustGridStructure::write_xyz(DustGridPlotFile* outfile) const
{
    // Output all leaf cells (should we limit this somehow?)
    for (int m=0; m<_Ncells; m++)
    {
        double xmin, ymin, zmin, xmax, ymax, zmax;
        _mesh->extent(m).extent(xmin, ymin, zmin, xmax, ymax, zmax);
        outfile->writeCube(xmin, ymin, zmin, xmax, ymax, zmax);
    }
}

//////////////////////////////////////////////////////////////////////
