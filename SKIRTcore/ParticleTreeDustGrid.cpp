/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include <cfloat>
#include "BinTreeNode.hpp"
#include "DustDistribution.hpp"
#include "DustGridPath.hpp"
#include "DustGridPlotFile.hpp"
#include "DustMassInBoxInterface.hpp"
#include "DustParticleInterface.hpp"
#include "FatalError.hpp"
#include "Log.hpp"
#include "OctTreeNode.hpp"
#include "ParticleTreeDustGrid.hpp"
#include "Random.hpp"

using namespace std;

//////////////////////////////////////////////////////////////////////

ParticleTreeDustGrid::ParticleTreeDustGrid()
    :  _treeType(OctTree), _extraLevels(0), _random(0), _eps(0), _dmib(0), _highestWriteLevel(0)
{
}

//////////////////////////////////////////////////////////////////////

ParticleTreeDustGrid::~ParticleTreeDustGrid()
{
    int n = _tree.size();
    for (int l=0; l<n; l++) delete _tree[l];
}

//////////////////////////////////////////////////////////////////////

namespace
{
    // private function to add a particle to a node, performing recursive subdivision if needed;
    // returns the level of the leaf node to which the particle was finally added,
    // or -1 if the particle is outside the node
    int addParticleToNode(int newparticle, TreeNode* parent,
                          DustParticleInterface* dpi, vector<int>& particlev, vector<TreeNode*>& tree)
    {
        // find the leaf node that contains this particle
        TreeNode* node = const_cast<TreeNode*>(parent->whichnode(dpi->particleCenter(newparticle)));
        if (!node) return -1;
        int id = node->id();

        // if the leaf node is still empty, just add the particle to it
        if (particlev[id]<0)
        {
            particlev[id] = newparticle;
            return node->level();
        }

        // if the leaf node already contains a particle, subdivide the node,
        // and add both the old and new particles to the appropriate child
        node->createchildren(tree.size());
        const vector<TreeNode*>& children = node->children();
        int n = children.size();
        for (int i = 0; i<n; i++)
        {
            tree.push_back(children[i]);
            particlev.push_back(-1);
        }
        addParticleToNode(particlev[id], node, dpi, particlev, tree);
        return addParticleToNode(newparticle, node, dpi, particlev, tree);
    }
}

//////////////////////////////////////////////////////////////////////

void ParticleTreeDustGrid::setupSelfBefore()
{
    BoxDustGrid::setupSelfBefore();

    // Cache some often used values
    _random = find<Random>();
    Log* log = find<Log>();
    _eps = 1e-12 * extent().widths().norm();
    DustDistribution* dd = find<DustDistribution>();
    _dmib = dd->interface<DustMassInBoxInterface>();
    DustParticleInterface* dpi = dd->interface<DustParticleInterface>();
    if (!dpi) throw FATALERROR("Can't retrieve particle locations from this dust distribution");
    int numParticles = dpi->numParticles();
    log->info("Constructing tree for " + QString::number(numParticles) + " particles...");

    // Create a list, used only during construction, that contains the index of the particle
    // contained in each leaf node so far created, or -1 if the node is empty
    // (the value is undefined for nonleaf nodes)
    vector<int> particlev;

    // Create the root node (which at this point is an empty leaf) using the requested type
    switch (_treeType)
    {
    default:
    case OctTree:
        _tree.push_back(new OctTreeNode(0,0,extent()));
        break;
    case BinTree:
        _tree.push_back(new BinTreeNode(0,0,extent()));
        break;
    }
    particlev.push_back(-1);
    int maxlevel = 0;

    // Add particles one by one, subdividing if the leaf node containing the new particle
    // already contains another particle
    for (int i=0; i<numParticles; i++)
    {
        if (i%50000 == 0) log->info("Adding particle number " + QString::number(i) +
                                    " (" + QString::number(i*100/numParticles) + "%)...");
        int level = addParticleToNode(i, root(), dpi, particlev, _tree);
        maxlevel = max(maxlevel, level);
    }

    // Perform additional subdivisions as requested
    if (_extraLevels>0)
    {
        log->info("Performing additional subdivisions...");
        maxlevel += _extraLevels;
        for (int e=0; e<_extraLevels; e++)
        {
            int Nnodes = _tree.size();
            for (int l=0; l<Nnodes; l++)
            {
                TreeNode* node = _tree[l];
                if (node->ynchildless())
                {
                    node->createchildren(_tree.size());
                    _tree.insert(_tree.end(), node->children().begin(), node->children().end());
                }
            }
        }
    }

    // Construction of a vector _idv that contains the node IDs of all
    // leaves. This is the actual dust cell vector (only the leaves will
    // eventually become valid dust cells). We also create a vector
    // _cellnumberv with the cell numbers of all the nodes (i.e. the
    // rank m of the node in the vector _idv if the node is a leaf, and
    // -1 if the node is not a leaf).
    int Nnodes = _tree.size();
    int m = 0;
    _cellnumberv.resize(Nnodes,-1);
    for (int l=0; l<Nnodes; l++)
    {
        if (_tree[l]->ynchildless())
        {
            _idv.push_back(l);
            _cellnumberv[l] = m;
            m++;
        }
    }
    setNumCells(_idv.size());

    // Log the number of cells
    log->info("Construction of the tree finished.");
    log->info("  Total number of nodes: " + QString::number(Nnodes));
    log->info("  Total number of leaves: " + QString::number(numCells()));
    vector<int> countv(maxlevel+1);
    for (int m=0; m<numCells(); m++)
    {
        TreeNode* node = _tree[_idv[m]];
        int level = node->level();
        countv[level]++;
    }
    log->info("  Number of leaf cells of each level:");
    for (int level=0; level<=maxlevel; level++)
        log->info("    Level " + QString::number(level) + ": " + QString::number(countv[level]) + " cells");

    // Determine the number of levels to be included in 3D grid output (if such output is requested)
    if (writeGrid())
    {
        int cumulativeCells = 0;
        for (_highestWriteLevel=0; _highestWriteLevel<=maxlevel; _highestWriteLevel++)
        {
            cumulativeCells += countv[_highestWriteLevel];
            if (cumulativeCells > 1500) break;          // experimental number
        }
        if (_highestWriteLevel<maxlevel)
            log->info("Will be outputting 3D grid data up to level " + QString::number(_highestWriteLevel) +
                      ", i.e. " + QString::number(cumulativeCells) + " cells.");
    }
}

