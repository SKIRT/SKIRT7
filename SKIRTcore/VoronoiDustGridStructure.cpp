/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "DustDistribution.hpp"
#include "DustGridPlotFile.hpp"
#include "DustParticleInterface.hpp"
#include "FatalError.hpp"
#include "Log.hpp"
#include "Random.hpp"
#include "Units.hpp"
#include "VoronoiDustGridStructure.hpp"
#include "VoronoiMesh.hpp"
#include "VoronoiMeshFile.hpp"
#include "VoronoiMeshInterface.hpp"
#include "container.hh"

using namespace std;

//////////////////////////////////////////////////////////////////////

VoronoiDustGridStructure::VoronoiDustGridStructure()
    : _numParticles(0), _distribution(DustDensity), _meshfile(0), _mesh(0), _meshOwned(true)
{
}

//////////////////////////////////////////////////////////////////////

VoronoiDustGridStructure::~VoronoiDustGridStructure()
{
    if (_meshOwned) delete _mesh;
}

//////////////////////////////////////////////////////////////////////

void VoronoiDustGridStructure::setupSelfBefore()
{
    GenDustGridStructure::setupSelfBefore();
    Log* log = find<Log>();

    // Verify property values
    if (_xmax <= 0.0 || _ymax <= 0.0 || _zmax <= 0.0) throw FATALERROR("The maximum extent should be positive");

    // Determine an appropriate set of particles and construct the Voronoi mesh
    switch (_distribution)
    {
    case Uniform:
        {
            if (_numParticles < 10) throw FATALERROR("The number of particles should be at least 10");
            vector<Vec> rv(_numParticles);
            for (int m=0; m<_numParticles; m++)
            {
                rv[m] = _random->position(extent());
            }
            log->info("Computing Voronoi tesselation for " + QString::number(_numParticles)
                      + " uniformly distributed random particles...");
            _mesh = new VoronoiMesh(rv, extent());
            break;
        }
    case CentralPeak:
        {
            if (_numParticles < 10) throw FATALERROR("The number of particles should be at least 10");
            const int a = 1000;                     // steepness of the peak; the central 1/a portion is NOT covered
            const double rscale = extent().rmax().norm();
            vector<Vec> rv(_numParticles);
            for (int m=1; m<_numParticles; m++)     // skip first particle so that it remains (0,0,0)
            {
                while (true)
                {
                    double r = rscale * pow(1./a, _random->uniform());   // random distribution according to 1/x
                    Direction k = _random->direction();
                    Position p = Position(r,k);
                    if (extent().contains(p))       // discard any points outside of the domain
                    {
                        rv[m] = p;
                        break;
                    }
                }
            }
            log->info("Computing Voronoi tesselation for " + QString::number(_numParticles)
                      + " random particles distributed in a central peak...");
            _mesh = new VoronoiMesh(rv, extent());
            break;
        }
    case DustDensity:
        {
            if (_numParticles < 10) throw FATALERROR("The number of particles should be at least 10");
            DustDistribution* dd = find<DustDistribution>();
            vector<Vec> rv(_numParticles);
            for (int m=0; m<_numParticles; m++)
            {
                while (true)
                {
                    Position p = dd->generatePosition();
                    if (extent().contains(p))       // discard any points outside of the domain
                    {
                        rv[m] = p;
                        break;
                    }
                }
            }
            log->info("Computing Voronoi tesselation for " + QString::number(_numParticles)
                      + " random particles distributed according to dust density...");
            _mesh = new VoronoiMesh(rv, extent());
            break;
        }
    case DustTesselation:
        {
            VoronoiMeshInterface* vmi = find<DustDistribution>()->interface<VoronoiMeshInterface>();
            if (!vmi) throw FATALERROR("Can't retrieve Voronoi mesh from this dust distribution");
            _mesh = vmi->mesh();
            _meshOwned = false;
            log->info("Using Voronoi tesselation from dust distribution with " + QString::number(_mesh->Ncells())
                      + " particles...");
            break;
        }
    case SPHParticles:
        {
            DustParticleInterface* dpi = find<DustDistribution>()->interface<DustParticleInterface>();
            if (!dpi) throw FATALERROR("Can't retrieve particle locations from this dust distribution");
            log->info("Computing Voronoi tesselation for " + QString::number(dpi->numParticles())
                      + " dust distribution particles...");
            _mesh = new VoronoiMesh(dpi, extent());
            break;
        }
    case File:
        {
            if (!_meshfile) throw FATALERROR("File containing particle locations is not defined");
            log->info("Computing Voronoi tesselation for particles loaded from file " + _meshfile->filename() + "...");
            _mesh = new VoronoiMesh(_meshfile, QList<int>(), extent());
            break;
        }
    default:
        throw FATALERROR("Unknown distribution type");
    }

    // Communicate the number of dust cells to the base class
    _Ncells = _mesh->Ncells();

    // Log statistics on the cell neighbors
    double avgNeighbors;
    int minNeighbors, maxNeighbors;
    _mesh->neighborStatistics(avgNeighbors, minNeighbors, maxNeighbors);
    log->info("Computed Voronoi tesselation with " + QString::number(_Ncells) + " cells:");
    log->info("  Average number of neighbors per cell: " + QString::number(avgNeighbors,'f',1));
    log->info("  Minimum number of neighbors per cell: " + QString::number(minNeighbors));
    log->info("  Maximum number of neighbors per cell: " + QString::number(maxNeighbors));

    // Log statistics on the block lists
    int nblocks = _mesh->Nblocks();
    double avgRefsPerBlock;
    int minRefsPerBlock, maxRefsPerBlock;
    _mesh->blockStatistics(avgRefsPerBlock, minRefsPerBlock, maxRefsPerBlock);
    log->info("Created grid to accelerate which-cell operations:");
    log->info("  Number of cells                  : " + QString::number(_Ncells));
    log->info("  Number of blocks                 : " + QString::number(nblocks*nblocks*nblocks) +
              " (" + QString::number(nblocks) + " in each dimension)");
    log->info("  Average number of cells per block: " + QString::number(avgRefsPerBlock,'f',1));
    log->info("  Minimum number of cells per block: " + QString::number(minRefsPerBlock));
    log->info("  Maximum number of cells per block: " + QString::number(maxRefsPerBlock));

    // Log statistics on the search trees
    double avgRefsPerTree;
    int nTrees, minRefsPerTree, maxRefsPerTree;
    _mesh->treeStatistics(nTrees, avgRefsPerTree, minRefsPerTree, maxRefsPerTree);
    log->info("Created search trees to accelerate which-cell operations:");
    log->info("  Number of trees                  : " + QString::number(nTrees) +
              " (" + QString::number(100.*nTrees/(nblocks*nblocks*nblocks),'f',1) + "% of blocks)");
    log->info("  Average number of cells per tree : " + QString::number(avgRefsPerTree,'f',1));
    log->info("  Minimum number of cells per tree : " + QString::number(minRefsPerTree));
    log->info("  Maximum number of cells per tree : " + QString::number(maxRefsPerTree));

    // If requested, output the plot files (we have to reconstruct the Voronoi tesselation...)
    if (writeGrid())
    {
        setWriteGrid(false);    // keep the base class from overwriting our plot files

        // create the plot files
        DustGridPlotFile plotxy(this, "ds_gridxy");
        DustGridPlotFile plotxz(this, "ds_gridxz");
        DustGridPlotFile plotyz(this, "ds_gridyz");
        DustGridPlotFile plotxyz(this, "ds_gridxyz");

        // load all particles in a Voro container
        int nb = max(3, min(1000, static_cast<int>(pow(_Ncells/5.,1./3.)) ));
        voro::container con(xmin(), xmax(), ymin(), ymax(), zmin(), zmax(), nb, nb, nb, false,false,false, 8);
        for (int m=0; m<_Ncells; m++)
        {
            Vec r = _mesh->particlePosition(m);
            con.put(m, r.x(),r.y(),r.z());
        }

        // loop over all Voro cells
        voro::c_loop_all loop(con);
        if (loop.start()) do
        {
            // Compute the cell
            voro::voronoicell fullcell;
            con.compute_cell(fullcell, loop);

            // Get the edges of the cell
            double x,y,z;
            loop.pos(x,y,z);
            vector<double> coords;
            fullcell.vertices(x,y,z, coords);
            vector<int> indices;
            fullcell.face_vertices(indices);

            // Write the edges of the cell to the plot files
            Box bounds = _mesh->extent(loop.pid());
            if (bounds.zmin()<=0 && bounds.zmax()>=0) plotxy.writePolyhedron(coords, indices);
            if (bounds.ymin()<=0 && bounds.ymax()>=0) plotxz.writePolyhedron(coords, indices);
            if (bounds.xmin()<=0 && bounds.xmax()>=0) plotyz.writePolyhedron(coords, indices);
            if (loop.pid() <= 1000) plotxyz.writePolyhedron(coords, indices);
        }
        while (loop.inc());
    }
}

