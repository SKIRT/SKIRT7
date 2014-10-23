/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include <cfloat>
#include <cmath>
#include "DustGridPath.hpp"
#include "DustParticleInterface.hpp"
#include "VoronoiMesh.hpp"
#include "VoronoiMeshFile.hpp"
#include "FatalError.hpp"
#include "Random.hpp"
#include "container.hh"

using namespace std;

////////////////////////////////////////////////////////////////////

namespace VoronoiMesh_Private
{
    // class to hold the information about a Voronoi cell that is relevant for calculating paths and densities
    class VoronoiCell : public Box  // enclosing box
    {
    private:
        Vec _r;                     // particle position
        Vec _c;                     // centroid position
        double _volume;             // volume
        vector<int> _neighbors;     // list of neighbor indices in cells vector

    public:
        // constructor stores the specified particle position; the other data members are set to zero
        VoronoiCell(Vec r) : _r(r), _volume(0) { }

        // initializes the receiver with information taken from the specified fully computed Voronoi cell
        void init(voro::voronoicell_neighbor& cell)
        {
            // copy basic geometric info
            double cx, cy, cz;
            cell.centroid(cx,cy,cz);
            _c = Vec(cx,cy,cz) + _r;
            _volume = cell.volume();

            // get the minimal and maximal coordinates of the box enclosing the cell
            vector<double> coords;
            cell.vertices(_r.x(),_r.y(),_r.z(), coords);
            _xmin = DBL_MAX;  _ymin = DBL_MAX;  _zmin = DBL_MAX;
            _xmax = -DBL_MAX; _ymax = -DBL_MAX; _zmax = -DBL_MAX;
            int n = coords.size();
            for (int i=0; i<n; i+=3)
            {
                _xmin = min(_xmin,coords[i]); _ymin = min(_ymin,coords[i+1]); _zmin = min(_zmin,coords[i+2]);
                _xmax = max(_xmax,coords[i]); _ymax = max(_ymax,coords[i+1]); _zmax = max(_zmax,coords[i+2]);
            }

            // copy a list of neighboring cell/particle ids
            cell.neighbors(_neighbors);
        }

        // returns the cell's particle position
        Vec particle() const { return _r; }

        // returns the squared distance from the cell's particle to the specified point
        double squaredDistanceTo(Vec r) const { return (r-_r).norm2(); }

        // returns the central position in the cell
        Vec centroid() const { return _c; }

        // returns the volume of the cell; overriding volume() function of Box bas class
        double volume() const { return _volume; }

        // returns a list of neighboring cell/particle ids
        const vector<int>& neighbors() { return _neighbors; }
    };

    // function to compare two points according to the specified axis (0,1,2)
    bool lessthan(Vec p1, Vec p2, int axis)
    {
        switch (axis)
        {
        case 0:  // split on x
            if (p1.x()<p2.x()) return true;
            if (p1.x()>p2.x()) return false;
            if (p1.y()<p2.y()) return true;
            if (p1.y()>p2.y()) return false;
            if (p1.z()<p2.z()) return true;
            return false;
        case 1:  // split on y
            if (p1.y()<p2.y()) return true;
            if (p1.y()>p2.y()) return false;
            if (p1.z()<p2.z()) return true;
            if (p1.z()>p2.z()) return false;
            if (p1.x()<p2.x()) return true;
            return false;
        case 2:  // split on z
            if (p1.z()<p2.z()) return true;
            if (p1.z()>p2.z()) return false;
            if (p1.x()<p2.x()) return true;
            if (p1.x()>p2.x()) return false;
            if (p1.y()<p2.y()) return true;
            return false;
        default: // this should never happen
            return false;
        }
    }

    // helper class to compare the location of two cell particles in one dimension
    class LessThan
    {
    private:
        const vector<VoronoiCell*>& _cells;
        int _axis;
    public:
        LessThan(const vector<VoronoiCell*>& cells, int depth) : _cells(cells), _axis(depth%3) { }
        bool operator() (int m1, int m2)
        {
            if (m1==m2) return false;
            return lessthan(_cells[m1]->particle(), _cells[m2]->particle(), _axis);
        }
    };

