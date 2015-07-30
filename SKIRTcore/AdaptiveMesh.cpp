/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include <cfloat>
#include <cmath>
#include <QFile>
#include "AdaptiveMesh.hpp"
#include "AdaptiveMeshFile.hpp"
#include "AdaptiveMeshNode.hpp"
#include "DustGridPath.hpp"
#include "FatalError.hpp"
#include "Log.hpp"
#include "Random.hpp"

using namespace std;

////////////////////////////////////////////////////////////////////

AdaptiveMesh::AdaptiveMesh(AdaptiveMeshFile* meshfile, QList<int> fieldIndices, const Box& extent, Log* log)
    : _log(log)
{
    // open the data file
    meshfile->open();

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
    _fieldvalues.resize(fieldIndices.size());

    // construct the root node, and recursively all other nodes
    // this also fills the _fieldvalues and _leafnodes vectors
    _root = new AdaptiveMeshNode(extent, uniqueIndices, meshfile, _leafnodes, _fieldvalues);
    _Ncells = _leafnodes.size();

    // verify that all data was read and close the file
    if (meshfile->read()) throw FATALERROR("Superfluous data in mesh data after all nodes were read");
    meshfile->close();

    // determine small value relative to the domain extent
    _eps = 1e-12 * extent.widths().norm();

    // clear the density distribution info
    _Ndistribs = 0;
    _integratedDensity = 0;
}

////////////////////////////////////////////////////////////////////

void AdaptiveMesh::addDensityDistribution(int densityField, int densityMultiplierField, double densityFraction)
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
    double integratedDensity = 0.0;
    for (int m=0; m<_Ncells; m++)
    {
        double density = _fieldvalues[densityField][m] * densityFraction;
        if (densityMultiplierField >= 0) density *= _fieldvalues[densityMultiplierField][m];
        if (density > 0) integratedDensity += density*_leafnodes[m]->volume();
    }
    _integratedDensityv.push_back(integratedDensity);
    _integratedDensity += integratedDensity;
}

////////////////////////////////////////////////////////////////////

void AdaptiveMesh::addNeighbors()
{
    for (int m=0; m<_Ncells; m++)
    {
        _leafnodes[m]->addNeighbors(_root, _eps);
    }
}

////////////////////////////////////////////////////////////////////

AdaptiveMesh::~AdaptiveMesh()
{
    delete _root;
}

////////////////////////////////////////////////////////////////////

int AdaptiveMesh::Ncells() const
{
    return _Ncells;
}

////////////////////////////////////////////////////////////////////

int AdaptiveMesh::cellIndex(Position bfr) const
{
    const AdaptiveMeshNode* node = _root->whichnode(bfr);
    return node ? node->cellIndex() : -1;
}

////////////////////////////////////////////////////////////////////

double AdaptiveMesh::volume() const
{
    return _root->volume();
}

////////////////////////////////////////////////////////////////////

double AdaptiveMesh::volume(int m) const
{
    if (m < 0 || m >= _Ncells) throw FATALERROR("Cell index out of range: " + QString::number(m));
    return _leafnodes[m]->volume();
}

////////////////////////////////////////////////////////////////////

Box AdaptiveMesh::extent() const
{
    return _root->extent();
}

////////////////////////////////////////////////////////////////////

Box AdaptiveMesh::extent(int m) const
{
    if (m < 0 || m >= _Ncells) throw FATALERROR("Cell index out of range: " + QString::number(m));
    return _leafnodes[m]->extent();
}

////////////////////////////////////////////////////////////////////

Position AdaptiveMesh::centralPosition(int m) const
{
    if (m < 0 || m >= _Ncells) throw FATALERROR("Cell index out of range: " + QString::number(m));
    return Position(_leafnodes[m]->center());
}

////////////////////////////////////////////////////////////////////

Position AdaptiveMesh::randomPosition(Random* random, int m) const
{
    if (m < 0 || m >= _Ncells) throw FATALERROR("Cell index out of range: " + QString::number(m));
    return random->position(_leafnodes[m]->extent());
}

////////////////////////////////////////////////////////////////////

double AdaptiveMesh::value(int g, int m) const
{
    int s = _storageIndices.value(g, -1);
    if (s < 0) throw FATALERROR("Field index out of range: " + QString::number(g));
    if (m < 0 || m >= _Ncells) throw FATALERROR("Cell index out of range: " + QString::number(m));
    return _fieldvalues[s][m];
}

////////////////////////////////////////////////////////////////////

double AdaptiveMesh::value(int g, Position bfr) const
{
    int s = _storageIndices.value(g, -1);
    if (s < 0) throw FATALERROR("Field index out of range: " + QString::number(g));
    int m = cellIndex(bfr);
    return m>=0 ? _fieldvalues[s][m] : 0;
}

////////////////////////////////////////////////////////////////////

double AdaptiveMesh::density(int h, int m) const
{
    if (!_integratedDensity) throw FATALERROR("There is no density field");
    if (h < 0 || h >= _Ndistribs) throw FATALERROR("Density distribution index out of range: " + QString::number(h));
    if (m < 0 || m >= _Ncells) throw FATALERROR("Cell index out of range: " + QString::number(m));

    double density = _fieldvalues[_densityFields[h]][m] * _densityFractions[h];
    if (_densityMultiplierFields[h] >= 0) density *= _fieldvalues[_densityMultiplierFields[h]][m];
    return density > 0 ? density : 0;
}