//////////////////////////////////////////////////////////////////////

void ParticleTreeDustGrid::setTreeType(ParticleTreeDustGrid::TreeType value)
{
    _treeType = value;
}

//////////////////////////////////////////////////////////////////////

ParticleTreeDustGrid::TreeType ParticleTreeDustGrid::treeType() const
{
    return _treeType;
}

//////////////////////////////////////////////////////////////////////

void ParticleTreeDustGrid::setExtraLevels(int value)
{
    _extraLevels = value;
}

//////////////////////////////////////////////////////////////////////

int ParticleTreeDustGrid::extraLevels() const
{
    return _extraLevels;
}

//////////////////////////////////////////////////////////////////////

double ParticleTreeDustGrid::volume(int m) const
{
    if (m<0 || m>numCells())
        throw FATALERROR("Invalid cell number: " + QString::number(m));
    TreeNode* node = getnode(m);
    return node->xwidth() * node->ywidth() * node->zwidth();
}

//////////////////////////////////////////////////////////////////////

int ParticleTreeDustGrid::whichcell(Position bfr) const
{
    const TreeNode* node = root()->whichnode(bfr);
    return node ? cellnumber(node) : -1;
}

//////////////////////////////////////////////////////////////////////

Position ParticleTreeDustGrid::centralPositionInCell(int m) const
{
    return Position(getnode(m)->extent().center());
}

//////////////////////////////////////////////////////////////////////

Position ParticleTreeDustGrid::randomPositionInCell(int m) const
{
    return _random->position(getnode(m)->extent());
}

//////////////////////////////////////////////////////////////////////

void ParticleTreeDustGrid::path(DustGridPath* path) const
{
    // Initialize the path
    path->clear();

    // If the photon package starts outside the dust grid, move it into the first grid cell that it will pass
    Position r = path->moveInside(extent(), _eps);

    // Get the node containing the current location;
    // if the position is not inside the grid, return an empty path
    const TreeNode* node = root()->whichnode(r);
    if (!node) return path->clear();

    // Start the loop over nodes/path segments until we leave the grid.
    // Use a different code segment depending on the search method.
    double x,y,z;
    r.cartesian(x,y,z);
    double kx,ky,kz;
    path->direction().cartesian(kx,ky,kz);

    while (node)
    {
        // calculate the distances to the planes of the node walls that are forward of this point
        double xnext = (kx<0.0) ? node->xmin() : node->xmax();
        double ynext = (ky<0.0) ? node->ymin() : node->ymax();
        double znext = (kz<0.0) ? node->zmin() : node->zmax();
        double dsx = (xnext-x)/kx;
        double dsy = (ynext-y)/ky;
        double dsz = (znext-z)/kz;

        // find the distance to the nearest wall (avoiding the wall containing the entry point)
        // and add the corresponding path segment (unless there is no exit point due to rounding errors)
        double ds = DBL_MAX;  // very large, but not infinity (so that infinite dsi values are discarded)
        if (dsx>0 && dsx<ds) ds = dsx;
        if (dsy>0 && dsy<ds) ds = dsy;
        if (dsz>0 && dsz<ds) ds = dsz;
        if (ds<DBL_MAX)
            path->addSegment(cellnumber(node), ds);
        else
            ds = 0;

        // advance the current point
        x += (ds+_eps)*kx;
        y += (ds+_eps)*ky;
        z += (ds+_eps)*kz;

        // always search from the root node down
        const TreeNode* oldnode = node;
        node = root()->whichnode(Vec(x,y,z));

        // if we're stuck in the same node...
        if (node==oldnode)
        {
            // try to escape by advancing the position to the next representable coordinates
            find<Log>()->warning("Photon package seems stuck in dust cell "
                                 + QString::number(node->id()) + " -- escaping");
            x = nextafter(x, (kx<0.0) ? -DBL_MAX : DBL_MAX);
            y = nextafter(y, (ky<0.0) ? -DBL_MAX : DBL_MAX);
            z = nextafter(z, (kz<0.0) ? -DBL_MAX : DBL_MAX);
            node = root()->whichnode(Vec(x,y,z));

            // if that didn't work, terminate the path
            if (node==oldnode)
            {
                find<Log>()->warning("Photon package is stuck in dust cell "
                                     + QString::number(node->id()) + " -- terminating this path");
                break;
            }
        }
    }
}