    // class to hold a node in the binary search tree (see en.wikipedia.org/wiki/Kd-tree)
    class Node
    {
    private:
        int _m;         // index in _cells to the particle defining the split at this node
        int _axis;      // split axis for this node (0,1,2)
        Node* _up;      // ptr to the parent node
        Node* _left;    // ptr to the left child node
        Node* _right;   // ptr to the right child node

        // returns the square of its argument
        static double sqr(double x) { return x*x; }

    public:
        // constructor stores the specified particle index and child pointers (which may be null)
        Node(int m, int depth, Node* left, Node* right) : _m(m), _axis(depth%3), _up(0), _left(left), _right(right)
        {
            if (_left) _left->setParent(this);
            if (_right) _right->setParent(this);
        }

        // destructor destroys the children
        ~Node() { delete _left; delete _right; }

        // sets parent pointer (called from parent's constructor)
        void setParent(Node* up) { _up = up; }

        // returns the corresponding data member
        int m() const { return _m; }
        Node* up() const { return _up; }
        Node* left() const { return _left; }
        Node* right() const { return _right; }

        // returns the apropriate child for the specified query point
        Node* child(Vec bfr, const vector<VoronoiCell*>& cells) const
            { return lessthan(bfr, cells[_m]->particle(), _axis) ? _left : _right; }

        // returns the other child than the one that would be apropriate for the specified query point
        Node* otherChild(Vec bfr, const vector<VoronoiCell*>& cells) const
            { return lessthan(bfr, cells[_m]->particle(), _axis) ? _right : _left; }

        // returns the squared distance from the query point to the split plane
        double squaredDistanceToSplitPlane(Vec bfr, const vector<VoronoiCell*>& cells) const
        {
            switch (_axis)
            {
            case 0:  // split on x
                return sqr(cells[_m]->particle().x() - bfr.x());
            case 1:  // split on y
                return sqr(cells[_m]->particle().y() - bfr.y());
            case 2:  // split on z
                return sqr(cells[_m]->particle().z() - bfr.z());
            default: // this should never happen
                return 0;
            }
        }

        // returns the node in this subtree that represents the particle nearest to the query point
        Node* nearest(Vec bfr, const vector<VoronoiCell*>& cells)
        {
            // recursively descend the tree until a leaf node is reached, going left or right depending on
            // whether the specified point is less than or greater than the current node in the split dimension
            Node* current = this;
            while (Node* child = current->child(bfr, cells)) current = child;

            // unwind the recursion, looking for the nearest node while climbing up
            Node* best = current;
            double bestSD = cells[best->m()]->squaredDistanceTo(bfr);
            while (true)
            {
                // if the current node is closer than the current best, then it becomes the current best
                double currentSD = cells[current->m()]->squaredDistanceTo(bfr);
                if (currentSD < bestSD)
                {
                    best = current;
                    bestSD = currentSD;
                }

                // if there could be points on the other side of the splitting plane for the current node
                // that are closer to the search point than the current best, then ...
                double splitSD = current->squaredDistanceToSplitPlane(bfr, cells);
                if (splitSD < bestSD)
                {
                    // move down the other branch of the tree from the current node looking for closer points,
                    // following the same recursive process as the entire search
                    Node* other = current->otherChild(bfr, cells);
                    if (other)
                    {
                        Node* otherBest = other->nearest(bfr, cells);
                        double otherBestSD = cells[otherBest->m()]->squaredDistanceTo(bfr);
                        if (otherBestSD < bestSD)
                        {
                            best = otherBest;
                            bestSD = otherBestSD;
                        }
                    }
                }

                // move up to the parent until we meet the top node
                if (current==this) break;
                current = current->up();
            }
            return best;
        }
    };
}

using namespace VoronoiMesh_Private;

////////////////////////////////////////////////////////////////////