//////////////////////////////////////////////////////////////////////

void VoronoiDustGridStructure::setExtentX(double value)
{
    _xmax = value;
    _xmin = - value;
}

//////////////////////////////////////////////////////////////////////

double VoronoiDustGridStructure::extentX() const
{
    return _xmax;
}

//////////////////////////////////////////////////////////////////////

void VoronoiDustGridStructure::setExtentY(double value)
{
    _ymax = value;
    _ymin = - value;
}

//////////////////////////////////////////////////////////////////////

double VoronoiDustGridStructure::extentY() const
{
    return _ymax;
}

//////////////////////////////////////////////////////////////////////

void VoronoiDustGridStructure::setExtentZ(double value)
{
    _zmax = value;
    _zmin = - value;
}

//////////////////////////////////////////////////////////////////////

double VoronoiDustGridStructure::extentZ() const
{
    return _zmax;
}

//////////////////////////////////////////////////////////////////////

void VoronoiDustGridStructure::setNumParticles(int value)
{
    _numParticles = value;
}

//////////////////////////////////////////////////////////////////////

int VoronoiDustGridStructure::numParticles() const
{
    return _numParticles;
}

//////////////////////////////////////////////////////////////////////

void VoronoiDustGridStructure::setDistribution(VoronoiDustGridStructure::Distribution value)
{
    _distribution = value;
}

