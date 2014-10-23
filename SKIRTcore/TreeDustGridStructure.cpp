/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include <cfloat>
#include <cmath>
#include "DustDistribution.hpp"
#include "DustGridPath.hpp"
#include "DustGridPlotFile.hpp"
#include "FatalError.hpp"
#include "Log.hpp"
#include "NR.hpp"
#include "Parallel.hpp"
#include "ParallelFactory.hpp"
#include "Random.hpp"
#include "TreeDustGridStructure.hpp"
#include "TreeNode.hpp"
#include "TreeNodeBoxDensityCalculator.hpp"
#include "TreeNodeSampleDensityCalculator.hpp"
#include "Units.hpp"

using namespace std;

//////////////////////////////////////////////////////////////////////

TreeDustGridStructure::TreeDustGridStructure()
    : _minlevel(0), _maxlevel(0),
      _search(TopDown), _Nrandom(100),
      _maxOpticalDepth(0), _maxMassFraction(0), _maxDensDispFraction(0),
      _parallel(0), _dd(0), _dmib(0),
      _totalmass(0), _eps(0),
      _Nnodes(0), _highestWriteLevel(0),
      _useDmibForSubdivide(false)
{
}

//////////////////////////////////////////////////////////////////////

TreeDustGridStructure::~TreeDustGridStructure()
{
    for (int l=0; l<_Nnodes; l++)
        delete _tree[l];
}

//////////////////////////////////////////////////////////////////////

void TreeDustGridStructure::setupSelfBefore()
{
    GenDustGridStructure::setupSelfBefore();
    Log* log = find<Log>();

    // Validate attribute values

    if (_maxlevel <= _minlevel)
        throw FATALERROR("Maximum tree level should be larger than minimum tree level");

    // Cache some often used values

    _parallel = find<ParallelFactory>()->parallel(4);   // limit number of threads for performance reasons
    _dd = find<DustDistribution>();
    _dmib = _dd->interface<DustMassInBoxInterface>();
    _useDmibForSubdivide = _dmib && !_maxDensDispFraction;
    _totalmass = _dd->mass();
    _eps = 1e-12 * extent().widths().norm();

    // Create the root node

    _tree.push_back(createRoot(extent()));

    // Recursively subdivide the root node until all nodes satisfy the
    // necessary criteria. When finished, set the number _Nnodes.

    int currentlevel = -1;
    unsigned int l = 0;
    while (true)
    {
        TreeNode* node = _tree[l];
        int level = node->level();
        if (level>currentlevel)
        {
            log->info("Starting subdivision of level " + QString::number(level) + "...");
            currentlevel = level;
        }
        if (l%50000 == 0)
            log->info("Subdividing node number " + QString::number(l) + "...");
        if (node->ynchildless())
            subdivide(node);
        l++;
        if (l>=_tree.size()) break;
    }
    _Nnodes = _tree.size();

    // Construction of a vector _idv that contains the node IDs of all
    // leaves. This is the actual dust cell vector (only the leaves will
    // eventually become valid dust cells). We also create a vector
    // _cellnumberv with the cell numbers of all the nodes (i.e. the
    // rank m of the node in the vector _idv if the node is a leaf, and
    // -1 if the node is not a leaf).

    int m = 0;
    _cellnumberv.resize(_Nnodes,-1);
    for (int l=0; l<_Nnodes; l++)
    {
        if (_tree[l]->ynchildless())
        {
            _idv.push_back(l);
            _cellnumberv[l] = m;
            m++;
        }
    }
    _Ncells = _idv.size();

    // Log the number of cells

    log->info("Construction of the tree finished.");
    log->info("  Total number of nodes: " + QString::number(_Nnodes));
    log->info("  Total number of leaves: " + QString::number(_Ncells));
    vector<int> countv(_maxlevel+1);
    for (int m=0; m<_Ncells; m++)
    {
        TreeNode* node = _tree[_idv[m]];
        int level = node->level();
        countv[level]++;
    }
    log->info("  Number of leaf cells of each level:");
    for (int level=0; level<=_maxlevel; level++)
        log->info("    Level " + QString::number(level) + ": " + QString::number(countv[level]) + " cells");

    // Determine the number of levels to be included in 3D grid output (if such output is requested)

    if (writeGrid())
    {
        int cumulativeCells = 0;
        for (_highestWriteLevel=0; _highestWriteLevel<=_maxlevel; _highestWriteLevel++)
        {
            cumulativeCells += countv[_highestWriteLevel];
            if (cumulativeCells > 1500) break;          // experimental number
        }
        if (_highestWriteLevel<_maxlevel)
            log->info("Will be outputting 3D grid data up to level " + QString::number(_highestWriteLevel) +
                      ", i.e. " + QString::number(cumulativeCells) + " cells.");
    }

    // Add neighbors to the tree structure (but only if required for the search method)

    if (_search == Neighbor)
    {
        log->info("Adding neighbors to the tree nodes...");
        for (int l=0; l<_Nnodes; l++) _tree[l]->addneighbors();
        for (int l=0; l<_Nnodes; l++) _tree[l]->sortneighbors();
    }
}