VoronoiMesh::VoronoiMesh(VoronoiMeshFile* meshfile, QList<int> fieldIndices, const Box& extent)
    : _extent(extent), _eps(1e-12 * extent.widths().norm()),
      _Ndistribs(0), _integratedDensity(0)
{
    // create a list of indices (g) without duplicates, ignoring negative values
    // create a hash table to map field indices (g) to storage indices (s)
    QList<int> uniqueIndices;
    foreach (int g, fieldIndices)
    {
        if (g >= 0 && !_storageIndices.contains(g))
        {
            _storageIndices.insert(g, uniqueIndices.size());
            uniqueIndices << g;
        }
    }

    // reserve room for the required number of fields
    _fieldvalues.resize(uniqueIndices.size());

    // read the particle records from the file, filling a temporary list of particle locations
    // and copying any required field values
    vector<Vec> particles;
    meshfile->open();
    while (meshfile->read())
    {
        // get and verify the particle location
        Vec r = meshfile->particle();
        if (extent.contains(r))
        {
            // store the particle
            particles.push_back(r);

            // get and store the column values
            int s = 0;
            foreach (int g, uniqueIndices)
            {
                _fieldvalues[s++].push_back(meshfile->value(g));
            }
        }
    }
    meshfile->close();

    // construct the Voronoi tesselation
    buildMesh(particles);
}

////////////////////////////////////////////////////////////////////

VoronoiMesh::VoronoiMesh(const std::vector<Vec> &particles, const Box &extent)
    : _extent(extent), _eps(1e-12 * extent.widths().norm()),
      _Ndistribs(0), _integratedDensity(0)
{
    // construct the Voronoi tesselation
    buildMesh(particles);
}

////////////////////////////////////////////////////////////////////

VoronoiMesh::VoronoiMesh(DustParticleInterface *dpi, const Box &extent)
    : _extent(extent), _eps(1e-12 * extent.widths().norm()),
      _Ndistribs(0), _integratedDensity(0)
{
    // copy the particle locations into a temporary vector
    vector<Vec> particles;
    int numParticles = dpi->numParticles();
    for (int m=0; m<numParticles; m++)
    {
        Vec r = dpi->particleCenter(m);
        if (extent.contains(r)) particles.push_back(r);
    }

    // construct the Voronoi tesselation
    buildMesh(particles);
}

////////////////////////////////////////////////////////////////////

void VoronoiMesh::buildMesh(const std::vector<Vec>& particles)
{
    // Cache some often used values
    _Ncells = particles.size();
    _nb = max(3, min(1000, static_cast<int>(3.*pow(_Ncells,1./3.)) ));
    _nb2 = _nb*_nb;
    _nb3 = _nb*_nb*_nb;

    // Initialize the vector that will hold pointers to the cell objects that will stay around,
    // using the serial number of the cell as index in the vector
    _cells.resize(_Ncells);

    // Add the specified particles to our cell vector AND to a temporary Voronoi container,
    // using the serial number of the cell as particle ID
    voro::container con(_extent.xmin(), _extent.xmax(), _extent.ymin(), _extent.ymax(), _extent.zmin(), _extent.zmax(),
                        _nb, _nb, _nb, false,false,false, 8);
    for (int m=0; m<_Ncells; m++)
    {
        Vec r = particles[m];
        _cells[m] = new VoronoiCell(r);  // these objects will be deleted by the destructor
        con.put(m, r.x(),r.y(),r.z());
    }

    // Initialize a vector of nb x nb x nb lists, each containing the cells overlapping a certain block in the domain
    _blocklists.resize(_nb3);

    // For each particle:
    //   - compute the corresponding cell in the Voronoi tesselation
    //   - extract and copy the relevant information to one of our own cell objects
    //   - store the cell object in the vector indexed on cell number
    //   - add the cell object to the lists for all blocks it overlaps
    voro::c_loop_all loop(con);
    if (loop.start()) do
    {
        // Compute the cell
        voro::voronoicell_neighbor fullcell;
        bool ok = con.compute_cell(fullcell, loop);
        if (!ok) throw FATALERROR("Can't compute Voronoi cell " + QString::number(loop.pid()));

        // Copy all relevant information to the cell object that will stay around
        VoronoiCell* cell = _cells[loop.pid()];
        cell->init(fullcell);

        // Add the cell object to the lists for all blocks it may overlap
        // --> a precise intersection test is really slow and doesn't substantially accelerate whichcell()
        int i1,j1,k1, i2,j2,k2;
        _extent.cellindices(i1,j1,k1, cell->rmin()-Vec(_eps,_eps,_eps), _nb,_nb,_nb);
        _extent.cellindices(i2,j2,k2, cell->rmax()+Vec(_eps,_eps,_eps), _nb,_nb,_nb);
        for (int i=i1; i<=i2; i++)
            for (int j=j1; j<=j2; j++)
                for (int k=k1; k<=k2; k++)
                    _blocklists[i*_nb2+j*_nb+k].push_back(loop.pid());
    }
    while (loop.inc());

    // for each block that contains more than a predefined number of cells,
    // construct a search tree on the particle locations of the cells
    _blocktrees.resize(_nb3);
    for (int b = 0; b<_nb3; b++)
    {
        vector<int>& ids = _blocklists[b];
        if (ids.size() > 5)
        {
            _blocktrees[b] = buildTree(ids.begin(), ids.end(), 0);
        }
    }
}

