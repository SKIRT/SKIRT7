/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef AXSPHEDUSTGRIDSTRUCTURE_HPP
#define AXSPHEDUSTGRIDSTRUCTURE_HPP

#include "Array.hpp"
#include "DustGridStructure.hpp"

//////////////////////////////////////////////////////////////////////

/** The AxSpheDustGridStructure class is an abstract subclass of the general DustGridStructure
    class, and represents two-dimensional, axisymmetric dust grid structures using spherical
    coordinates. The grid is defined in the meridional plane and rotated around the Z-axis. Each
    cell in such a grid is a torus. The meridional grid is specified through a set of \f$N_r+1\f$
    radial grid points \f$r_i\f$ (with \f$i=0,\ldots,N_r\f$) and a set of \f$N_\theta+1\f$ angular
    grid points \f$\theta_k\f$ (with \f$k=0,\ldots,N_\theta\f$). In total there are
    \f$N_{\text{cells}} = N_r\,N_\theta\f$ cells in the dust grid structure.

    To ensure that the dust grid covers a convex volume, the outer grid points are required to have
    the following values: \f[ \begin{split} r_0 &= 0 \\ r_{N_r} &= r_\text{max} \\ \theta_0 &= 0 \\
    \theta_{N_\theta} &= \pi \end{split} \f] where \f$r_{\text{max}}\f$ is the maximum radial
    extent of the grid. Furthermore, one of the angular grid points must have the value
    \f$\theta=\pi/2\f$. */
class AxSpheDustGridStructure : public DustGridStructure
{
    Q_OBJECT
    Q_CLASSINFO("Title", "an axisymmetric dust grid structure")
    Q_CLASSINFO("AllowedIf", "!GenGeometry")
    Q_CLASSINFO("AllowedIf", "!AdaptiveMeshStellarComp")
    Q_CLASSINFO("AllowedIf", "!SPHStellarComp")
    Q_CLASSINFO("AllowedIf", "!VoronoiStellarComp")
    Q_CLASSINFO("AllowedIf", "!AdaptiveMeshDustDistribution")
    Q_CLASSINFO("AllowedIf", "!SPHDustDistribution")
    Q_CLASSINFO("AllowedIf", "!VoronoiDustDistribution")

    //============= Construction - Setup - Destruction =============

protected:
    /** The default constructor; it is protected since this is an abstract class. */
    AxSpheDustGridStructure();

    /** This function pre-calculates and stores the opening angle cosines \f$c_k = \cos\theta_k\f$
        for the boundary cones in the angular grid, to help speed up calculations in the path()
        method. */
    void setupSelfAfter();

    //======================== Other Functions =======================

public:
    /** This function returns the dimension of the grid, which is 2 for all subclasses of this
        class since it is a base class for axisymmetric grids using spherical coordinates. */
    int dimension() const;

    /** This function returns the maximum extent \f$x_{\text{max}}\f$ of the grid structure in the
        \f$x\f$ direction. In this class, the function returns the maximum radial extent
        \f$r_{\text{max}}\f$. */
    double xmax() const;

    /** This function returns the maximum extent \f$y_{\text{max}}\f$ of the grid structure in the
        \f$y\f$ direction. In this class, the function returns the maximum radial extent
        \f$r_{\text{max}}\f$. */
    double ymax() const;

    /** This function returns the maximum extent \f$z_{\text{max}}\f$ of the grid structure in the
        \f$z\f$ direction. In this class, the function returns the maximum radial extent
        \f$r_{\text{max}}\f$. */
    double zmax() const;

    /** This function returns the volume of the dust cell with cell number \f$m\f$. In this class,
        the function determines the radial and angular bin indices \f$i\f$ and \f$k\f$ that
        correspond to the cell number \f$m\f$, and then calculates the volume as \f[ V =
        \frac{2\pi}{3} \left(r_{i+1}^3-r_i^3\right) \left(\cos\theta_k-\cos\theta_{k+1}\right). \f]
        */
    double volume(int m) const;

    /** This function returns the number of the dust cell that contains the position
        \f${\bf{r}}\f$. In this class, the function determines the radial and angular bin indices
        and calculates the correct cell number based on these two numbers. */
    int whichcell(Position bfr) const;