//////////////////////////////////////////////////////////////////////

void TreeDustGridStructure::subdivide(TreeNode* node)
{
    // If level is below or at minlevel, there is always subdivision, and the subdivision is "regular"
    int level = node->level();
    if (level <= _minlevel)
    {
        node->createchildren(_tree.size());
        _tree.insert(_tree.end(), node->children().begin(), node->children().end());
    }

    // if level is below maxlevel, there may be subdivision depending on various stopping criteria
    else if (level < _maxlevel)
    {
        // construct an appropriate density calculator to estimate properties for stopping criteria and division
        TreeNodeDensityCalculator* calc;
        if (_useDmibForSubdivide)
        {
            // use the DustMassInBox interface
            calc = new TreeNodeBoxDensityCalculator(_dmib, node);
        }
        else
        {
            // sample the density in the cell
            TreeNodeSampleDensityCalculator* sampleCalc =
                    new TreeNodeSampleDensityCalculator(_random, _Nrandom, _dd, node);
            _parallel->call(sampleCalc, _Nrandom);
            calc = sampleCalc;
        }

        // if no stopping criteria are enabled, we keep subdividing indefinitely
        bool needDivision = (_maxOpticalDepth == 0 && _maxMassFraction == 0 && _maxDensDispFraction == 0);

        // otherwise, we subdivide if at least one stopping criterion is not satisfied

        // check mass fraction
        if (!needDivision && _maxMassFraction > 0)
        {
            double massfraction = calc->mass() / _totalmass;
            if (massfraction >= _maxMassFraction) needDivision = true;
        }

        // check optical depth
        if (!needDivision && _maxOpticalDepth > 0)
        {
            double opticaldepth = calc->opticalDepth();
            if (opticaldepth >= _maxOpticalDepth) needDivision = true;
        }

        // check density dispersion fraction
        if (!needDivision && _maxDensDispFraction > 0)
        {
            double densdispfraction = calc->densityDispersion();
            if (densdispfraction >= _maxDensDispFraction) needDivision = true;
        }

        if (needDivision)
        {
            // there is subdivision, possibly using calculated properties such as barycenter
            node->createchildren(_tree.size(), calc);
            _tree.insert(_tree.end(), node->children().begin(), node->children().end());
        }

        delete calc;
    }
}

//////////////////////////////////////////////////////////////////////

void TreeDustGridStructure::setExtentX(double value)
{
    if (value <= 0.0) throw FATALERROR("The maximum extent (in the X direction) should be positive");
    _xmax = value;
    _xmin = - value;
}

//////////////////////////////////////////////////////////////////////

double TreeDustGridStructure::extentX() const
{
    return _xmax;
}

//////////////////////////////////////////////////////////////////////

void TreeDustGridStructure::setExtentY(double value)
{
    if (value <= 0.0) throw FATALERROR("The maximum extent (in the X direction) should be positive");
    _ymax = value;
    _ymin = - value;
}