////////////////////////////////////////////////////////////////////

Node* VoronoiMesh::buildTree(vector<int>::iterator first, vector<int>::iterator last, int depth)
{
    size_t length = last-first;
    if (length>0)
    {
        LessThan compare(_cells, depth);
        size_t median = length >> 1;
        nth_element(first, first+median, last, compare);
        return new Node(*(first+median), depth,
                        buildTree(first, first+median, depth+1),
                        buildTree(first+median+1, last, depth+1));
    }
    return 0;
}

////////////////////////////////////////////////////////////////////

void VoronoiMesh::addDensityDistribution(int densityField, int densityMultiplierField, double densityFraction)
{
    // verify indices
    if (!_storageIndices.contains(densityField)) throw FATALERROR("Density field index out of range");
    if (densityMultiplierField >= 0 && (!_storageIndices.contains(densityMultiplierField)
                                        || densityMultiplierField==densityField))
        throw FATALERROR("Density multiplier field index out of range");

    // map field indices to storage indices
    densityField = _storageIndices.value(densityField);
    densityMultiplierField = _storageIndices.value(densityMultiplierField, -1);

    // store the information for this density distribution
    _densityFields << densityField;
    _densityMultiplierFields << densityMultiplierField;
    _densityFractions << densityFraction;
    _Ndistribs++;

    // update the integrated density (ignore cells with negative density)
    for (int m=0; m<_Ncells; m++)
    {
        double density = _fieldvalues[densityField][m] * densityFraction;
        if (densityMultiplierField >= 0) density *= _fieldvalues[densityMultiplierField][m];
        if (density > 0) _integratedDensity += density*_cells[m]->volume();
    }
}

////////////////////////////////////////////////////////////////////

VoronoiMesh::~VoronoiMesh()
{
    for (int m=0; m<_Ncells; m++) delete _cells[m];
    for (int b=0; b<_nb3; b++) delete _blocktrees[b];
}

////////////////////////////////////////////////////////////////////

int VoronoiMesh::Ncells() const
{
    return _Ncells;
}

////////////////////////////////////////////////////////////////////

void VoronoiMesh::neighborStatistics(double &average, int &minimum, int &maximum) const
{
    int minNeighbors = INT_MAX;
    int maxNeighbors = 0;
    qint64 totalNeighbors = 0;
    for (int m=0; m<_Ncells; m++)
    {
        int ns = _cells[m]->neighbors().size();
        totalNeighbors += ns;
        minNeighbors = min(minNeighbors, ns);
        maxNeighbors = max(maxNeighbors, ns);
    }
    average = double(totalNeighbors)/_Ncells;
    minimum = minNeighbors;
    maximum = maxNeighbors;
}

