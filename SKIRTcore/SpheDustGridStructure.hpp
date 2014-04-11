/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef SPHEDUSTGRIDSTRUCTURE_HPP
#define SPHEDUSTGRIDSTRUCTURE_HPP

#include "Array.hpp"
#include "DustGridStructure.hpp"

////////////////////////////////////////////////////////////////////

/** The SpheDustGridStructure class is an abstract subclass of the general DustGridStructure class,
    and represents one-dimensional, spherically symmetric dust grid structures. Each cell in such a
    grid is a spherical shell. Internally, a spherical dust grid structure is specified through a
    set of \f$N_r+1\f$ radial grid points \f$r_i\f$ (with \f$i=0,\ldots,N_r\f$). In total there are
    \f$N_{\text{cells}} = N_r\f$ cells in the dust grid structure. */
class SpheDustGridStructure : public DustGridStructure
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a spherically symmetric dust grid structure")
    Q_CLASSINFO("AllowedIf", "!AxGeometry")
    Q_CLASSINFO("AllowedIf", "!GenGeometry")
    Q_CLASSINFO("AllowedIf", "!OffsetGeometry")
    Q_CLASSINFO("AllowedIf", "!AdaptiveMeshStellarSystem")
    Q_CLASSINFO("AllowedIf", "!SPHStellarSystem")
    Q_CLASSINFO("AllowedIf", "!VoronoiStellarSystem")
    Q_CLASSINFO("AllowedIf", "!AdaptiveMeshDustDistribution")
    Q_CLASSINFO("AllowedIf", "!SPHDustDistribution")
    Q_CLASSINFO("AllowedIf", "!VoronoiDustDistribution")

    //============= Construction - Setup - Destruction =============

protected:
    /** The default constructor; it is protected since this is an abstract class. */
    SpheDustGridStructure();

    //======================== Other Functions =======================

public:
    /** This function returns the dimension of the grid, which is 1 for all subclasses of this
        class since it is a base class for spherically symmetric grids. */
    int dimension() const;

    /** This function returns the maximum extent \f$x_{\text{max}}\f$ of the grid structure in the
        \f$x\f$ direction. For spherical dust grid structures, it returns the maximum radial extent
        \f$r_{\text{max}}\f$. */
    double xmax() const;

    /** This function returns the maximum extent \f$y_{\text{max}}\f$ of the grid structure in the
        \f$y\f$ direction. For spherical dust grid structures, it returns the maximum radial extent
        \f$r_{\text{max}}\f$. */
    double ymax() const;

    /** This function returns the maximum extent \f$z_{\text{max}}\f$ of the grid structure in the
        \f$x\f$ direction. For spherical dust grid structures, it returns the maximum radial extent
        \f$z_{\text{max}}\f$. */
    double zmax() const;

    /** This function returns the volume of the dust cell with cell number \f$m\f$. For a spherical
        dust grid structure, the function determines the radial bin \f$i\f$ that corresponds to the
        cell number \f$m\f$ and the volume is easily calculated as \f[V =
        \frac{4\pi}{3}\,(r_{i+1}^3-r_i^3),\f] with \f$r_i\f$ and \f$r_{i+1}\f$ the inner and outer
        radius of the shell respectively. */
    double volume(int m) const;

    /** This function returns the number of the dust cell that contains the position
        \f${\bf{r}}\f$. It just determines the radial bin index and returns that number. */
    int whichcell(Position bfr) const;

    /** This function returns the central location from the dust cell with cell number \f$m\f$. For
        a spherical dust grid structure, the function first determines the radial bin \f$i\f$
        corresponding to the cell number \f$m\f$. Then, the central radius is determined using \f[
        r = \frac{r_i + r_{i+1}}{2} \f] This random radius is combined with the unit vector on the
        X-axis to generate the central position from the cell. */
    Position centralPositionInCell(int m) const;

    /** This function returns a random location from the dust cell with cell number \f$m\f$. For a
        spherical dust grid structure, the function first determines the radial bin \f$i\f$
        corresponding to the cell number \f$m\f$. Then, a random radius is determined using \f[ r =
        r_i + {\cal{X}}\,(r_{i+1}-r_i) \f] with \f${\cal{X}}\f$ a random deviate. This random
        radius is combined with a random position on the unit sphere to generate a random position
        from the cell. */
    Position randomPositionInCell(int m) const;

    /** This function returns a DustGridPath object, corresponding to path through the grid
        starting at the position \f${\bf{r}}\f$ into the direction \f${\bf{k}}\f$. This
        DustGridPath consists of three vectors: the first one lists the cell numbers \f$m\f$ of all
        the cells crossed by the path, the second lists the path length \f$\Delta s\f$ covered in
        each of these dust cells, and the third lists the total covered path length \f$s\f$ until
        the end of each cell is encountered. */
    DustGridPath path(Position bfr, Direction bfk) const;

protected:
    /** This function writes the intersection of the dust grid structure with the xy plane to the
        specified DustGridPlotFile object. */
    void write_xy(DustGridPlotFile* outfile) const;

private:
    /** This private function determines the radial bin index \f$i\f$ such that \f$r_i \leq r <
        r_{i+1}\f$. It uses a simple bisection method borrowed from the Numerical Recipes book. */
    int whichrcell(double r) const;

    /** This private function returns the cell number corresponding to the radial bin index
        \f$i\f$. It trivially returns \f$m=i\f$. */
    int index(int i) const;

    /** This private function calculates the radial bin index \f$i\f$ from the cell number \f$m\f$.
        For a spherical dust grid structure, we have \f$i=m\f$. */
    void invertindex(int m, int& i) const;

    //======================== Data Members ========================

protected:
    // data members to be initialized in a subclass
    int _Nr;
    double _rmax;
    Array _rv;
};

////////////////////////////////////////////////////////////////////

#endif // SPHEDUSTGRIDSTRUCTURE_HPP
