/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef CUBDUSTGRIDSTRUCTURE_HPP
#define CUBDUSTGRIDSTRUCTURE_HPP

#include "Array.hpp"
#include "GenDustGridStructure.hpp"
class Box;

////////////////////////////////////////////////////////////////////

/** The CubDustGridStructure class is an abstract subclass of the GenDustGridStructure class,
    and represents three-dimensional dust grid structures based on a regular cartesian grid. Each
    cell in such a grid structure is a little cuboid (not necessarily all with the same size or
    axis ratios). Internally, a cartesian dust grid structure is specified through a set of grid
    points in the X, Y and Z directions. The number of grid points in the three directions are
    \f$N_x+1\f$, \f$N_y+1\f$ and \f$N_z+1\f$, which leads to a total number of \f$N_{\text{cells}}
    = N_x\,N_y\,N_z\f$ cells. */
class CubDustGridStructure : public GenDustGridStructure
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a regular cartesian 3D dust grid structure")

    //============= Construction - Setup - Destruction =============

protected:
    /** The default constructor; it is protected since this is an abstract class. */
    CubDustGridStructure();

    //======================== Other Functions =======================

public:
    /** This function returns the maximum extent \f$x_{\text{max}}\f$ of the grid structure in the
        \f$x\f$ direction. This information is stored internally. */
    virtual double xmax() const;
    //double xmax() const;

    /** This function returns the maximum extent \f$y_{\text{max}}\f$ of the grid structure in the
        \f$y\f$ direction. This information is stored internally. */
    virtual double ymax() const;
    //double ymax() const;

    /** This function returns the maximum extent \f$z_{\text{max}}\f$ of the grid structure in the
        \f$z\f$ direction. This information is stored internally. */
    virtual double zmax() const;
    //double zmax() const;

    /** This function returns the volume of the dust cell with cell number \f$m\f$. For a cartesian
        dust grid structure, the function determines the bin indices \f$i\f$, \f$j\f$ and \f$k\f$
        corresponding to the X, Y and Z directions. The volume is then easily calculated as \f$V =
        (x_{i+1}-x_i)\, (y_{j+1}-y_j)\, (z_{k+1}-z_k) \f$. */
    double volume(int m) const;

    /** This function returns the number of the dust cell that contains the position
        \f${\bf{r}}\f$. For a cartesian dust grid structure, the function determines the bin
        indices in the X, Y and Z directions and calculates the correct cell number based on these
        indices. */
    int whichcell(Position bfr) const;

    /** This function returns the central location from the dust cell with cell number \f$m\f$. For
        a cartesian dust grid structure, the function first determines the bin indices \f$i\f$ and
        \f$k\f$ in the X, Y and Z directions that correspond to the cell number \f$m\f$. Then the
        coordinates \f$x\f$, \f$y\f$ and \f$z\f$ are determined using \f[ \begin{split} x &=
        \frac{x_i + x_{i+1}}{2} \\ y &= \frac{y_j + y_{j+1}}{2} \\ z &= \frac{z_k + z_{k+1}}{2}
        \end{split} \f] A position with these cartesian coordinates is returned. */
    Position centralPositionInCell(int m) const;

    /** This function returns a random location from the dust cell with cell number \f$m\f$. For a
        cartesian dust grid structure, the function first determines the bin indices \f$i\f$ and
        \f$k\f$ in the X, Y and Z directions that correspond to the cell number \f$m\f$. Then
        random coordinates \f$x\f$, \f$y\f$ and \f$z\f$ are determined using \f[ \begin{split} x &=
        x_i + {\cal{X}}_1\,(x_{i+1}-x_i) \\ y &= y_j + {\cal{X}}_2\,(y_{j+1}-y_j) \\ z &= z_k +
        {\cal{X}}_3\, (z_{k+1}-z_k), \end{split} \f] with \f${\cal{X}}_1\f$, \f${\cal{X}}_2\f$ and
        \f${\cal{X}}_3\f$ three uniform deviates. A position with these cartesian coordinates is
        returned. */
    Position randomPositionInCell(int m) const;

    /** This function returns a DustGridPath object, corresponding to path through the grid
        starting at the position \f${\bf{r}}\f$ into the direction \f${\bf{k}}\f$. This
        DustGridPath consists of three vectors: the first one lists the cell numbers \f$m\f$ of all
        the cells crossed by the path, the second lists the path length \f$\Delta s\f$ covered in
        each of these dust cells, and the third lists the total covered path length \f$s\f$ until
        the end of each cell is encountered. */
    DustGridPath path(Position bfr, Direction bfk) const;

protected:
    /** This function writes the intersection of the dust grid structure with the xy plane
        to the specified DustGridPlotFile object. */
    void write_xy(DustGridPlotFile* outfile) const;

    /** This function writes the intersection of the dust grid structure with the xz plane
        to the specified DustGridPlotFile object. */
    void write_xz(DustGridPlotFile* outfile) const;

    /** This function writes the intersection of the dust grid structure with the yz plane
        to the specified DustGridPlotFile object. */
    void write_yz(DustGridPlotFile* outfile) const;

    /** This function writes 3D information for all cells in the dust grid structure to the
        specified DustGridPlotFile object. */
    void write_xyz(DustGridPlotFile* outfile) const;

private:
    /** This private function determines the number \f$i\f$ such that \f$x_i \leq x < x_{i+1}\f$.
        It uses a simple bisection method borrowed from the Numerical Recipes book. */
    int whichxcell(double x) const;

    /** This private function determines the number \f$j\f$ such that \f$y_j \leq y < y_{j+1}\f$.
        It uses a simple bisection method borrowed from the Numerical Recipes book. */
    int whichycell(double y) const;

    /** This private function determines the number \f$k\f$ such that \f$z_k \leq z < z_{k+1}\f$.
        It uses a simple bisection method borrowed from the Numerical Recipes book. */
    int whichzcell(double z) const;

    /** This function returns the cell number \f$m\f$ corresponding to the three bin indices
        \f$i\f$, \f$j\f$ and \f$k\f$. The correspondence is \f$m=k+j\,N_z+i\,N_y\,N_z\f$. */
    int index(int i, int j, int k) const;

    /** This function calculates the three bin indices \f$i\f$, \f$j\f$ and \f$k\f$ of the cell
        number \f$m\f$, and then returns the coordinates of the corresponding cell as a Box object.
        Since the relation between the cell number and the three bin indices is
        \f$m=k+j\,N_z+i\,N_y\,N_z\f$, we can use the formulae \f[ \begin{split} i &= \lfloor
        m/(N_y\,N_z) \rfloor \\ j &= \lfloor (m-i\,N_y\,N_z)/N_z \rfloor \\ k &=
        m\,{\text{mod}}\,N_z. \end{split} \f] */
    Box box(int m) const;

    //======================== Data Members ========================

protected:
    // data members to be initialized in a subclass
    int _Nx;
    int _Ny;
    int _Nz;
    double _xmin, _xmax;
    double _ymin, _ymax;
    double _zmin, _zmax;
    Array _xv;
    Array _yv;
    Array _zv;
};

////////////////////////////////////////////////////////////////////

#endif // CUBDUSTGRIDSTRUCTURE_HPP
