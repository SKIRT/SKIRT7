/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include "AdaptiveMeshFile.hpp"
#include "AdaptiveMeshNode.hpp"
#include "FatalError.hpp"

using namespace std;

////////////////////////////////////////////////////////////////////

AdaptiveMeshNode::AdaptiveMeshNode(const Box& extent, QList<int> fieldIndices, AdaptiveMeshFile* meshfile,
                                   vector<AdaptiveMeshNode*>& leafnodes, vector< vector<double> >& fieldvalues)
    : Box(extent)
{
    // read a line and detect premature end-of file
    if (!meshfile->read()) throw FATALERROR("Reached end of file in mesh data before all nodes were read");

    // this is a nonleaf line
    if (meshfile->isNonLeaf())
    {
        _m = -1;

        // get the number of child nodes
        meshfile->numChildNodes(_Nx, _Ny, _Nz);

        // construct and store our children, in local Morton order
        _nodes.resize(_Nx*_Ny*_Nz);
        int m = 0;
        for (int k=0; k<_Nz; k++)
            for (int j=0; j<_Ny; j++)
                for (int i=0; i<_Nx; i++)
                {
                    Vec r0 = extent.fracpos(i,j,k, _Nx, _Ny, _Nz);
                    Vec r1 = extent.fracpos(i+1,j+1,k+1, _Nx, _Ny, _Nz);
                    _nodes[m++] = new AdaptiveMeshNode(Box(r0, r1), fieldIndices, meshfile, leafnodes, fieldvalues);
                }
    }
    // this is a leaf line
    else
    {
        _Nx = 0; _Ny = 0; _Nz = 0;

        // add this leaf node to the list
        _m = leafnodes.size();
        leafnodes.push_back(this);

        // get and store the column values
        int s = 0;
        foreach (int g, fieldIndices)
        {
            fieldvalues[s++].push_back(meshfile->value(g));
        }
    }
}

////////////////////////////////////////////////////////////////////

void AdaptiveMeshNode::addNeighbors(AdaptiveMeshNode* root, double eps)
{
    // skip if this node has children or if neighbors already have been added
    if (_nodes.empty())
    {
        // node center
        double xc = (_xmin+_xmax)/2.;
        double yc = (_ymin+_ymax)/2.;
        double zc = (_zmin+_zmax)/2.;

        // determine the node just beyond the center of each wall (or null pointer for domain walls)
        _nodes.resize(6);
        _nodes[BACK]   = root->whichnode(Vec(_xmin-eps, yc, zc));
        _nodes[FRONT]  = root->whichnode(Vec(_xmax+eps, yc, zc));
        _nodes[LEFT]   = root->whichnode(Vec(xc, _ymin-eps, zc));
        _nodes[RIGHT]  = root->whichnode(Vec(xc, _ymax+eps, zc));
        _nodes[BOTTOM] = root->whichnode(Vec(xc, yc, _zmin-eps));
        _nodes[TOP]    = root->whichnode(Vec(xc, yc, _zmax+eps));
    }
}

////////////////////////////////////////////////////////////////////

AdaptiveMeshNode::~AdaptiveMeshNode()
{
    if (!isLeaf())
    {
        int n = _nodes.size();
        for (int i=0; i<n; i++) delete _nodes[i];
    }
}

////////////////////////////////////////////////////////////////////

int AdaptiveMeshNode::cellIndex()  const
{
    return _m;
}

////////////////////////////////////////////////////////////////////

bool AdaptiveMeshNode::isLeaf() const
{
    return _m >= 0;
}

////////////////////////////////////////////////////////////////////

const AdaptiveMeshNode* AdaptiveMeshNode::child(Vec r) const
{
    // estimate the child node indices; this may be off by one due to rounding errors
    int i,j,k;
    cellindices(i,j,k, r, _Nx,_Ny,_Nz);

    // get the estimated node using local Morton order
    const AdaptiveMeshNode* node = _nodes[ (k*_Ny+j)*_Nx+i ];

    // if the point is NOT in the node, correct the indices and get the new node
    if (!node->contains(r))
    {
        if (r.x() < node->xmin()) i--; else if (r.x() > node->xmax()) i++;
        if (r.y() < node->ymin()) j--; else if (r.y() > node->ymax()) j++;
        if (r.z() < node->zmin()) k--; else if (r.z() > node->zmax()) k++;
        node = _nodes[ (k*_Ny+j)*_Nx+i ];
        if (!node->contains(r)) throw FATALERROR("Can't locate the appropriate child node");
    }
    return node;
}

////////////////////////////////////////////////////////////////////

const AdaptiveMeshNode* AdaptiveMeshNode::whichnode(Vec r) const
{
    if (!contains(r)) return 0;

    const AdaptiveMeshNode* node = this;
    while (!node->isLeaf())
    {
        node = node->child(r);
    }
    return node;
}

////////////////////////////////////////////////////////////////////

const AdaptiveMeshNode* AdaptiveMeshNode::whichnode(AdaptiveMeshNode::Wall wall, Vec r) const
{
    const AdaptiveMeshNode* node = _nodes.size()==6 ? _nodes[wall] : 0;
    if (node && node->contains(r)) return node;
    else return 0;
}

////////////////////////////////////////////////////////////////////