//////////////////////////////////////////////////////////////////////

double TreeDustGridStructure::extentY() const
{
    return _ymax;
}

//////////////////////////////////////////////////////////////////////

void TreeDustGridStructure::setExtentZ(double value)
{
    if (value <= 0.0) throw FATALERROR("The maximum extent (in the X direction) should be positive");
    _zmax = value;
    _zmin = - value;
}

//////////////////////////////////////////////////////////////////////

double TreeDustGridStructure::extentZ() const
{
    return _zmax;
}

//////////////////////////////////////////////////////////////////////

void TreeDustGridStructure::setMinLevel(int value)
{
    if (value < 0) throw FATALERROR("The minimum tree level should be at least 0");
    if (value > 50) throw FATALERROR("The minimum tree level should be at most 50");
    _minlevel = value;
}

//////////////////////////////////////////////////////////////////////

int TreeDustGridStructure::minLevel() const
{
    return _minlevel;
}

//////////////////////////////////////////////////////////////////////

void TreeDustGridStructure::setMaxLevel(int value)
{
    if (value < 2) throw FATALERROR("The maximum tree level should be at least 2");
    if (value > 50) throw FATALERROR("The minimum tree level should be at most 50");
    _maxlevel = value;
}

//////////////////////////////////////////////////////////////////////

int TreeDustGridStructure::maxLevel() const
{
    return _maxlevel;
}

//////////////////////////////////////////////////////////////////////

void TreeDustGridStructure::setSearchMethod(TreeDustGridStructure::SearchMethod value)
{
    _search = value;
}

//////////////////////////////////////////////////////////////////////

TreeDustGridStructure::SearchMethod TreeDustGridStructure::searchMethod() const
{
    return _search;
}

//////////////////////////////////////////////////////////////////////

void TreeDustGridStructure::setSampleCount(int value)
{
    if (value < 10) throw FATALERROR("Number of random samples must be at least 10");
    _Nrandom = value;
}

//////////////////////////////////////////////////////////////////////

int TreeDustGridStructure::sampleCount() const
{
    return _Nrandom;
}

//////////////////////////////////////////////////////////////////////

void TreeDustGridStructure::setMaxOpticalDepth(double value)
{
    if (value < 0.0) throw FATALERROR("The maximum mean optical depth should be positive");
    _maxOpticalDepth = value;
}

//////////////////////////////////////////////////////////////////////

double TreeDustGridStructure::maxOpticalDepth() const
{
    return _maxOpticalDepth;
}

//////////////////////////////////////////////////////////////////////

void TreeDustGridStructure::setMaxMassFraction(double value)
{
    if (value < 0.0) throw FATALERROR("The maximum mass fraction should be positive");
    _maxMassFraction = value;
}

//////////////////////////////////////////////////////////////////////

double TreeDustGridStructure::maxMassFraction() const
{
    return _maxMassFraction;
}

//////////////////////////////////////////////////////////////////////

void TreeDustGridStructure::setMaxDensDispFraction(double value)
{
    if (value < 0.0) throw FATALERROR("The maximum density dispersion fraction should be positive");
    _maxDensDispFraction = value;
}

//////////////////////////////////////////////////////////////////////

double TreeDustGridStructure::maxDensDispFraction() const
{
    return _maxDensDispFraction;
}

//////////////////////////////////////////////////////////////////////

double TreeDustGridStructure::xmax() const
{
    return _xmax;
}

//////////////////////////////////////////////////////////////////////

double TreeDustGridStructure::ymax() const
{
    return _ymax;
}

//////////////////////////////////////////////////////////////////////

double TreeDustGridStructure::zmax() const
{
    return _zmax;
}

//////////////////////////////////////////////////////////////////////

double TreeDustGridStructure::volume(int m) const
{
    if (m<0 || m>_Ncells)
        throw FATALERROR("Invalid cell number: " + QString::number(m));
    TreeNode* node = getnode(m);
    return node->xwidth() * node->ywidth() * node->zwidth();
}

//////////////////////////////////////////////////////////////////////

