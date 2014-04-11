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

/** The DustGridPath class is a technical class that contains the details of a path through a dust
    grid structure. Given a dust grid structure, i.e. an object of a subclass of the
    DustGridStucture class, a starting position \f${\bf{r}}\f$ and a propagation direction
    \f${\bf{k}}\f$, one can calculate the path through the dust grid structure. A DustGridPath
    object keeps record of all the cells that are crossed by this path, together with the physical
    path length \f$\Delta s\f$ covered within each cell and the path length \f$s\f$ covered along
    the entire path up to the end of the cell. The difference between this class and the
    DustSystemPath class is that the DustGridPath class represents only the geometrical properties
    of a path (i.e. it is only linked to a DustGridStructure class object), whereas the
    DustSystemPath class also records the details on the optical depth (it is linked to a
    DustSystem class object). */
class DustGridPath
{
public:
    /** This constructor creates an empty path with the specified initial position and propagation
        direction. If the optional \em sizehint is larger than zero, memory space is
        reserved for a path with that size, avoiding the need for reallocating the internal vectors
        as the path grows. The path is still allowed to grow beyond the specified size hint. In any
        case the constructed path is empty (representing a path that lies outside the dust grid
        structure). */
    DustGridPath(const Position& bfr, const Direction& bfk, int sizehint = 0);

    /** This function adds a segment in cell \f$m\f$ with length \f$s\f$ to the path. */
    void addsegment(int m, double ds);

    /** This function adds the segments to the path that are needed to move the initial position
        along the propagation direction (both specified in the constructor) inside a given box, and
        returns the final position. If the initial position is already inside the box, no segments
        are added. If the half-ray formed by the initial position and the propagation direction
        does not intersect the box, the function returns some arbitrary position outside the box.
        The small value specified by \em eps is added to the path length beyond the intersection
        point so that the final position is well inside the box, guarding against rounding errors.
        */
    Position moveinside(const Box &box, double eps);

    /** This function removes all path segments, resulting in an empty path with the original
        initial position and propagation direction. The function returns a reference to the
        DustGridPath object itself (for convenient use in a return statement). */
    DustGridPath& clear();

    /** This function returns the number of cells crossed along the path. */
    int size() const;

    /** This function returns the initial position of the path. */
    const Position& position() const;

    /** This function returns the propagation direction along the path. */
    const Direction& direction() const;

    /** This function returns the vector with the cell numbers \f$m\f$ of all the cells encountered
        along the path. */
    const std::vector<int>& mv() const;

    /** This function returns the vector with the path lengths covered from the initial position of
        the path until the end point of each cell encountered along the path. */
    const std::vector<double>& sv() const;

    /** This function returns the vector with the path lengths covered within each of the cells
        encountered along the path. */
    const std::vector<double>& dsv() const;

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