////////////////////////////////////////////////////////////////////

int VoronoiMesh::Nblocks() const
{
    return _nb;
}

////////////////////////////////////////////////////////////////////

void VoronoiMesh::blockStatistics(double &average, int &minimum, int &maximum) const
{
    int minRefsPerBlock = INT_MAX;
    int maxRefsPerBlock = 0;
    qint64 totalRefs = 0;
    for (int b = 0; b<_nb3; b++)
    {
        int refs = _blocklists[b].size();
        totalRefs += refs;
        minRefsPerBlock = min(minRefsPerBlock, refs);
        maxRefsPerBlock = max(maxRefsPerBlock, refs);
    }
    average = double(totalRefs)/_nb3;
    minimum = minRefsPerBlock;
    maximum = maxRefsPerBlock;
}

void VoronoiMesh::treeStatistics(int& Ntrees, double& average, int& minimum, int& maximum) const
{
    int minRefsPerTree = INT_MAX;
    int maxRefsPerTree = 0;
    qint64 totalRefs = 0;
    int trees = 0;
    for (int b = 0; b<_nb3; b++)
    {
        if (_blocktrees[b])
        {
            trees++;
            int refs = _blocklists[b].size();
            totalRefs += refs;
            minRefsPerTree = min(minRefsPerTree, refs);
            maxRefsPerTree = max(maxRefsPerTree, refs);
        }
    }
    Ntrees = trees;
    average = double(totalRefs)/trees;
    minimum = minRefsPerTree;
    maximum = maxRefsPerTree;
}

////////////////////////////////////////////////////////////////////

int VoronoiMesh::cellIndex(Position bfr) const
{
    // make sure the position is inside the domain
    if (!_extent.contains(bfr)) return -1;

    // determine the block in which the point falls
    int i,j,k;
    _extent.cellindices(i,j,k, bfr, _nb,_nb,_nb);
    int b = i*_nb2+j*_nb+k;

    // look for the closest particle in this block, using the search tree if there is one
    Node* tree = _blocktrees[b];
    if (tree) return tree->nearest(bfr,_cells)->m();

    // if there is no search tree, simply loop over the index list
    const vector<int>& ids = _blocklists[b];
    int m = -1;
    double mdist = DBL_MAX;
    int n = ids.size();
    for (int i=0; i<n; i++)
    {
        double idist = _cells[ids[i]]->squaredDistanceTo(bfr);
        if (idist < mdist)
        {
            m = ids[i];
            mdist = idist;
        }
    }
    return m;
}

////////////////////////////////////////////////////////////////////

double VoronoiMesh::volume() const
{
    return _extent.volume();
}

////////////////////////////////////////////////////////////////////

double VoronoiMesh::volume(int m) const
{
    if (m < 0 || m >= _Ncells) throw FATALERROR("Cell index out of range: " + QString::number(m));
    return _cells[m]->volume();
}

////////////////////////////////////////////////////////////////////

Box VoronoiMesh::extent() const
{
    return _extent;
}

////////////////////////////////////////////////////////////////////

Box VoronoiMesh::extent(int m) const
{
    if (m < 0 || m >= _Ncells) throw FATALERROR("Cell index out of range: " + QString::number(m));
    return _cells[m]->extent();
}

////////////////////////////////////////////////////////////////////

Position VoronoiMesh::particlePosition(int m) const
{
    if (m < 0 || m >= _Ncells) throw FATALERROR("Cell index out of range: " + QString::number(m));
    return Position(_cells[m]->particle());
}

////////////////////////////////////////////////////////////////////

Position VoronoiMesh::centralPosition(int m) const
{
    if (m < 0 || m >= _Ncells) throw FATALERROR("Cell index out of range: " + QString::number(m));
    return Position(_cells[m]->centroid());
}

////////////////////////////////////////////////////////////////////