int TreeDustGridStructure::whichcell(Position bfr) const
{
    const TreeNode* node = root()->whichnode(bfr);
    return node ? cellnumber(node) : -1;
}

//////////////////////////////////////////////////////////////////////

Position TreeDustGridStructure::centralPositionInCell(int m) const
{
    return Position(getnode(m)->extent().center());
}

//////////////////////////////////////////////////////////////////////

Position TreeDustGridStructure::randomPositionInCell(int m) const
{
    return _random->position(getnode(m)->extent());
}

//////////////////////////////////////////////////////////////////////

void TreeDustGridStructure::path(DustGridPath* path) const
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

    // ----------- Top-down -----------

    if (_search == TopDown)
    {
        while (node)
        {
            double xnext = (kx<0.0) ? node->xmin() : node->xmax();
            double ynext = (ky<0.0) ? node->ymin() : node->ymax();
            double znext = (kz<0.0) ? node->zmin() : node->zmax();
            double dsx = (fabs(kx)>1e-15) ? (xnext-x)/kx : DBL_MAX;
            double dsy = (fabs(ky)>1e-15) ? (ynext-y)/ky : DBL_MAX;
            double dsz = (fabs(kz)>1e-15) ? (znext-z)/kz : DBL_MAX;

            double ds;
            if (dsx<=dsy && dsx<=dsz) ds = dsx;
            else if (dsy<=dsx && dsy<=dsz) ds = dsy;
            else ds = dsz;
            path->addSegment(cellnumber(node), ds);
            x += (ds+_eps)*kx;
            y += (ds+_eps)*ky;
            z += (ds+_eps)*kz;

            // always search from the root node down
            node = root()->whichnode(Vec(x,y,z));
        }
    }

    // ----------- Neighbor -----------

    else if (_search == Neighbor)
    {
        while (node)
        {
            double xnext = (kx<0.0) ? node->xmin() : node->xmax();
            double ynext = (ky<0.0) ? node->ymin() : node->ymax();
            double znext = (kz<0.0) ? node->zmin() : node->zmax();
            double dsx = (fabs(kx)>1e-15) ? (xnext-x)/kx : DBL_MAX;
            double dsy = (fabs(ky)>1e-15) ? (ynext-y)/ky : DBL_MAX;
            double dsz = (fabs(kz)>1e-15) ? (znext-z)/kz : DBL_MAX;

            double ds;
            TreeNode::Wall wall;
            if (dsx<=dsy && dsx<=dsz)
            {
                ds = dsx;
                wall = (kx<0.0) ? TreeNode::BACK : TreeNode::FRONT;
            }
            else if (dsy<=dsx && dsy<=dsz)
            {
                ds = dsy;
                wall = (ky<0.0) ? TreeNode::LEFT : TreeNode::RIGHT;
            }
            else
            {
                ds = dsz;
                wall = (kz<0.0) ? TreeNode::BOTTOM : TreeNode::TOP;
            }
            path->addSegment(cellnumber(node), ds);
            x += (ds+_eps)*kx;
            y += (ds+_eps)*ky;
            z += (ds+_eps)*kz;

            // attempt to find the new node among the neighbors of the current node;
            // this should not fail unless the new location is outside the grid,
            // however on rare occasions it fails due to rounding errors (e.g. in a corner),
            // thus we use top-down search as a fall-back
            node = node->whichnode(wall, Vec(x,y,z));
            if (!node) node = root()->whichnode(Vec(x,y,z));
        }
    }

    // ----------- Bookkeeping -----------

    // !! This code section relies on the fact that an octtree node is used !!

    else if (_search == Bookkeeping)
    {
        int l = node->id();  // node index in the tree
        while (true)
        {
            double xnext = (kx<0.0) ? _tree[l]->xmin() : _tree[l]->xmax();
            double ynext = (ky<0.0) ? _tree[l]->ymin() : _tree[l]->ymax();
            double znext = (kz<0.0) ? _tree[l]->zmin() : _tree[l]->zmax();
            double dsx = (fabs(kx)>1e-15) ? (xnext-x)/kx : DBL_MAX;
            double dsy = (fabs(ky)>1e-15) ? (ynext-y)/ky : DBL_MAX;
            double dsz = (fabs(kz)>1e-15) ? (znext-z)/kz : DBL_MAX;

            // First option: the x-wall is hit first. After moving
            // towards the boundary, we have to find the next cell. First we
            // check whether the node is on the right or left side of his
            // father node. If the movement is towards positive x (i.e. if
            // kx>0) we move up in the tree until we find a node on the left
            // side. The next cell will then be the corresponding right node
            // (if it is a leaf) or one of its children. If we have to move
            // up until we hit the root node, this means our path has ended.

            if (dsx<=dsy && dsx<=dsz)
            {
                path->addSegment(_cellnumberv[l], dsx);
                x = xnext;
                y += ky*dsx;
                z += kz*dsx;
                while (true)
                {
                    int oct = ((l - 1) % 8) + 1;
                    bool place = (kx<0.0) ? (oct % 2 == 1) : (oct % 2 == 0);
                    if (!place) break;
                    l = _tree[l]->father()->id();
                    if (l == 0) return;
                }
                l += (kx<0.0) ? -1 : 1;
                while (_cellnumberv[l] == -1)
                {
                    double yM = _tree[l]->child(0)->ymax();
                    double zM = _tree[l]->child(0)->zmax();
                    if (kx<0.0)
                    {
                        if (y<=yM)
                            l = (z<=zM) ? _tree[l]->child(1)->id() : _tree[l]->child(5)->id();
                        else
                            l = (z<=zM) ? _tree[l]->child(3)->id() : _tree[l]->child(7)->id();
                    }
                    else
                    {
                        if (y<=yM)
                            l = (z<=zM) ? _tree[l]->child(0)->id() : _tree[l]->child(4)->id();
                        else
                            l = (z<=zM) ? _tree[l]->child(2)->id() : _tree[l]->child(6)->id();
                    }
                }
            }

            // Repeat the same exercise, but now the y-wall is hit first...

            else if (dsy<dsx && dsy<=dsz)
            {
                path->addSegment(_cellnumberv[l], dsy);
                x += kx*dsy;
                y  = ynext;
                z += kz*dsy;
                while (true)
                {
                    bool place = (ky<0.0) ? ((l-1) % 4 < 2) : ((l-1) % 4 > 1);
                    if (!place) break;
                    l = _tree[l]->father()->id();
                    if (l == 0) return;
                }
                l += (ky<0.0) ? -2 : 2;
                while (_cellnumberv[l] == -1)
                {
                    double xM = _tree[l]->child(0)->xmax();
                    double zM = _tree[l]->child(0)->zmax();
                    if (ky<0.0)
                    {
                        if (x<=xM)
                            l = (z<=zM) ? _tree[l]->child(2)->id() : _tree[l]->child(6)->id();
                        else
                            l = (z<=zM) ? _tree[l]->child(3)->id() : _tree[l]->child(7)->id();
                    }
                    else
                    {
                        if (x<=xM)
                            l = (z<=zM) ? _tree[l]->child(0)->id() : _tree[l]->child(4)->id();
                        else
                            l = (z<=zM) ? _tree[l]->child(1)->id() : _tree[l]->child(5)->id();
                    }
                }
            }

            // Finally, repeat the same exercise, but now the z-wall is hit first...

            else if (dsz< dsx && dsz< dsy)
            {
                path->addSegment(_cellnumberv[l], dsz);
                x += kx*dsz;
                y += ky*dsz;
                z  = znext;
                while (true)
                {
                    int oct = ((l-1) % 8) + 1;
                    bool place = (kz<0.0) ? (oct < 5) : (oct > 4);
                    if (!place) break;
                    l = _tree[l]->father()->id();
                    if (l == 0) return;
                }
                l += (kz<0.0) ? -4 : 4;
                while (_cellnumberv[l] == -1)
                {
                    double xM = _tree[l]->child(0)->xmax();
                    double yM = _tree[l]->child(0)->ymax();
                    if (kz<0.0)
                    {
                        if (x<=xM)
                            l = (y<=yM) ? _tree[l]->child(4)->id() : _tree[l]->child(6)->id();
                        else
                            l = (y<=yM) ? _tree[l]->child(5)->id() : _tree[l]->child(7)->id();
                    }
                    else
                    {
                        if (x<=xM)
                            l = (y<=yM) ? _tree[l]->child(0)->id() : _tree[l]->child(2)->id();
                        else
                            l = (y<=yM) ? _tree[l]->child(1)->id() : _tree[l]->child(3)->id();
                    }
                }
            }
        }
    }

    // ------------------------------
}