////////////////////////////////////////////////////////////////////

double AdaptiveMesh::density(int h, Position bfr) const
{
    int m = cellIndex(bfr);
    return m>=0 ? density(h, m) : 0;
}

////////////////////////////////////////////////////////////////////

double AdaptiveMesh::density(int m) const
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

double AdaptiveMesh::density(Position bfr) const
{
    int m = cellIndex(bfr);
    return m>=0 ? density(m) : 0;
}

////////////////////////////////////////////////////////////////////

double AdaptiveMesh::integratedDensity(int h) const
{
    if (h < 0 || h >= _Ndistribs) throw FATALERROR("Density distribution index out of range: " + QString::number(h));
    return _integratedDensityv[h];
}

////////////////////////////////////////////////////////////////////

double AdaptiveMesh::integratedDensity() const
{
    return _integratedDensity;
}

////////////////////////////////////////////////////////////////////

double AdaptiveMesh::SigmaX() const
{
    const int NSAMPLES = 10000;
    double sum = 0;
    double xmin = _root->extent().xmin();
    double xmax = _root->extent().xmax();
    for (int k = 0; k < NSAMPLES; k++)
    {
        sum += density(Position(xmin + k*(xmax-xmin)/NSAMPLES, _eps, _eps));
    }
    return (sum/NSAMPLES)*(xmax-xmin);
}

////////////////////////////////////////////////////////////////////

double AdaptiveMesh::SigmaY() const
{
    const int NSAMPLES = 10000;
    double sum = 0;
    double ymin = _root->extent().ymin();
    double ymax = _root->extent().ymax();
    for (int k = 0; k < NSAMPLES; k++)
    {
        sum += density(Position(_eps, ymin + k*(ymax-ymin)/NSAMPLES, _eps));
    }
    return (sum/NSAMPLES)*(ymax-ymin);
}

////////////////////////////////////////////////////////////////////

double AdaptiveMesh::SigmaZ() const
{
    const int NSAMPLES = 10000;
    double sum = 0;
    double zmin = _root->extent().zmin();
    double zmax = _root->extent().zmax();
    for (int k = 0; k < NSAMPLES; k++)
    {
        sum += density(Position(_eps, _eps, zmin + k*(zmax-zmin)/NSAMPLES));
    }
    return (sum/NSAMPLES)*(zmax-zmin);
}

////////////////////////////////////////////////////////////////////

void AdaptiveMesh::path(DustGridPath* path) const
{
    // Initialize the path
    path->clear();

    // If the photon package starts outside the dust grid, move it into the first grid cell that it will pass
    Position r = path->moveInside(_root->extent(), _eps);

    // Get the node containing the current location;
    // if the position is not inside the grid, return an empty path
    const AdaptiveMeshNode* node = _root->whichnode(r);
    if (!node) return path->clear();

    // Start the loop over nodes/path segments until we leave the grid.
    double kx,ky,kz;
    path->direction().cartesian(kx,ky,kz);
    while (node)
    {
        double xnext = (kx<0.0) ? node->xmin() : node->xmax();
        double ynext = (ky<0.0) ? node->ymin() : node->ymax();
        double znext = (kz<0.0) ? node->zmin() : node->zmax();
        double dsx = (fabs(kx)>1e-15) ? (xnext-r.x())/kx : DBL_MAX;
        double dsy = (fabs(ky)>1e-15) ? (ynext-r.y())/ky : DBL_MAX;
        double dsz = (fabs(kz)>1e-15) ? (znext-r.z())/kz : DBL_MAX;

        double ds;
        AdaptiveMeshNode::Wall wall;
        if (dsx<=dsy && dsx<=dsz)
        {
            ds = dsx;
            wall = (kx<0.0) ? AdaptiveMeshNode::BACK : AdaptiveMeshNode::FRONT;
        }
        else if (dsy<=dsx && dsy<=dsz)
        {
            ds = dsy;
            wall = (ky<0.0) ? AdaptiveMeshNode::LEFT : AdaptiveMeshNode::RIGHT;
        }
        else
        {
            ds = dsz;
            wall = (kz<0.0) ? AdaptiveMeshNode::BOTTOM : AdaptiveMeshNode::TOP;
        }
        path->addSegment(node->cellIndex(), ds);
        r += (ds+_eps)*(path->direction());

        // try the most likely neighbor of the current node, and use top-down search as a fall-back
        const AdaptiveMeshNode* oldnode = node;
        node = node->whichnode(wall,r);
        if (!node) node = _root->whichnode(r);

        // if we're stuck in the same node...
        if (node==oldnode)
        {
            // try to escape by advancing the position to the next representable coordinates
            if (_log) _log->warning("Photon package seems stuck in dust cell "
                                    + QString::number(node->cellIndex()) + " -- escaping");
            r.set( nextafter(r.x(), (kx<0.0) ? -DBL_MAX : DBL_MAX),
                   nextafter(r.y(), (ky<0.0) ? -DBL_MAX : DBL_MAX),
                   nextafter(r.z(), (kz<0.0) ? -DBL_MAX : DBL_MAX) );
            node = _root->whichnode(r);

            // if that didn't work, terminate the path
            if (node==oldnode)
            {
                if (_log) _log->warning("Photon package is stuck in dust cell "
                                        + QString::number(node->cellIndex()) + " -- terminating this path");
                break;
            }
        }
    }
}

////////////////////////////////////////////////////////////////////
