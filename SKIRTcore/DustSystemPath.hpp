/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef DUSTSYSTEMPATH_HPP
#define DUSTSYSTEMPATH_HPP

#include <vector>
#include "Direction.hpp"
#include "Position.hpp"

//////////////////////////////////////////////////////////////////////

/** The DustSystemPath class is a very technical class that is used to optimize the radiation
    transfer through a dust system. It is a record of and completely characterizes a path through a
    dusty medium. The path is characterized by the starting position \f${\bf{r}}\f$ and propagation
    direction \f${\bf{k}}\f$. Given this information and the structure of the dust system, one can
    exactly calculate which dust cells are passed through. For each dust cell, the DustSystemPath
    class objects records some information that can be useful for later calculations, such as the
    cell number (each dust cell in a dust component is assigned a cell number), the physical path
    length \f$\Delta s\f$ covered within the cell, the path length \f$s\f$ covered along the entire
    path up to the end of the cell, and the corresponding optical depths \f$\Delta\tau\f$ and
    \f$\tau\f$ at a the wavelength \f$\ell\f$. Internally, a DustSystemPath object is basically a
    collection of vectors with these data. */
class DustSystemPath
{
public:

    /** Trivial constructor for the DustSystemPath class. */
    DustSystemPath();

    /** This function initializes a path by providing an initial position \f${\bf{r}}\f$ and a
        propagation direction \f${\bf{k}}\f$, a wavelength index \f$\ell\f$, and a bunch of
        vectors. */
    void initialize(Position bfr, Direction bfk,
                    const std::vector<int>& mv,
                    const std::vector<double>& dsv,
                    const std::vector<double>& sv,
                    int ell,
                    const std::vector<double>& dtauv,
                    const std::vector<double>& tauv);

    /** This function returns the initial position of the path. */
    Position position() const;

    /** This function returns the propagation direction along the path. */
    Direction direction() const;

    /** This function returns the cell number corresponding to the \f$n\f$'th cell along the path.
        */
    int cellnumber(unsigned int n) const;

    /** This function returns the total path length covered from the start position to the edge of
        the \f$n\f$'th cell along the path (the last cell included). */
    double s(unsigned int n) const;

    /** This function returns the path length covered within the \f$n\f$'th cell along the path. */
    double ds(unsigned int n) const;

    /** This function returns the wavelength index \f$\ell\f$ at which the optical depth is
        calculated. */
    int ell() const;

    /** This function returns the optical depth covered from the start position to the edge of the
        \f$n\f$'th cell along the path (the last cell included). */
    double tau(unsigned int n) const;

    /** This function returns the optical depth covered within the \f$n\f$'th cell along the path.
        */
    double dtau(unsigned int n) const;

    /** This function returns the total optical depth along the entire path. */
    double opticaldepth() const;

    /** This function calculates the pathlength a photon package can travel along the path, until
        it has covered an optical depth \f$\tau\f$. In other words, it converts an optical depth
        \f$\tau\f$ to a physical pathlength \f$s\f$. Since we have recorded the cumulative optical
        depth along this path, this is rather straightforward: we have to determine the first cell
        along the path for which the cumulative optical depth \f$\tau_{m}\f$ becomes larger than
        \f$\tau\f$. This means that the position we are looking for lies within the \f$m\f$'th dust
        cell. The exact pathlength corresponding to \f$\tau\f$ is then found by linear
        interpolation within this cell. */
    double pathlength(double tau) const;

    /** This function returns the number of cells along the path. */
    int size() const;

private:
    Position _bfr;
    Direction _bfk;
    std::vector<int> _mv;
    std::vector<double> _dsv;
    std::vector<double> _sv;
    int _ell;
    std::vector<double> _dtauv;
    std::vector<double> _tauv;
};

//////////////////////////////////////////////////////////////////////

#endif // DUSTSYSTEMPATH_HPP
