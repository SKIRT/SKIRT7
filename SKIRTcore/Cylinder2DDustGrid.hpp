/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef CYLINDER2DDUSTGRID_HPP
#define CYLINDER2DDUSTGRID_HPP

#include "Array.hpp"
#include "CylinderDustGrid.hpp"
#include "MoveableMesh.hpp"
#include "Random.hpp"

////////////////////////////////////////////////////////////////////

/** The Cylinder2DDustGrid class is subclass of the CylinderDustGrid class, and represents
    axisymmetric dust grids based on cylindrical coordinates. The grid is defined in the meridional
    plane and rotated around the Z-axis. The meridional grid is specified through a set of
    \f$N_R+1\f$ radial grid points \f$R_i\f$ (with \f$i=0,\ldots,N_R\f$) and a set of \f$N_z+1\f$
    vertical grid points \f$z_k\f$ (with \f$k=0,\ldots,N_z\f$). In total there are
    \f$N_{\text{cells}} = N_R\,N_z\f$ cells in the dust grid. */
class Cylinder2DDustGrid : public CylinderDustGrid
{
    Q_OBJECT
    Q_CLASSINFO("Title", "an axisymmetric dust grid in cylindrical coordinates")
    Q_CLASSINFO("AllowedIf", "!GenGeometry")
    Q_CLASSINFO("AllowedIf", "!AdaptiveMeshStellarComp")
    Q_CLASSINFO("AllowedIf", "!SPHStellarComp")
    Q_CLASSINFO("AllowedIf", "!VoronoiStellarComp")
    Q_CLASSINFO("AllowedIf", "!AdaptiveMeshDustDistribution")
    Q_CLASSINFO("AllowedIf", "!SPHDustDistribution")
    Q_CLASSINFO("AllowedIf", "!VoronoiDustDistribution")

    Q_CLASSINFO("Property", "meshR")
    Q_CLASSINFO("Title", "the bin distribution in the radial direction")
    Q_CLASSINFO("Default", "LinMesh")

    Q_CLASSINFO("Property", "meshZ")
    Q_CLASSINFO("Title", "the bin distribution in the Z direction")
    Q_CLASSINFO("Default", "LinMesh")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE Cylinder2DDustGrid();

    /** This function sets up a number of data members that depend on the Mesh objects configured
        for this grid. */
    void setupSelfAfter();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the bin distribution in the radial direction. */
    Q_INVOKABLE void setMeshR(Mesh* value);

    /** Returns the bin distribution in the radial direction. */
    Q_INVOKABLE Mesh* meshR() const;

    /** Sets the bin distribution in the Z direction. */
    Q_INVOKABLE void setMeshZ(MoveableMesh* value);

    /** Returns the bin distribution in the Z direction. */
    Q_INVOKABLE MoveableMesh* meshZ() const;

    //======================== Other Functions =======================

public:
    /** This function returns the dimension of the grid, which is 2 for this class. */
    int dimension() const;

    /** This function returns the number of cells in the dust grid. */
    int numCells() const;

    /** This function returns the volume of the dust cell with cell number \f$m\f$. For an
        axisymmetric dust grid, the function determines the radial and vertical bin indices \f$i\f$
        and \f$k\f$ that correspond to the cell number \f$m\f$, and then calculates the volume as
        \f[ V = \pi \left(R_{i+1}-R_i\right)^2 \left(z_{k+1}-z_k\right). \f] */
    double volume(int m) const;

    /** This function returns the number of the dust cell that contains the position
        \f${\bf{r}}\f$. It just determines the radial and vertical bin indices and calculates the
        correct cell number based on these two numbers. */
    int whichcell(Position bfr) const;

    /** This function returns the central location from the dust cell with cell number \f$m\f$. For
        an axisymmetric dust grid, the function first determines the radial and vertical bin
        indices \f$i\f$ and \f$k\f$ that correspond to the cell number \f$m\f$. The cylindrical
        coordinates of the central position are subsequently determined from \f[ \begin{split} R &=
        \frac{R_i + R_{i+1}}{2} \\ \phi &= 0 \\ z &= \frac{z_k + z_{k+1}}{2} \end{split} \f] A
        position with these cylindrical coordinates is returned. */
    Position centralPositionInCell(int m) const;

    /** This function returns a random location from the dust cell with cell number \f$m\f$. For an
        axisymmetric dust grid, the function first determines the radial and vertical bin indices
        \f$i\f$ and \f$k\f$ that correspond to the cell number \f$m\f$. Then a random radius
        \f$R\f$, a random azimuth \f$\phi\f$, and a random height \f$z\f$ are determined using \f[
        \begin{split} R &= R_i + {\cal{X}}_1\,(R_{i+1}-R_i) \\ \phi &= 2\pi\,{\cal{X}}_2 \\ z &=
        z_k + {\cal{X}}_3\, (z_{k+1}-z_k), \end{split} \f] with \f${\cal{X}}_1\f$,
        \f${\cal{X}}_2\f$ and \f${\cal{X}}_3\f$ three uniform deviates. A position with these
        cylindrical coordinates is returned. */
    Position randomPositionInCell(int m) const;

    /** This function calculates a path through the grid. The DustGridPath object passed as an
        argument specifies the starting position \f${\bf{r}}\f$ and the direction \f${\bf{k}}\f$
        for the path. The data on the calculated path are added back into the same object. */
    void path(DustGridPath* path) const;

private:
    /** This function writes the intersection of the dust grid with the xy plane to the specified
        DustGridPlotFile object. */
    void write_xy(DustGridPlotFile* outfile) const;

    /** This function writes the intersection of the dust grid with the xz plane to the specified
        DustGridPlotFile object. */
    void write_xz(DustGridPlotFile* outfile) const;

private:
    /** This private function returns the cell number corresponding to the radial index \f$i\f$ and
        the vertical index \f$k\f$. The correspondence is simply \f$m=k+N_z\,i\f$. */
    int index(int i, int k) const;

    /** This private function calculates the radial index \f$i\f$ and the vertical index \f$k\f$
        from a cell number \f$m\f$. As the correspondence between \f$m\f$, \f$i\f$ and \f$k\f$ is
        given by \f$m=k+N_z\,i\f$, one directly obtains \f$i=\lfloor m/N_z \rfloor\f$ and
        \f$k=m\!\mod N_z\f$. */
    void invertindex(int m, int& i, int& k) const;

    //======================== Data Members ========================

private:
    // discoverable properties
    Mesh* _meshR;
    MoveableMesh* _meshz;

    // other data members
    Random* _random;
    int _NR;
    int _Nz;
    Array _Rv;
    Array _zv;
};

//////////////////////////////////////////////////////////////////////

#endif // CYLINDER2DDUSTGRID_HPP
