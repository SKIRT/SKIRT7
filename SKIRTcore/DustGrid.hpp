/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef DUSTGRID_HPP
#define DUSTGRID_HPP

#include "Box.hpp"
#include "Position.hpp"
#include "SimulationItem.hpp"
class DustGridPath;
class DustGridPlotFile;

//////////////////////////////////////////////////////////////////////

/** The DustGrid class is an abstract base class for dust grids. It represents a collection of dust
    cells, which together divide a given configuration space. Each position in the computational
    domain corresponds to a single dust cell. The dust grid only contains the purely geometric
    properties, i.e.\ it contains no information on the actual distribution of dust over the grid
    (that information is contained in the DustSystem class). */
class DustGrid : public SimulationItem
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a dust grid")

    Q_CLASSINFO("Property", "writeGrid")
    Q_CLASSINFO("Title", "output data files for plotting the structure of the grid")
    Q_CLASSINFO("Default", "yes")

    //============= Construction - Setup - Destruction =============

protected:
    /** The default constructor; it is protected since this is an abstract class. */
    DustGrid();

    /** This function verifies that the grid has sufficient dimension to represent the simulation's
        geometry, and caches some values for use by subclasses. */
    void setupSelfBefore();

    /** If the corresponding write flag is turned on, this function writes out data files that
        describe the intersection of the dust grid with the coordinate planes in a text format that
        can be easily plotted. The number of data files written depends on the dimension of the
        simulation's geometry: for spherical symmetry only the intersection with the xy plane is
        written, for axial symmetry the intersections with the xy and xz planes are written, and
        for general geometries all three intersections are written. The data files are called
        <tt>prefix_ds_gridxy.dat</tt>, <tt>prefix_ds_gridxz.dat</tt> and
        <tt>prefix_ds_gridyz.dat</tt>. Within a file, each line contains two coordinates seperated
        by whitespace or is empty. Consecutive nonempty lines represent a sequence of "lineto"
        commands; an empty line marks a "moveto" command. */
    void setupSelfAfter();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the flag that indicates whether or not to output text files that allow plotting the
        structure of the grid for further analysis. The default value is true. */
    Q_INVOKABLE void setWriteGrid(bool value);

    /** Returns the flag that indicates whether or not to output text files that allow plotting the
        structure of the grid for further analysis. */
    Q_INVOKABLE bool writeGrid() const;

    //======================== Other Functions =======================

public:
    /** This pure virtual function returns the dimension of the dust grid, which depends on its
        (lack of) symmetry. A value of 1 means spherical symmetry, 2 means axial symmetry and 3
        means none of these symmetries. */
    virtual int dimension() const = 0;

    /** This pure virtual function returns the number of cells in the dust grid. */
    virtual int numCells() const = 0;

    /** This pure virtual function returns the bounding box that encloses the dust grid. */
    virtual Box boundingbox() const = 0;

    /** This virtual function returns the weight corresponding to the cell with cell number
        \f$m\f$. It is defaulted to return the value 1 for all cells, which is appropriate for
        virtually all dust grids. However, this function can be (and is) overwritten by special
        subclasses. */
    virtual double weight(int m) const;

    /** This pure virtual function returns the volume of the dust cell with cell number \f$m\f$. */
    virtual double volume(int m) const = 0;

    /** This pure virtual function returns the number of the dust cell that contains the position
        \f${\bf{r}}\f$. */
    virtual int whichcell(Position bfr) const = 0;

    /** This pure virtual function returns the central location of the dust cell with cell number
        \f$m\f$. */
    virtual Position centralPositionInCell(int m) const = 0;

    /** This pure virtual function returns a random location from the dust cell with cell number
        \f$m\f$. */
    virtual Position randomPositionInCell(int m) const = 0;

    /** This pure virtual function calculates a path through the grid. The DustGridPath object
        passed as an argument specifies the starting position \f${\bf{r}}\f$ and the direction
        \f${\bf{k}}\f$ for the path. The data on the calculated path are added back into the same
        object. This consists of three vectors: the first one lists the cell numbers \f$m\f$ of all
        the cells crossed by the path, the second lists the path length \f$\Delta s\f$ covered in
        each of these dust cells, and the third lists the total covered path length \f$s\f$ until
        the end of each cell is encountered. */
    virtual void path(DustGridPath* path) const = 0;

protected:
    /** This virtual function writes the intersection of the dust grid with the xy plane to the
        specified DustGridPlotFile object. The default implementation does nothing. */
    virtual void write_xy(DustGridPlotFile* outfile) const;

    /** This virtual function writes the intersection of the dust grid with the xz plane to the
        specified DustGridPlotFile object. The default implementation does nothing. */
    virtual void write_xz(DustGridPlotFile* outfile) const;

    /** This virtual function writes the intersection of the dust grid with the yz plane to the
        specified DustGridPlotFile object. The default implementation does nothing. */
    virtual void write_yz(DustGridPlotFile* outfile) const;

    /** This virtual function writes 3D information for all or part of the cells in the dust grid
        structure to the specified DustGridPlotFile object. The default implementation does
        nothing. */
    virtual void write_xyz(DustGridPlotFile* outfile) const;

    //======================== Data Members ========================

private:
    // discoverable attributes
    bool _writeGrid;
};

//////////////////////////////////////////////////////////////////////

#endif // DUSTGRID_HPP