//////////////////////////////////////////////////////////////////////

QList<SimulationItem*> ParticleTreeDustGrid::interfaceCandidates(const type_info &interfaceTypeInfo)
{
    if (interfaceTypeInfo == typeid(DustGridDensityInterface) && !_dmib)
        return QList<SimulationItem*>();
    return BoxDustGrid::interfaceCandidates(interfaceTypeInfo);
}

//////////////////////////////////////////////////////////////////////

double ParticleTreeDustGrid::density(int h, int m) const
{
    TreeNode* node = getnode(m);
    return _dmib->massInBox(h, node->extent()) / node->volume();
}

//////////////////////////////////////////////////////////////////////

void ParticleTreeDustGrid::write_xy(DustGridPlotFile* outfile) const
{
    // Output the root cell and all leaf cells that are close to the section plane
    outfile->writeRectangle(_xmin, _ymin, _xmax, _ymax);
    for (int m=0; m<numCells(); m++)
    {
        TreeNode* node = getnode(m);
        if (fabs(node->zmin()) < 1e-8*extent().zwidth())
        {
            outfile->writeRectangle(node->xmin(), node->ymin(), node->xmax(), node->ymax());
        }
    }
}

//////////////////////////////////////////////////////////////////////

void ParticleTreeDustGrid::write_xz(DustGridPlotFile* outfile) const
{
    // Output the root cell and all leaf cells that are close to the section plane
    outfile->writeRectangle(_xmin, _zmin, _xmax, _zmax);
    for (int m=0; m<numCells(); m++)
    {
        TreeNode* node = getnode(m);
        if (fabs(node->ymin()) < 1e-8*extent().ywidth())
        {
            outfile->writeRectangle(node->xmin(), node->zmin(), node->xmax(), node->zmax());
        }
    }
}

//////////////////////////////////////////////////////////////////////

void ParticleTreeDustGrid::write_yz(DustGridPlotFile* outfile) const
{
    // Output the root cell and all leaf cells that are close to the section plane
    outfile->writeRectangle(_ymin, _zmin, _ymax, _zmax);
    for (int m=0; m<numCells(); m++)
    {
        TreeNode* node = getnode(m);
        if (fabs(node->xmin()) < 1e-8*extent().xwidth())
        {
            outfile->writeRectangle(node->ymin(), node->zmin(), node->ymax(), node->zmax());
        }
    }
}

//////////////////////////////////////////////////////////////////////

void ParticleTreeDustGrid::write_xyz(DustGridPlotFile* outfile) const
{
    // Output all leaf cells up to a certain level
    for (int m=0; m<numCells(); m++)
    {
        TreeNode* node = getnode(m);
        if (node->level() <= _highestWriteLevel)
            outfile->writeCube(node->xmin(), node->ymin(), node->zmin(), node->xmax(), node->ymax(), node->zmax());
    }
}

//////////////////////////////////////////////////////////////////////

TreeNode* ParticleTreeDustGrid::root() const
{
    return _tree[0];
}

//////////////////////////////////////////////////////////////////////

TreeNode* ParticleTreeDustGrid::getnode(int m) const
{
    return _tree[_idv[m]];
}

//////////////////////////////////////////////////////////////////////

int ParticleTreeDustGrid::cellnumber(const TreeNode* node) const
{
    return _cellnumberv[node->id()];
}

//////////////////////////////////////////////////////////////////////