//////////////////////////////////////////////////////////////////////

QList<SimulationItem*> TreeDustGridStructure::interfaceCandidates(const type_info &interfaceTypeInfo)
{
    if (interfaceTypeInfo == typeid(DustGridDensityInterface) && !_dmib)
        return QList<SimulationItem*>();
    return GenDustGridStructure::interfaceCandidates(interfaceTypeInfo);
}

//////////////////////////////////////////////////////////////////////

double TreeDustGridStructure::density(int h, int m) const
{
    TreeNode* node = getnode(m);
    return _dmib->massInBox(h, node->extent()) / node->volume();
}

//////////////////////////////////////////////////////////////////////

void TreeDustGridStructure::write_xy(DustGridPlotFile* outfile) const
{
    // Output the root cell and all leaf cells that are close to the section plane
    outfile->writeRectangle(_xmin, _ymin, _xmax, _ymax);
    for (int m=0; m<_Ncells; m++)
    {
        TreeNode* node = getnode(m);
        if (fabs(node->zmin()) < 1e-8*extent().zwidth())
        {
            outfile->writeRectangle(node->xmin(), node->ymin(), node->xmax(), node->ymax());
        }
    }
}

//////////////////////////////////////////////////////////////////////

void TreeDustGridStructure::write_xz(DustGridPlotFile* outfile) const
{
    // Output the root cell and all leaf cells that are close to the section plane
    outfile->writeRectangle(_xmin, _zmin, _xmax, _zmax);
    for (int m=0; m<_Ncells; m++)
    {
        TreeNode* node = getnode(m);
        if (fabs(node->ymin()) < 1e-8*extent().ywidth())
        {
            outfile->writeRectangle(node->xmin(), node->zmin(), node->xmax(), node->zmax());
        }
    }
}

