/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef ADAPTIVEMESHDUSTGRID_HPP
#define ADAPTIVEMESHDUSTGRID_HPP

#include "DustGrid.hpp"
#include "DustGridDensityInterface.hpp"
#include "Random.hpp"
class AdaptiveMesh;

////////////////////////////////////////////////////////////////////

/** An instance of the AdaptiveMeshDustGrid class represents a three-dimensional dust
    grid, the structure of which is described by an imported adaptive mesh. In fact, this class
    directly uses the adaptive mesh created by an AdaptiveMeshGeometry object. For this to
    work, the dust distribution must consist of a single dust component that uses the
    AdaptiveMeshGeometry geometry. */
class AdaptiveMeshDustGrid : public DustGrid, public DustGridDensityInterface
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a dust grid based on the adaptive mesh dust geometry")
    Q_CLASSINFO("AllowedIf", "AdaptiveMeshDustDistribution,AdaptiveMeshGeometry")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE AdaptiveMeshDustGrid();

    /** This function locates the adaptive mesh in the simulation hierarchy, and remembers a
        pointer to it. */
    void setupSelfBefore();

    //======================== Other Functions =======================

public:
    /** This function returns the dimension of the dust grid, which is 3 for an adaptive mesh dust grid. */
    int dimension() const;

    /** This function returns the number of cells in the dust grid. */
    int numCells() const;

    /** This function returns the bounding box that encloses the dust grid. */
    Box boundingbox() const;

    /** This function returns the volume of the dust cell with cell number \f$m\f$. */
    double volume(int m) const;

    /** This function returns the number of the dust cell that contains the position
        \f${\bf{r}}\f$. */
    int whichcell(Position bfr) const;

    /** This function returns the central location from the dust cell with cell number \f$m\f$. */
    Position centralPositionInCell(int m) const;

    /** This function returns a random location from the dust cell with cell number \f$m\f$. */
    Position randomPositionInCell(int m) const;

    /** This function implements the DustGridDensityInterface interface. It returns the density for
        the dust component \em h in the dust grid cell with index \em m. */
    double density(int h, int m) const;

    /** This function calculates a path through the grid. The DustGridPath object passed as an
        argument specifies the starting position \f${\bf{r}}\f$ and the direction \f${\bf{k}}\f$
        for the path. The data on the calculated path are added back into the same object. */
    void path(DustGridPath* path) const;

protected:
    /** This function writes the intersection of the dust grid structure with the xy plane to the
        specified DustGridPlotFile object. */
    void write_xy(DustGridPlotFile* outfile) const;

    /** This function writes the intersection of the dust grid structure with the xz plane to the
        specified DustGridPlotFile object. */
    void write_xz(DustGridPlotFile* outfile) const;

    /** This function writes the intersection of the dust grid structure with the yz plane to the
        specified DustGridPlotFile object. */
    void write_yz(DustGridPlotFile* outfile) const;

    /** This function writes 3D information for all cells in the dust grid structure to the
        specified DustGridPlotFile object. */
    void write_xyz(DustGridPlotFile* outfile) const;

    //======================== Data Members ========================

private:
    Random* _random;
    AdaptiveMesh* _amesh;    // adaptive grid obtained from dust geometry
    double _nf;             // normalization factor obtained from dust distribution
};

////////////////////////////////////////////////////////////////////

#endif // ADAPTIVEMESHDUSTGRID_HPP
