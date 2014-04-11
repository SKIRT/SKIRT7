/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef AXDUSTGRIDSTRUCTURE_HPP
#define AXDUSTGRIDSTRUCTURE_HPP

#include "Array.hpp"
#include "DustGridStructure.hpp"

//////////////////////////////////////////////////////////////////////

/** The AxDustGridStructure class is an abstract subclass of the general DustGridStructure class,
    and represents two-dimensional, axisymmetric dust grid structures. The grid consists of a
    cartesian grid in the meridional plane, which is turned around the Z-axis. Each cell in such a
    grid is a torus. Internally, an axisymmetric dust grid structure is specified through a set of
    \f$N_R+1\f$ radial grid points \f$R_i\f$ (with \f$i=0,\ldots,N_R\f$) and a set of \f$N_z+1\f$
    vertical grid points \f$z_k\f$ (with \f$k=0,\ldots,N_z\f$). In total there are
    \f$N_{\text{cells}} = N_R\,N_z\f$ cells in the dust grid structure. */
class AxDustGridStructure : public DustGridStructure
{
    Q_OBJECT
    Q_CLASSINFO("Title", "an axisymmetric dust grid structure")
    Q_CLASSINFO("AllowedIf", "!GenGeometry")
    Q_CLASSINFO("AllowedIf", "!AdaptiveMeshStellarSystem")
    Q_CLASSINFO("AllowedIf", "!SPHStellarSystem")
    Q_CLASSINFO("AllowedIf", "!VoronoiStellarSystem")
    Q_CLASSINFO("AllowedIf", "!AdaptiveMeshDustDistribution")
    Q_CLASSINFO("AllowedIf", "!SPHDustDistribution")
    Q_CLASSINFO("AllowedIf", "!VoronoiDustDistribution")

    //============= Construction - Setup - Destruction =============

protected:
    /** The default constructor; it is protected since this is an abstract class. */
    AxDustGridStructure();

    //======================== Other Functions =======================

public:
    /** This function returns the dimension of the grid, which is 2 for all subclasses of this
        class since it is a base class for axisymmetric grids. */
    int dimension() const;

    /** This function returns the maximum extent \f$x_{\text{max}}\f$ of the grid structure in the
        \f$x\f$ direction. This information is stored internally. For axisymmetric grid structures,
        it returns the maximum radial extent \f$R_{\text{max}}\f$. */
    double xmax() const;

    /** This function returns the maximum extent \f$y_{\text{max}}\f$ of the grid structure in the
        \f$y\f$ direction. This information is stored internally. For axisymmetric grid structures,
        it returns the maximum radial extent \f$R_{\text{max}}\f$. */
    double ymax() const;

    /** This function returns the maximum extent \f$z_{\text{max}}\f$ of the grid structure in the
        \f$z\f$ direction. This information is stored internally. */
    double zmax() const;

    /** This function returns the volume of the dust cell with cell number \f$m\f$. For an
        axisymmetric dust grid structure, the function determines the radial and vertical bin
        indices \f$i\f$ and \f$k\f$ that correspond to the cell number \f$m\f$, and then calculates
        the volume as \f[ V = \pi \left(R_{i+1}-R_i\right)^2 \left(z_{k+1}-z_k\right). \f] */
    double volume(int m) const;

    /** This function returns the number of the dust cell that contains the position
        \f${\bf{r}}\f$. It just determines the radial and vertical bin indices and calculates the
        correct cell number based on these two numbers. */
    int whichcell(Position bfr) const;

    /** This function returns the central location from the dust cell with cell number \f$m\f$. For
        an axisymmetric dust grid structure, the function first determines the radial and vertical
        bin indices \f$i\f$ and \f$k\f$ that correspond to the cell number \f$m\f$. The cylindrical
        coordinates of the central position are subsequently determined from \f[ \begin{split} R &=
        \frac{R_i + R_{i+1}}{2} \\ \phi &= 0 \\ z &= \frac{z_k + z_{k+1}}{2} \end{split} \f] A
        position with these cylindrical coordinates is returned. */
    Position centralPositionInCell(int m) const;

    /** This function returns a random location from the dust cell with cell number \f$m\f$. For an
        axisymmetric dust grid structure, the function first determines the radial and vertical bin
        indices \f$i\f$ and \f$k\f$ that correspond to the cell number \f$m\f$. Then a random
        radius \f$R\f$, a random azimuth \f$\phi\f$, and a random height \f$z\f$ are determined
        using \f[ \begin{split} R &= R_i + {\cal{X}}_1\,(R_{i+1}-R_i) \\ \phi &= 2\pi\,{\cal{X}}_2
        \\ z &= z_k + {\cal{X}}_3\, (z_{k+1}-z_k), \end{split} \f] with \f${\cal{X}}_1\f$,
        \f${\cal{X}}_2\f$ and \f${\cal{X}}_3\f$ three uniform deviates. A position with these
        cylindrical coordinates is returned. */
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

private:

    /** This private function determines the radial bin number \f$i\f$ such that \f$R_i \leq R <
        R_{i+1}\f$. It uses a simple bisection method borrowed from the Numerical Recipes book. */
    int whichRcell(double R) const;

    /** This private function determines the vertical bin number \f$k\f$ such that \f$z_k \leq z <
        z_{k+1}\f$. It uses a simple bisection method borrowed from the Numerical Recipes book. */
    int whichzcell(double z) const;

    /** This private function returns the cell number corresponding to the radial index \f$i\f$ and
        the vertical index \f$k\f$. The correspondence is simply \f$m=k+N_z\,i\f$. */
    int index(int i, int k) const;

    /** This private function calculates the radial index \f$i\f$ and the vertical index \f$k\f$
        from a cell number \f$m\f$. As the correspondence between \f$m\f$, \f$i\f$ and \f$k\f$ is
        given by \f$m=k+N_z\,i\f$, one directly obtains \f$i=\lfloor m/N_z \rfloor\f$ and
        \f$k=m\!\mod N_z\f$. */
    void invertindex(int m, int& i, int& k) const;

    //======================== Data Members ========================

protected:
    // data members to be initialized in a subclass
    int _NR;
    int _Nz;
    double _Rmax;
    double _zmin, _zmax;
    Array _Rv;
    Array _zv;
};

//////////////////////////////////////////////////////////////////////

#endif // AXDUSTGRIDSTRUCTURE_HPP