//////////////////////////////////////////////////////////////////////

void TreeDustGridStructure::write_yz(DustGridPlotFile* outfile) const
{
    // Output the root cell and all leaf cells that are close to the section plane
    outfile->writeRectangle(_ymin, _zmin, _ymax, _zmax);
    for (int m=0; m<_Ncells; m++)
    {
        TreeNode* node = getnode(m);
        if (fabs(node->xmin()) < 1e-8*extent().xwidth())
        {
            outfile->writeRectangle(node->ymin(), node->zmin(), node->ymax(), node->zmax());
        }
    }
}

//////////////////////////////////////////////////////////////////////

void TreeDustGridStructure::write_xyz(DustGridPlotFile* outfile) const
{
    // Output all leaf cells up to a certain level
    for (int m=0; m<_Ncells; m++)
    {
        TreeNode* node = getnode(m);
        if (node->level() <= _highestWriteLevel)
            outfile->writeCube(node->xmin(), node->ymin(), node->zmin(), node->xmax(), node->ymax(), node->zmax());
    }
}

//////////////////////////////////////////////////////////////////////

TreeNode* TreeDustGridStructure::root() const
{
    return _tree[0];
}

//////////////////////////////////////////////////////////////////////

TreeNode* TreeDustGridStructure::getnode(int m) const
{
    return _tree[_idv[m]];
}

//////////////////////////////////////////////////////////////////////

int TreeDustGridStructure::cellnumber(const TreeNode* node) const
{
    return _cellnumberv[node->id()];
}

//////////////////////////////////////////////////////////////////////
