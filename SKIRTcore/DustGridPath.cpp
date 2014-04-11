/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include <limits>
#include "Box.hpp"
#include "DustGridPath.hpp"

using namespace std;

//////////////////////////////////////////////////////////////////////

DustGridPath::DustGridPath(const Position& bfr, const Direction& bfk, int sizehint)
    : _bfr(bfr), _bfk(bfk), _s(0)
{
    if (sizehint > 0)
    {
        _mv.reserve(sizehint);
        _sv.reserve(sizehint);
        _dsv.reserve(sizehint);
    }
}

//////////////////////////////////////////////////////////////////////

void DustGridPath::addsegment(int m, double ds)
{
    if (ds>0)
    {
        _s += ds;
        _mv.push_back(m);
        _sv.push_back(_s);
        _dsv.push_back(ds);
    }
}

//////////////////////////////////////////////////////////////////////

Position DustGridPath::moveinside(const Box& box, double eps)
{
    // a position that is certainly not inside any box
    static const Position OUTSIDE(numeric_limits<double>::infinity(),
                                  numeric_limits<double>::infinity(),
                                  numeric_limits<double>::infinity());

    // initial position and direction
    double kx,ky,kz;
    _bfk.cartesian(kx,ky,kz);
    double rx,ry,rz;
    _bfr.cartesian(rx,ry,rz);

    // --> x direction
    if (rx <= box.xmin())
    {
        if (kx <= 0.0) return OUTSIDE;
        else
        {
            double ds = (box.xmin()-rx)/kx;
            addsegment(-1,ds);
            rx = box.xmin() + eps;
            ry += ky*ds;
            rz += kz*ds;
        }
    }
    else if (rx >= box.xmax())
    {
        if (kx >= 0.0) return OUTSIDE;
        else
        {
            double ds = (box.xmax()-rx)/kx;
            addsegment(-1,ds);
            rx = box.xmax() - eps;
            ry += ky*ds;
            rz += kz*ds;
        }
    }

    // --> y direction
    if (ry <= box.ymin())
    {
        if (ky <= 0.0) return OUTSIDE;
        else
        {
            double ds = (box.ymin()-ry)/ky;
            addsegment(-1,ds);
            rx += kx*ds;
            ry = box.ymin() + eps;
            rz += kz*ds;
        }
    }
    else if (ry >= box.ymax())
    {
        if (ky >= 0.0) return OUTSIDE;
        else
        {
            double ds = (box.ymax()-ry)/ky;
            addsegment(-1,ds);
            rx += kx*ds;
            ry = box.ymax() - eps;
            rz += kz*ds;
        }
    }

    // --> z direction
    if (rz <= box.zmin())
    {
        if (kz <= 0.0) return OUTSIDE;
        else
        {
            double ds = (box.zmin()-rz)/kz;
            addsegment(-1,ds);
            rx += kx*ds;
            ry += ky*ds;
            rz = box.zmin() + eps;
        }
    }
    else if (rz >= box.zmax())
    {
        if (kz >= 0.0) return OUTSIDE;
        else
        {
            double ds = (box.zmax()-rz)/kz;
            addsegment(-1,ds);
            rx += kx*ds;
            ry += ky*ds;
            rz = box.zmax() - eps;
        }
    }

    // the position should now be just inside the box; although in rare cases, it may be still be outside!
    return Position(rx,ry,rz);
}

//////////////////////////////////////////////////////////////////////

DustGridPath& DustGridPath::clear()
{
    _s = 0;
    _mv.clear();
    _sv.clear();
    _dsv.clear();
    return *this;
}

//////////////////////////////////////////////////////////////////////

int
DustGridPath::size()
const
{
    return _mv.size();
}

//////////////////////////////////////////////////////////////////////

const Position&
DustGridPath::position()
const
{
    return _bfr;
}

//////////////////////////////////////////////////////////////////////

const Direction&
DustGridPath::direction()
const
{
    return _bfk;
}

//////////////////////////////////////////////////////////////////////

const vector<int>&
DustGridPath::mv()
const
{
    return _mv;
}

//////////////////////////////////////////////////////////////////////

const vector<double>&
DustGridPath::sv()
const
{
    return _sv;
}

//////////////////////////////////////////////////////////////////////

const vector<double>&
DustGridPath::dsv()
const
{
    return _dsv;
}

//////////////////////////////////////////////////////////////////////
