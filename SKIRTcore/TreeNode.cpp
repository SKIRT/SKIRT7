/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include <algorithm>
#include "TreeNode.hpp"

using namespace std;

//////////////////////////////////////////////////////////////////////

TreeNode::TreeNode(TreeNode* father, int id, const Box& extent)
    : Box(extent),
      _id(id), _level(father ? father->level() + 1 : 0),
      _father(father)
{
}

//////////////////////////////////////////////////////////////////////

TreeNode::~TreeNode()
{
}

//////////////////////////////////////////////////////////////////////

TreeNode* TreeNode::father() const
{
    return _father;
}

//////////////////////////////////////////////////////////////////////

int TreeNode::id() const
{
    return _id;
}

//////////////////////////////////////////////////////////////////////

int TreeNode::level() const
{
    return _level;
}

//////////////////////////////////////////////////////////////////////

bool TreeNode::ynchildless() const
{
    return _children.empty();
}

//////////////////////////////////////////////////////////////////////

const std::vector<TreeNode*>& TreeNode::children() const
{
    return _children;
}

//////////////////////////////////////////////////////////////////////

TreeNode* TreeNode::child(int l) const
{
    return _children[l];
}

//////////////////////////////////////////////////////////////////////

const TreeNode* TreeNode::whichnode(Vec r) const
{
    if (!contains(r)) return 0;

    const TreeNode* node = this;
    while (!node->ynchildless())
    {
        node = node->child(r);
    }
    return node;
}

//////////////////////////////////////////////////////////////////////

const TreeNode* TreeNode::whichnode(TreeNode::Wall wall, Vec r) const
{
    const vector<TreeNode*>& neighbors = _neighbors[wall];
    int N = neighbors.size();
    for (int i=0; i<N; i++)
    {
        if (neighbors[i]->contains(r)) return neighbors[i];
    }
    return 0;  // specified position is not inside any of the neighbors
}

//////////////////////////////////////////////////////////////////////

void TreeNode::ensureneighborlists()
{
    _neighbors.resize(6);
}

//////////////////////////////////////////////////////////////////////

namespace
{
    // helper class to calculate the area of overlap between two rectangles lined-up with the coordinate axes;
    class Rect
    {
    public:
        Rect(double x1, double y1, double x2, double y2) : _x1(x1), _y1(y1), _x2(x2), _y2(y2) { }
        static double overlap(const Rect& r1, const Rect& r2)
        {
            return max(min(r1._x2, r2._x2) - max(r1._x1, r2._x1), 0.) *
                   max(min(r1._y2, r2._y2) - max(r1._y1, r2._y1), 0.);
        }
    private:
        double _x1, _y1, _x2, _y2;
    };

    // functor to compare the area of overlap between two neigbors at the same wall of the same node
    class LargerOverlap
    {
    public:
        LargerOverlap(const TreeNode* base, TreeNode::Wall wall) : _base(base), _wall(wall) { }
        bool operator() (const TreeNode* node1, TreeNode* node2) { return overlap(node1) > overlap(node2); }
        // returns the overlap area between the specified node and the base node
        double overlap(const TreeNode* node)
        {
            switch (_wall)
            {
            case TreeNode::BACK: case TreeNode::FRONT:
                return Rect::overlap(Rect(_base->ymin(),_base->zmin(),_base->ymax(),_base->zmax()),
                                     Rect( node->ymin(), node->zmin(), node->ymax(), node->zmax()));
            case TreeNode::LEFT: case TreeNode::RIGHT:
                return Rect::overlap(Rect(_base->xmin(),_base->zmin(),_base->xmax(),_base->zmax()),
                                     Rect( node->xmin(), node->zmin(), node->xmax(), node->zmax()));
            case TreeNode::BOTTOM: case TreeNode::TOP:
                return Rect::overlap(Rect(_base->xmin(),_base->ymin(),_base->xmax(),_base->ymax()),
                                     Rect( node->xmin(), node->ymin(), node->xmax(), node->ymax()));
            }
            return 0;
        }
    private:
        const TreeNode* _base;
        TreeNode::Wall _wall;
    };
}

//////////////////////////////////////////////////////////////////////

void TreeNode::sortneighbors()
{
    for (unsigned int wall=0; wall<_neighbors.size(); wall++)
    {
        LargerOverlap larger(this, (Wall)wall);
        sort(_neighbors[wall].begin(), _neighbors[wall].end(), larger);
    }
}

//////////////////////////////////////////////////////////////////////

void TreeNode::addneighbor(TreeNode::Wall wall, TreeNode* node)
{
    _neighbors[wall].push_back(node);
}

//////////////////////////////////////////////////////////////////////

void TreeNode::deleteneighbor(TreeNode::Wall wall, TreeNode* node)
{
    vector<TreeNode*>& neighbors = _neighbors[wall];
    int N = neighbors.size();
    for (int i=0; i<N; i++)
    {
        if (neighbors[i] == node)
        {
            neighbors.erase(neighbors.begin()+i);
            break;
        }
    }
}

//////////////////////////////////////////////////////////////////////

void TreeNode::makeneighbors(TreeNode::Wall wall1, TreeNode *node1, TreeNode *node2)
{
    static const Wall complementingWall[] = { FRONT, BACK, RIGHT, LEFT, TOP, BOTTOM };
    node1->addneighbor(wall1, node2);
    node2->addneighbor(complementingWall[wall1], node1);
}

//////////////////////////////////////////////////////////////////////
