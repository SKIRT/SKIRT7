/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "AdaptiveMesh.hpp"
#include "AdaptiveMeshDustGrid.hpp"
#include "AdaptiveMeshInterface.hpp"
#include "DustDistribution.hpp"
#include "DustGridPlotFile.hpp"
#include "FatalError.hpp"
#include "Log.hpp"

using namespace std;

//////////////////////////////////////////////////////////////////////

AdaptiveMeshDustGrid::AdaptiveMeshDustGrid()
    : _random(0), _amesh(0)
{
}

////////////////////////////////////////////////////////////////////

void AdaptiveMeshDustGrid::setupSelfBefore()
{
    DustGrid::setupSelfBefore();

    // get the adaptive mesh and tell it to construct neighbor information for its cells
    DustDistribution* dd = find<DustDistribution>();
    AdaptiveMeshInterface* interface = dd->interface<AdaptiveMeshInterface>();
    if (!interface) throw FATALERROR("Can't find an adaptive mesh in the simulation hierarchy");
    _amesh = interface->mesh();
    find<Log>()->info("Adding neighbor information to adaptive mesh...");
    _amesh->addNeighbors();

    // calculate the normalization factor imposed by the dust distribution
    // we need this to directly compute cell densities for the DustGridDensityInterface
    _nf = dd->mass() / _amesh->integratedDensity();
}

//////////////////////////////////////////////////////////////////////

int AdaptiveMeshDustGrid::dimension() const
{
    return 3;
}

//////////////////////////////////////////////////////////////////////

int AdaptiveMeshDustGrid::numCells() const
{
    return _amesh->Ncells();
}

//////////////////////////////////////////////////////////////////////

Box AdaptiveMeshDustGrid::boundingbox() const
{
    return _amesh->extent();
}

//////////////////////////////////////////////////////////////////////

double AdaptiveMeshDustGrid::volume(int m) const
{
    return _amesh->volume(m);
}

//////////////////////////////////////////////////////////////////////

int AdaptiveMeshDustGrid::whichcell(Position bfr) const
{
    return _amesh->cellIndex(bfr);
}

//////////////////////////////////////////////////////////////////////

Position AdaptiveMeshDustGrid::centralPositionInCell(int m) const
{
    return _amesh->centralPosition(m);
}

//////////////////////////////////////////////////////////////////////

Position AdaptiveMeshDustGrid::randomPositionInCell(int m) const
{
    return _amesh->randomPosition(_random, m);
}

//////////////////////////////////////////////////////////////////////

double AdaptiveMeshDustGrid::density(int h, int m) const
{
    return _nf * _amesh->density(h, m);
}

//////////////////////////////////////////////////////////////////////

void AdaptiveMeshDustGrid::path(DustGridPath* path) const
{
    _amesh->path(path);
}

//////////////////////////////////////////////////////////////////////

void AdaptiveMeshDustGrid::write_xy(DustGridPlotFile* outfile) const
{
    // Output the domain
    double xmin, ymin, zmin, xmax, ymax, zmax;
    boundingbox().extent(xmin, ymin, zmin, xmax, ymax, zmax);
    outfile->writeRectangle(xmin, ymin, xmax, ymax);

    // Output all leaf cells that intersect the coordinate plane
    double eps = 1e-8*(zmax-zmin);
    for (int m=0; m<numCells(); m++)
    {
        _amesh->extent(m).extent(xmin, ymin, zmin, xmax, ymax, zmax);
        if (zmin < eps && zmax > -eps)
        {
            outfile->writeRectangle(xmin, ymin, xmax, ymax);
        }
    }
}

//////////////////////////////////////////////////////////////////////

void AdaptiveMeshDustGrid::write_xz(DustGridPlotFile* outfile) const
{
    // Output the domain
    double xmin, ymin, zmin, xmax, ymax, zmax;
    boundingbox().extent(xmin, ymin, zmin, xmax, ymax, zmax);
    outfile->writeRectangle(xmin, zmin, xmax, zmax);

    // Output all leaf cells that intersect the coordinate plane
    double eps = 1e-8*(ymax-ymin);
    for (int m=0; m<numCells(); m++)
    {
        _amesh->extent(m).extent(xmin, ymin, zmin, xmax, ymax, zmax);
        if (ymin < eps && ymax > -eps)
        {
            outfile->writeRectangle(xmin, zmin, xmax, zmax);
        }
    }
}

//////////////////////////////////////////////////////////////////////

void AdaptiveMeshDustGrid::write_yz(DustGridPlotFile* outfile) const
{
    // Output the domain
    double xmin, ymin, zmin, xmax, ymax, zmax;
    boundingbox().extent(xmin, ymin, zmin, xmax, ymax, zmax);
    outfile->writeRectangle(ymin, zmin, ymax, zmax);

    // Output all leaf cells that intersect the coordinate plane
    double eps = 1e-8*(xmax-xmin);
    for (int m=0; m<numCells(); m++)
    {
        _amesh->extent(m).extent(xmin, ymin, zmin, xmax, ymax, zmax);
        if (xmin < eps && xmax > -eps)
        {
            outfile->writeRectangle(ymin, zmin, ymax, zmax);
        }
    }
}

//////////////////////////////////////////////////////////////////////

void AdaptiveMeshDustGrid::write_xyz(DustGridPlotFile* outfile) const
{
    // Output all leaf cells (should we limit this somehow?)
    for (int m=0; m<numCells(); m++)
    {
        double xmin, ymin, zmin, xmax, ymax, zmax;
        _amesh->extent(m).extent(xmin, ymin, zmin, xmax, ymax, zmax);
        outfile->writeCube(xmin, ymin, zmin, xmax, ymax, zmax);
    }
}

//////////////////////////////////////////////////////////////////////
