/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef ADAPTIVEMESHDUSTGRIDSTRUCTURE_HPP
#define ADAPTIVEMESHDUSTGRIDSTRUCTURE_HPP

#include "DustGridDensityInterface.hpp"
#include "GenDustGridStructure.hpp"
class AdaptiveMesh;

////////////////////////////////////////////////////////////////////

/** An instance of the AdaptiveMeshDustGridStructure class represents a three-dimensional dust
    grid, the structure of which is described by an imported adaptive mesh. In fact, this class
    directly uses the adaptive mesh created by an AdaptiveMeshGeometry object. For this to
    work, the dust distribution must consist of a single dust component that uses the
    AdaptiveMeshGeometry geometry. */
class AdaptiveMeshDustGridStructure : public GenDustGridStructure, public DustGridDensityInterface
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a dust grid structure based on the adaptive mesh dust geometry")
    Q_CLASSINFO("AllowedIf", "AdaptiveMeshDustDistribution,AdaptiveMeshGeometry")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE AdaptiveMeshDustGridStructure();

    /** This function locates the adaptive mesh in the simulation hierarchy, and remembers a
        pointer to it. It also initializes the number of cells in the data member maintained by the
        base class. */
    void setupSelfBefore();

    //======================== Other Functions =======================

public:
    /** This function returns the maximum extent \f$x_{\text{max}}\f$ of the grid structure in the
        \f$x\f$ direction. */
    double xmax() const;

    /** This function returns the maximum extent \f$y_{\text{max}}\f$ of the grid structure in the
        \f$y\f$ direction. */
    double ymax() const;

    /** This function returns the maximum extent \f$z_{\text{max}}\f$ of the grid structure in the
        \f$z\f$ direction. */
    double zmax() const;

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
    AdaptiveMesh* _mesh;    // adaptive grid obtained from dust geometry
    double _nf;             // normalization factor obtained from dust distribution
};

////////////////////////////////////////////////////////////////////

#endif // ADAPTIVEMESHDUSTGRIDSTRUCTURE_HPP