//////////////////////////////////////////////////////////////////////

VoronoiDustGridStructure::Distribution VoronoiDustGridStructure::distribution() const
{
    return _distribution;
}

//////////////////////////////////////////////////////////////////////

void VoronoiDustGridStructure::setVoronoiMeshFile(VoronoiMeshFile* value)
{
    if (_meshfile) delete _meshfile;
    _meshfile = value;
    if (_meshfile) _meshfile->setParent(this);
}

//////////////////////////////////////////////////////////////////////

VoronoiMeshFile* VoronoiDustGridStructure::voronoiMeshFile() const
{
    return _meshfile;
}
//////////////////////////////////////////////////////////////////////

double VoronoiDustGridStructure::xmax() const
{
    return _xmax;
}

//////////////////////////////////////////////////////////////////////

double VoronoiDustGridStructure::ymax() const
{
    return _ymax;
}

//////////////////////////////////////////////////////////////////////

double VoronoiDustGridStructure::zmax() const
{
    return _zmax;
}

//////////////////////////////////////////////////////////////////////

double VoronoiDustGridStructure::volume(int m) const
{
    return _mesh->volume(m);
}

//////////////////////////////////////////////////////////////////////

int VoronoiDustGridStructure::whichcell(Position bfr) const
{
    return _mesh->cellIndex(bfr);
}

//////////////////////////////////////////////////////////////////////

Position VoronoiDustGridStructure::centralPositionInCell(int m) const
{
    return _mesh->centralPosition(m);
}

//////////////////////////////////////////////////////////////////////

Position VoronoiDustGridStructure::randomPositionInCell(int m) const
{
    return _mesh->randomPosition(_random, m);
}

//////////////////////////////////////////////////////////////////////

void VoronoiDustGridStructure::path(DustGridPath* path) const
{
    _mesh->path(path);
}

//////////////////////////////////////////////////////////////////////
