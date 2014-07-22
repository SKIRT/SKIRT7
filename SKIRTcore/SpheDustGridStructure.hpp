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
    set of \f$N_r+1\f$ radial grid border points \f$r_i\f$ (with \f$i=0,\ldots,N_r\f$). There are
    \f$N_{\text{cells}} = N_r\f$ cells in the dust grid structure, with cell indices \f$m\f$ that
    map one-to-one to the lower border indices \f$i=0,\ldots,N_r-1\f$. */
class SpheDustGridStructure : public DustGridStructure
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a spherically symmetric dust grid structure")
    Q_CLASSINFO("AllowedIf", "!AxGeometry")
    Q_CLASSINFO("AllowedIf", "!GenGeometry")
    Q_CLASSINFO("AllowedIf", "!NetzerAccretionDiskGeometry")
    Q_CLASSINFO("AllowedIf", "!OffsetGeometry")
    Q_CLASSINFO("AllowedIf", "!AdaptiveMeshStellarComp")
    Q_CLASSINFO("AllowedIf", "!SPHStellarComp")
    Q_CLASSINFO("AllowedIf", "!VoronoiStellarComp")
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
        dust grid structure, cell number \f$m\f$ corresponds to the radial bin with lower border
        index \f$i=m\f$, and the volume is easily calculated as \f[V =
        \frac{4\pi}{3}\, (r_{i+1}^3-r_i^3),\f] with \f$r_i\f$ and \f$r_{i+1}\f$ the inner and outer
        radius of the shell respectively. */
    double volume(int m) const;

    /** This function returns the number of the dust cell that contains the position
        \f${\bf{r}}\f$. It just determines the radial bin index and returns that number. */
    int whichcell(Position bfr) const;

    /** This function returns the central location from the dust cell with cell number \f$m\f$. For
        a spherical dust grid structure, cell number \f$m\f$ corresponds to the radial bin with
        lower border index \f$i=m\f$, and the central radius is determined using \f[ r = \frac{r_i
        + r_{i+1}}{2}. \f] The returned position is arbitrarily located on the x-axis. */
    Position centralPositionInCell(int m) const;

    /** This function returns a random location from the dust cell with cell number \f$m\f$. For a
        spherical dust grid structure, cell number \f$m\f$ corresponds to the radial bin with
        lower border index \f$i=m\f$, and a random radius is determined using \f[ r =
        r_i + {\cal{X}}\,(r_{i+1}-r_i) \f] with \f${\cal{X}}\f$ a random deviate. This random
        radius is combined with a random position on the unit sphere to generate a random position
        from the cell. */
    Position randomPositionInCell(int m) const;

    /** This function calculates a path through the grid. The DustGridPath object passed as an
        argument specifies the starting position \f${\bf{r}}\f$ and the direction \f${\bf{k}}\f$
        for the path. The data on the calculated path are added back into the same object. */
    void path(DustGridPath* path) const;

protected:
    /** This function writes the intersection of the dust grid structure with the xy plane to the
        specified DustGridPlotFile object. */
    void write_xy(DustGridPlotFile* outfile) const;

    //======================== Data Members ========================

protected:
    // data members to be initialized in a subclass
    int _Nr;
    double _rmax;
    Array _rv;
};

////////////////////////////////////////////////////////////////////

#endif // SPHEDUSTGRIDSTRUCTURE_HPP
