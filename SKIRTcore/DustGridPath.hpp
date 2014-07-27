/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef DUSTGRIDPATH_HPP
#define DUSTGRIDPATH_HPP

#include <vector>
#include <functional>
#include "Direction.hpp"
#include "Position.hpp"
class Box;

//////////////////////////////////////////////////////////////////////

/** A DustGridPath object contains the details of a path through a dust grid structure. Given a
    dust grid structure, i.e. an object of a DustGridStucture subclass, a starting position
    \f${\bf{r}}\f$ and a propagation direction \f${\bf{k}}\f$, one can calculate the path through
    the dust grid structure. A DustGridPath object keeps record of all the cells that are crossed
    by this path, together with the physical path length \f$\Delta s\f$ covered within each cell,
    and the path length \f$s\f$ covered along the entire path up to the end of the cell. Given
    additional information about the dust properties in each cell (at a particular wavelength), one
    can also calculate optical depth information for the path. A DustGridPath object keeps record
    of the optical depth \f$\Delta\tau\f$ along the path segment within each cell, and the optical
    depth \f$\tau\f$ along the entire path up to the end of the cell. */
class DustGridPath
{
public:

    // ------- Constructors and setters -------

    /** This constructor creates an empty path with the specified initial position and propagation
        direction. */
    DustGridPath(const Position& bfr, const Direction& bfk);

    /** This constructor creates an empty path with the initial position and propagation direction
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

    /** This function calucates and stores the optical depth details for the path \f[
        (\Delta\tau)_i = (\Delta s)_i \times (\kappa\rho)_{m_i}, \f] \f[ \tau_i = \sum_{j=0}^i
        (\Delta\tau)_j,\f] using the path segment lengths \f$\Delta s_i\f$ already stored within
        the path object, and the multiplication factors \f$(\kappa\rho)_{m_i}\f$ provided by the
        caller through a call-back function, where \f$m_i\f$ is the number of the dust cell being
        crossed in path segment \f$i\f$. */
    void fillOpticalDepth(std::function<double(int m)> kapparho);

    // ------- Trivial getters -------

    /** This function returns the number of cells crossed along the path. */
    int size() const { return _mv.size(); }

    /** This function returns the initial position of the path. */
    const Position& position() const { return _bfr; }

    /** This function returns the propagation direction along the path. */
    const Direction& direction() const { return _bfk; }

    /** This function returns the cell number \f$m\f$ for segment $i$ in the path. */
    int m(int i) const { return _mv[i]; }

    /** This function returns the path length covered from the initial position of the path until
        the end point of the cell in segment $i$ in the path. */
    double s(int i) const { return _sv[i]; }

    /** This function returns the path length covered within the cell in segment $i$ in the path.
        */
    double ds(int i) const { return _dsv[i]; }

    /** This function returns the optical depth covered from the initial position of the path until
        the end point of the cell in segment $i$ in the path. */
    double tau(int i) const { return _tauv[i]; }

    /** This function returns the optical depth covered within the cell in segment $i$ in the path.
        */
    double dtau(int i) const { return _dtauv[i]; }

    // ------- Nontrivial getters -------

    /** This function returns the total optical depth along the entire path. It assumes that the
        fillOpticalDepth() function was previously invoked for the path. */
    double tau() const;

    /** This function calculates the path length a photon package can travel along the path until
        it has covered an optical depth \f$\tau\f$. In other words, it converts an optical depth
        \f$\tau\f$ to a physical path length \f$s\f$. The function assumes that the
        fillOpticalDepth() function was previously invoked for the path. We have to determine the
        first cell along the path for which the cumulative optical depth \f$\tau_{m}\f$ becomes
        larger than \f$\tau\f$. This means that the position we are looking for lies within the
        \f$m\f$'th dust cell. The exact path length corresponding to \f$\tau\f$ is then found by
        linear interpolation within this cell. */
    double pathlength(double tau) const;

    // ------- Data members -------

protected:
    Position _bfr;
    Direction _bfk;
private:
    double _s;
    std::vector<int> _mv;
    std::vector<double> _sv;
    std::vector<double> _dsv;
    std::vector<double> _tauv;
    std::vector<double> _dtauv;
};

//////////////////////////////////////////////////////////////////////

#endif // DUSTGRIDPATH_HPP