Position VoronoiMesh::randomPosition(Random* random, int m) const
{
    if (m < 0 || m >= _Ncells) throw FATALERROR("Cell index out of range: " + QString::number(m));

    // get loop-invariant information about the cell
    const Box& box = _cells[m]->extent();
    const vector<int>& neighbors = _cells[m]->neighbors();

    // generate random points in the enclosing box until one happens to be inside the cell
    for (int i=0; i<10000; i++)
    {
        Vec r = random->position(box);
        if (isPointClosestTo(r, m, neighbors)) return Position(r);
    }
    throw FATALERROR("Can't find random position in cell");
}

//////////////////////////////////////////////////////////////////////

bool VoronoiMesh::isPointClosestTo(Vec r, int m, const vector<int>& ids) const
{
    double target = _cells[m]->squaredDistanceTo(r);
    foreach (int id, ids)
    {
        if (id>=0 && _cells[id]->squaredDistanceTo(r) < target) return false;
    }
    return true;
}

////////////////////////////////////////////////////////////////////

double VoronoiMesh::value(int g, int m) const
{
    int s = _storageIndices.value(g, -1);
    if (s < 0) throw FATALERROR("Field index out of range: " + QString::number(g));
    if (m < 0 || m >= _Ncells) throw FATALERROR("Cell index out of range: " + QString::number(m));
    return _fieldvalues[s][m];
}

////////////////////////////////////////////////////////////////////

double VoronoiMesh::value(int g, Position bfr) const
{
    int s = _storageIndices.value(g, -1);
    if (s < 0) throw FATALERROR("Field index out of range: " + QString::number(g));
    int m = cellIndex(bfr);
    return m>=0 ? _fieldvalues[s][m] : 0;
}

////////////////////////////////////////////////////////////////////

double VoronoiMesh::density(int h, int m) const
{
    if (!_integratedDensity) throw FATALERROR("There is no density field");
    if (h < 0 || h >= _Ndistribs) throw FATALERROR("Density distribution index out of range: " + QString::number(h));
    if (m < 0 || m >= _Ncells) throw FATALERROR("Cell index out of range: " + QString::number(m));

    double density = _fieldvalues[_densityFields[h]][m] * _densityFractions[h];
    if (_densityMultiplierFields[h] >= 0) density *= _fieldvalues[_densityMultiplierFields[h]][m];
    return density > 0 ? density : 0;
}

////////////////////////////////////////////////////////////////////

double VoronoiMesh::density(int h, Position bfr) const
{
    int m = cellIndex(bfr);
    return m>=0 ? density(h, m) : 0;
}

////////////////////////////////////////////////////////////////////

double VoronoiMesh::density(int m) const
{
    if (!_integratedDensity) throw FATALERROR("There is no density field");
    if (m < 0 || m >= _Ncells) throw FATALERROR("Cell index out of range: " + QString::number(m));

    double result = 0;
    for (int h=0; h<_Ndistribs; h++)
    {
        double density = _fieldvalues[_densityFields[h]][m] * _densityFractions[h];
        if (_densityMultiplierFields[h] >= 0) density *= _fieldvalues[_densityMultiplierFields[h]][m];
        if (density > 0) result += density;
    }
    return result;
}

////////////////////////////////////////////////////////////////////

double VoronoiMesh::density(Position bfr) const
{
    int m = cellIndex(bfr);
    return m>=0 ? density(m) : 0;
}

////////////////////////////////////////////////////////////////////

double VoronoiMesh::integratedDensity() const
{
    return _integratedDensity;
}

////////////////////////////////////////////////////////////////////

double VoronoiMesh::SigmaX() const
{
    const int NSAMPLES = 10000;
    double sum = 0;
    double xmin = _extent.xmin();
    double xmax = _extent.xmax();
    for (int k = 0; k < NSAMPLES; k++)
    {
        sum += density(Position(xmin + k*(xmax-xmin)/NSAMPLES, _eps, _eps));
    }
    return (sum/NSAMPLES)*(xmax-xmin);
}