    /** This function returns the central location from the dust cell with cell number \f$m\f$. In
        this class, the function first determines the radial and angular bin indices \f$i\f$ and
        \f$k\f$ that correspond to the cell number \f$m\f$. The spherical coordinates of the
        central position are subsequently determined from \f[ \begin{split} r &= \frac{r_i +
        r_{i+1}}{2} \\ \theta &= \frac{\theta_k + \theta_{k+1}}{2} \\ \phi &= 0. \end{split} \f] */
    Position centralPositionInCell(int m) const;

    /** This function returns a random location from the dust cell with cell number \f$m\f$. In
        this class, the function first determines the radial and angular bin indices \f$i\f$ and
        \f$k\f$ that correspond to the cell number \f$m\f$. Then a random radius \f$r\f$, a random
        inclination \f$\theta\f$, and a random azimuth \f$\phi\f$ are determined using \f[
        \begin{split} r &= \left( r_i^2 + {\cal{X}}_1\,(r_{i+1}^2-r_i^2) \right)^{1/2} \\ \theta &=
        \theta_k + {\cal{X}}_2\, (\theta_{k+1}-\theta_k) \\ \phi &= 2\pi\,{\cal{X}}_3, \end{split}
        \f] with \f${\cal{X}}_1\f$, \f${\cal{X}}_2\f$ and \f${\cal{X}}_3\f$ three uniform deviates.
        */
    Position randomPositionInCell(int m) const;

    /** This function calculates a path through the grid. The DustGridPath object passed as an
        argument specifies the starting position \f${\bf{r}}\f$ and the direction \f${\bf{k}}\f$
        for the path. The data on the calculated path are added back into the same object.

        We represent the path by its parameter equation \f${\bf{x}}={\bf{r}}+s\,{\bf{k}}\f$, and we
        assume that \f${\bf{k}}\f$ is a unit vector. The two intersection points with a radial
        boundary sphere \f${\bf{x}}^2=R^2\f$ are obtained by solving the quadratic equation \f$s^2
        + 2\,({\bf{r}}\cdot{\bf{k}})\,s + ({\bf{r}}^2-R^2)=0\f$ for \f$s\f$. The two intersection
        points with an angular boundary cone \f$x_z^2=c^2\,{\bf{x}}^2\f$ (with \f$c=\cos\theta\f$)
        are obtained by solving the quadratic equation \f$(c^2-k_z^2)\,s^2 +
        2\,(c^2\,{\bf{r}}\cdot{\bf{k}}-r_z k_z)\,s + (c^2\,{\bf{r}}^2-r_z^2)=0\f$ for \f$s\f$. The
        intersection points with the reflected cone are always more distant than the other cell
        boundaries (the requirement to include the xy-plane \f$\theta=\pi/2\f$ in the grid ensures
        that this is true) and thus these phantom points are automatically ignored. */
    void path(DustGridPath* path) const;

protected:
    /** This function writes the intersection of the dust grid structure with the xy plane to the
        specified DustGridPlotFile object. */
    void write_xy(DustGridPlotFile* outfile) const;

    /** This function writes the intersection of the dust grid structure with the xz plane to the
        specified DustGridPlotFile object. */
    void write_xz(DustGridPlotFile* outfile) const;

private:
    /** This private function returns the cell number corresponding to the radial index \f$i\f$ and
        the angular index \f$k\f$. The correspondence is simply \f$m=k+N_\theta\,i\f$. */
    int index(int i, int k) const;

    /** This private function calculates the radial index \f$i\f$ and the angular index \f$k\f$
        from a cell number \f$m\f$. As the correspondence between \f$m\f$, \f$i\f$ and \f$k\f$ is
        given by \f$m=k+N_\theta\,i\f$, one directly obtains \f$i=\lfloor m/N_\theta \rfloor\f$ and
        \f$k=m\!\mod N_\theta\f$. */
    void invertindex(int m, int& i, int& k) const;

    //======================== Data Members ========================

protected:
    // data members to be initialized in a subclass
    int _Nr;
    int _Ntheta;
    double _rmax;
    Array _rv;
    Array _thetav;

    // data members initialized in this class
    Array _cv;
};

//////////////////////////////////////////////////////////////////////

#endif // AXSPHEDUSTGRIDSTRUCTURE_HPP
