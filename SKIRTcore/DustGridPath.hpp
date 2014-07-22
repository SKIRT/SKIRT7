/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef DUSTGRIDPATH_HPP
#define DUSTGRIDPATH_HPP

#include <vector>
#include "Direction.hpp"
#include "Position.hpp"
class Box;

//////////////////////////////////////////////////////////////////////

/** A DustGridPath object contains the details of a path through a dust grid structure. Given a
    dust grid structure, i.e. an object of a DustGridStucture subclass, a starting position
    \f${\bf{r}}\f$ and a propagation direction \f${\bf{k}}\f$, one can calculate the path through
    the dust grid structure. A DustGridPath object keeps record of all the cells that are crossed
    by this path, together with the physical path length \f$\Delta s\f$ covered within each cell
    and the path length \f$s\f$ covered along the entire path up to the end of the cell. */
class DustGridPath
{
public:

    // ------- Constructors and setters -------

    /** This constructor creates an empty path with the specified initial position and propagation
        direction. */
    DustGridPath(const Position& bfr, const Direction& bfk);

    /** This constructor creates an empty path the with initial position and propagation direction
        initialized to null values. After using this constructor, invoke the setPosition() and
        setDirection() functions to set these properties to appropriate values. */
    DustGridPath();

    /** This function sets the initial position of the path to a new value. */
    void setPosition(const Position& bfr) { _bfr = bfr; }

    /** This function sets the propagation direction along the path to a new value. */
    void setDirection(const Direction& bfk) { _bfk = bfk; }

    /** This function removes all path segments, resulting in an empty path with the original
        initial position and propagation direction. */
    void clear();

    /** This function adds a segment in cell \f$m\f$ with length \f$\Delta s\f$ to the path,
        assuming \f$\Delta s>0\f$. Otherwise the function does nothing. */
    void addSegment(int m, double ds);

    /** This function adds the segments to the path that are needed to move the initial position
        along the propagation direction (both specified in the constructor) inside a given box, and
        returns the final position. If the initial position is already inside the box, no segments
        are added. If the half-ray formed by the initial position and the propagation direction
        does not intersect the box, the function returns some arbitrary position outside the box.
        The small value specified by \em eps is added to the path length beyond the intersection
        point so that the final position is well inside the box, guarding against rounding errors.
        */
    Position moveInside(const Box &box, double eps);

    // ------- Getters -------

    /** This function returns the number of cells crossed along the path. */
    int size() const { return _mv.size(); }

    /** This function returns the initial position of the path. */
    const Position& position() const { return _bfr; }

    /** This function returns the propagation direction along the path. */
    const Direction& direction() const { return _bfk; }

    /** This function returns the vector with the cell numbers \f$m\f$ of all the cells encountered
        along the path. */
    const std::vector<int>& mv() const { return _mv; }

    /** This function returns the vector with the path lengths covered from the initial position of
        the path until the end point of each cell encountered along the path. */
    const std::vector<double>& sv() const { return _sv; }

    /** This function returns the vector with the path lengths covered within each of the cells
        encountered along the path. */
    const std::vector<double>& dsv() const { return _dsv; }

    /** This function returns the cell number \f$m\f$ for segment $i$ in the path. */
    int mv(int i) const { return _mv[i]; }

    /** This function returns the path length covered from the initial position of the path until
        the end point of the cell in segment $i$ in the path. */
    double sv(int i) const { return _sv[i]; }

    /** This function returns the path length covered within the cell in segment $i$ in the path.
        */
    double dsv(int i) const { return _dsv[i]; }

private:
    Position _bfr;
    Direction _bfk;
    double _s;
    std::vector<int> _mv;
    std::vector<double> _sv;
    std::vector<double> _dsv;
};

//////////////////////////////////////////////////////////////////////

#endif // DUSTGRIDPATH_HPP