////////////////////////////////////////////////////////////////////

double VoronoiMesh::SigmaY() const
{
    const int NSAMPLES = 10000;
    double sum = 0;
    double ymin = _extent.ymin();
    double ymax = _extent.ymax();
    for (int k = 0; k < NSAMPLES; k++)
    {
        sum += density(Position(_eps, ymin + k*(ymax-ymin)/NSAMPLES, _eps));
    }
    return (sum/NSAMPLES)*(ymax-ymin);
}

////////////////////////////////////////////////////////////////////

double VoronoiMesh::SigmaZ() const
{
    const int NSAMPLES = 10000;
    double sum = 0;
    double zmin = _extent.zmin();
    double zmax = _extent.zmax();
    for (int k = 0; k < NSAMPLES; k++)
    {
        sum += density(Position(_eps, _eps, zmin + k*(zmax-zmin)/NSAMPLES));
    }
    return (sum/NSAMPLES)*(zmax-zmin);
}

////////////////////////////////////////////////////////////////////

void VoronoiMesh::path(DustGridPath* path) const
{
    // Initialize the path
    path->clear();
    Direction bfk = path->direction();

    // If the photon package starts outside the dust grid, move it into the first grid cell that it will pass
    Position r = path->moveInside(_extent, _eps);

    // Get the index of the cell containing the current position;
    // if the position is not inside the grid, return an empty path
    int mr = cellIndex(r);
    if (mr<0) return path->clear();

    // Start the loop over cells/path segments until we leave the grid
    while (mr>=0)
    {
        // get the particle position for this cell
        Vec pr = _cells[mr]->particle();

        // initialize the smallest nonnegative intersection distance and corresponding index
        double sq = DBL_MAX;          // very large, but not infinity (so that infinite si values are discarded)
        const int NO_INDEX = -99;     // meaningless cell index
        int mq = NO_INDEX;

        // loop over the list of neighbor indices
        const vector<int>& mv = _cells[mr]->neighbors();
        int n = mv.size();
        for (int i=0; i<n; i++)
        {
            int mi = mv[i];

            // declare the intersection distance for this neighbor (init to a value that will be rejected)
            double si = 0;

            // --- intersection with neighboring cell
            if (mi>=0)
            {
                // get the particle position for this neighbor
                Vec pi = _cells[mi]->particle();

                // calculate the (unnormalized) normal on the bisecting plane
                Vec n = pi - pr;

                // calculate the denominator of the intersection quotient
                double ndotk = Vec::dot(n,bfk);

                // if the denominator is negative the intersection distance is negative, so don't calculate it
                if (ndotk > 0)
                {
                    // calculate a point on the bisecting plane
                    Vec p = 0.5 * (pi + pr);

                    // calculate the intersection distance
                    si = Vec::dot(n,p-r) / ndotk;
                }
            }

            // --- intersection with domain wall
            else
            {
                switch (mi)
                {
                case -1: si = (extent().xmin()-r.x())/bfk.x(); break;
                case -2: si = (extent().xmax()-r.x())/bfk.x(); break;
                case -3: si = (extent().ymin()-r.y())/bfk.y(); break;
                case -4: si = (extent().ymax()-r.y())/bfk.y(); break;
                case -5: si = (extent().zmin()-r.z())/bfk.z(); break;
                case -6: si = (extent().zmax()-r.z())/bfk.z(); break;
                default: throw FATALERROR("Invalid neighbor ID");
                }
            }

            // remember the smallest nonnegative intersection point
            if (si > 0 && si < sq)
            {
                sq = si;
                mq = mi;
            }
        }

        // if no exit point was found, advance the current point by small distance and recalculate cell index
        if (mq == NO_INDEX)
        {
            r += bfk*_eps;
            mr = cellIndex(r);
        }
        // otherwise add a path segment and set the current point to the exit point
        else
        {
            path->addSegment(mr, sq);
            r += (sq+_eps)*bfk;
            mr = mq;
        }
    }
}

////////////////////////////////////////////////////////////////////
