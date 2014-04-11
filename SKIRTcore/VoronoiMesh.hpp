/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef VORONOIMESH_HPP
#define VORONOIMESH_HPP

#include <vector>
#include <QList>
#include <QHash>
#include "Box.hpp"
#include "DustGridPath.hpp"
class DustParticleInterface;
class Random;
class VoronoiMeshFile;
namespace VoronoiMesh_Private { class VoronoiCell; class Node; }

////////////////////////////////////////////////////////////////////

/** The VoronoiMesh class is used to manage a cartesian three-dimensional Voronoi mesh. It offers
    methods to interrogate the data structure in various (sometimes quite advanced) ways. Once an
    VoronoiMesh instance has been constructed, its data is never modified. Consequently all methods
    are re-entrant.

    For the purposes of this class, a Voronoi mesh represents any number (including zero) of scalar
    fields over a given cuboidal spatial domain. The Voronoi mesh partitions the domain in
    polyhedra according to a Voronoi tesselation of the domain. Consider a given set of points in
    the domain, called particles. For each particle there will be a corresponding region consisting
    of all points in the domain closer to that particle than to any other. These regions are called
    Voronoi cells, and together they form the Voronoi tesselation of the domain. Each Voronoi cell
    holds a single value per field (i.e. the fields are considered to be constant over each cell).

    There are constructors to obtain the particle coordinates and corresponding field values from
    various sources, including vectors in memory and a data file. For more information on the
    supported Voronoi mesh file formats, refer to the VoronoiMeshFile class and its subclasses.

    The extent of the cuboidal domain and all particle coordinates must be provided to this class
    in SI units. The meaning of the fields, and the units in which the values are expressed, are
    not relevant for this class and are determined by the caller in agreement with the supplier of
    the data file. Since at least one of the fields will likely be interpreted as a density
    distribution over the domain, this class offers some specific functionality to support that use
    case.

    This class uses the Voro++ code written by Chris H. Rycroft (LBL / UC Berkeley) to build the
    Voronoi tesselation.
*/
class VoronoiMesh
{
    //================= Construction - Destruction =================

public:
    /** This constructor reads the mesh data from the specified Voronoi mesh file. The \em
        fieldIndices argument specifies a list of indices \f$g\f$ for the scalar fields
        \f$F_g({\bf{r}})\f$ that will be held by the newly constructed instance. The exact meaning
        of the field indices depends on the imported data file format, but usually they correspond
        to zero-based column or variable indices. The data file must contain a sufficient number of
        columns or variables to accommodate the highest index in the list; additional columns or
        variables in the file are ignored. The indices may be specified in any order, and the same
        index may be specified more than once. Negative values are ignored. The last argument \em
        extent specifies the extent of the domain as a box lined up with the coordinate axes.
        Any particles located outside of the domain are discarded. */
    VoronoiMesh(VoronoiMeshFile* meshfile, QList<int> fieldIndices, const Box& extent);

    /** This constructor obtains the particle coordinates from a DustParticleInterface instance.
        There are no field values associated with the particles. The last argument \em extent
        specifies the extent of the domain as a box lined up with the coordinate axes.
        Any particles located outside of the domain are discarded. */
    VoronoiMesh(DustParticleInterface* dpi, const Box& extent);

    /** This constructor uses the particle coordinates specified as a vector. There are no field
        values associated with the particles. The last argument \em extent specifies the extent of
        the domain as a box lined up with the coordinate axes. The specified particle locations
        are assumed to be inside the domain; no check is performed. */
    VoronoiMesh(const std::vector<Vec>& particles, const Box& extent);

private:
    /** This private function is called from each constructor. Given a list of generating
        particles, it actually builds the Voronoi tesselation and stores the corresponding list of
        cells including any properties relevant for supporting the interrogation capabilities
        offered by this class. All other data (such as Voronoi cell vertices, edges and faces) is
        discarded.

        The function performs the following steps:
         - add the particles to a Voro++ container, and compute the Voronoi cells one by one;
         - copy the relevant cell information (such as the list of neighboring cells) from the
           Voro++ data structures into our own;
         - build a data structure that allows fast retrieval of a list of the Voronoi cells
           possibly overlapping a given point in the domain (see below).

        To accelerate operation of the cellIndex() function, which is called quite frequently, the
        domain is partitioned yet again, this time using a linear cubodial grid. The cells in this
        grid are called \em blocks. For each block, the function builds and stores a list of all
        Voronoi cells that possibly overlap that block. Retrieving the list of cells possibly
        overlapping a given point in the domain then comes down to locating the block containing
        the point (which is trivial since the grid is linear). The current implementation uses a
        Voronoi cell's enclosing cuboid to test for intersection with a block. Performing a precise
        intersection test (which was once implemented) is \em really slow and the shortened block
        lists don't substantially accelerate the cellIndex() function.

        To further reduce the search time within blocks that overlaps with a large number of cells,
        this function builds a binary search tree on the cell particle locations for those blocks
        (see for example <a href="http://en.wikipedia.org/wiki/Kd-tree">en.wikipedia.org/wiki/Kd-tree</a>).
    */
    void buildMesh(const std::vector<Vec>& particles);

    /** This private function builds the binary search tree. TO DO: complete documentation. */
    VoronoiMesh_Private::Node* buildTree(std::vector<int>::iterator first, std::vector<int>::iterator last, int depth);

public:
    /** This function adds a density distribution accessed by functions such as density() and
        integratedDensity(). The first argument \em densityField specifies the index \f$g_d\f$ of
        the field that should be interpreted as a (not necessarily normalized) density distribution
        \f$D\f$ over the domain. If \em densityMultiplierField is nonnegative (the default value is
        -1), it specifies the index \f$g_m\f$ for the field that will serve as a multiplication
        factor for the density distribution. Finally, the density is always multiplied by the
        constant fraction \f$f\f$ specified by \em densityFraction (with a default value of 1). In
        other words the density distribution value for each cell is determined by
        \f$D=F_{g_d}\times F_{g_m}\times f\f$.

        This function can be called repeatedly to specify multiple density distributions, which will
        be accessable through index \f$h\f$ in order of addition. */
    void addDensityDistribution(int densityField, int densityMultiplierField = -1, double densityFraction = 1.);

    /** The destructor releases the data structures allocated during construction. */
    ~VoronoiMesh();

    //=============== Basic getters and interrogation ==============

public:
    /** This function returns the number of leaf cells \f$N_\text{cells}\f$ in the Voronoi mesh,
        which by definition equals the number of particles located inside the domain. */
    int Ncells() const;

    /** This function retrieves the average, minimum and maximum number of neighbors per Voronoi cell into
        its arguments. */
    void neighborStatistics(double& average, int& minimum, int& maximum) const;

    /** This function returns the number of blocks \f$N_\text{blocks}\f$ in each spatial direction
        of the regular grid used to accelerate operation of the cellIndex() function. The total
        number of cells in the grid is \f$N_\text{blocks}^3\f$. */
    int Nblocks() const;

    /** This function retrieves the average, minimum and maximum number of Voronoi cells possibly
        overlapping a block in the regular grid used to accelerate operation of the cellIndex()
        function. */
    void blockStatistics(double& average, int& minimum, int& maximum) const;

    /** This function retrieves the number of search trees built to accelerate operation of the
        cellIndex() function, and the average, minimum and maximum number of Voronoi cells in each
        search tree. */
    void treeStatistics(int& Ntrees, double& average, int& minimum, int& maximum) const;

    /** This function returns the cell index \f$0\le m \le N_{cells}-1\f$ for the cell containing
        the specified point \f${\bf{r}}\f$. If the point is outside the domain, the function
        returns -1. By definition of a Voronoi tesselation, the closest particle position
        determines the Voronoi cell containing the specified point.

        The function uses the block and search tree data structures created in buildMesh() during
        construction to accelerate its operation. It computes the appropriate block index from the
        coordinates of the specified point, which provides a list of Voronoi cells possibly overlapping
        the point. If there is a search tree for this block, the function uses it to locate the nearest
        point in \f${\cal{O}}(\log N)\f$ time. Otherwise it calculates the distance from the specified
        point to the particle positions for each of the possibly overlapping cells, determining the
        nearest one in linear time. For a small number of cells this is more efficient than using the
        search tree.
    */
    int cellIndex(Position bfr) const;

    /** This function returns the volume of the complete domain. */
    double volume() const;

    /** This function returns the volume of the Voronoi cell with given index \f$0\le m \le
        N_{cells}-1\f$. If the index is out of range a fatal error is thrown. */
    double volume(int m) const;

    /** This function returns the extent of the complete domain. */
    Box extent() const;

    /** This function returns the bounding box (enclosing cuboid lined up with the coordinate axes)
        of the Voronoi cell with given index \f$0\le m \le N_{cells}-1\f$. If the index is out of
        range a fatal error is thrown. */
    Box extent(int m) const;

    /** This function returns the position of the particle that generated the Voronoi cell with
        given index \f$0\le m \le N_{cells}-1\f$. If the index is out of range a fatal error is
        thrown. */
    Position particlePosition(int m) const;

    /** This function returns the centroid of the Voronoi cell with given index \f$0\le m \le
        N_{cells}-1\f$. If the index is out of range a fatal error is thrown. */
    Position centralPosition(int m) const;

    /** This function returns a random point in the Voronoi cell with given index \f$0\le m \le
        N_{cells}-1\f$, drawn from a uniform distribution. If the index is out of range a fatal
        error is thrown. The first argument provides the random generator to be used.

        The function generates uniformly distributed random points in the enclosing cuboid
        until one happens to be inside the cell. The candidate point is inside the cell if it is
        closer to the cell's particle position than to any neighbor cell's particle positions.
    */
    Position randomPosition(Random* random, int m) const;

private:
    /** This function returns true if the specified point is closer to the particle
        defining the cell with index \em m than to all of the particles defining the cells with the
        indices in the list \em ids; otherwise it returns false. If the list \em ids contains the
        neighbors of \em m then this function returns true if the point is inside cell \em m. */
    bool isPointClosestTo(Vec r, int m, const std::vector<int> &ids) const;

public:
    /** This function returns the value \f$F_g(m)\f$ of the specified field in the cell with given
        index \f$0\le m \le N_{cells}-1\f$. The field is specified through its zero-based index
        \f$g\f$. If either index is out of range a fatal error is thrown. */
    double value(int g, int m) const;

    /** This function returns the value \f$F_g({\bf{r}})\f$ of the specified field at a given point
        \f${\bf{r}}\f$. The field is specified through its zero-based index \f$g\f$. If the field
        index is out of range a fatal error is thrown. If the point is outside the domain, the
        function returns zero. */
    double value(int g, Position bfr) const;

    //================ Density related interrogation ===============

public:
    /** This function returns the value \f$D_h(m)\f$ of the density distribution with index \f$0\le
        h \le N_{distributions}-1\f$ in the cell with given index \f$0\le m \le N_{cells}-1\f$. If
        no density distribution was added or if one of the indices is out of range, a fatal error
        is thrown. */
    double density(int h, int m) const;

    /** This function returns the value \f$D_h({\bf{r}})\f$ of the density distribution with index
        \f$0\le h \le N_{distributions}-1\f$ at a given point \f${\bf{r}}\f$. If no density
        distribution was added or the index is out of range, a fatal error is thrown. If the point
        is outside the domain, the function returns zero. */
    double density(int h, Position bfr) const;

    /** This function returns the value \f$\sum_h D_h(m)\f$ of the total density distribution in
        the cell with given index \f$0\le m \le N_{cells}-1\f$. If no density distribution was
        added or if the cell index is out of range, a fatal error is thrown. */
    double density(int m) const;

    /** This function returns the value \f$\sum_h D_h({\bf{r}})\f$ of the total density
        distribution at a given point \f${\bf{r}}\f$. If no density distribution was added, a fatal
        error is thrown. If the point is outside the domain, the function returns zero. */
    double density(Position bfr) const;

    /** This function returns the value of the total density distribution integrated over the
        complete domain, in other words it computes \f[ \iiint_\text{domain} \sum_h D_h({\bf{r}})
        \,\text{d}{\bf{r}} \approx \sum_{m=0}^{N_\text{cells}-1} \sum_h D_h(m)\times V_m \f] This
        can be useful to help normalize the density distribution over the domain. If no density
        distribution was added, a fatal error is thrown. */
    double integratedDensity() const;

    /** This function returns the X-axis surface density of the total density
        distribution, defined as the integration of the density along the entire X-axis, \f[
        \Sigma_X = \int_{x_\text{min}}^{x_\text{max}} \rho(x,0,0)\, {\text{d}}x.\f] This
        integral is calculated numerically using 10000 samples along the X-axis. */
    double SigmaX() const;

    /** This function returns the Y-axis surface density of the total density
        distribution, defined as the integration of the density along the entire Y-axis, \f[
        \Sigma_Y = \int_{y_\text{min}}^{y_\text{max}} \rho(0,y,0)\, {\text{d}}y.\f] This
        integral is calculated numerically using 10000 samples along the Y-axis. */
    double SigmaY() const;

    /** This function returns the Z-axis surface density of the total density
        distribution, defined as the integration of the density along the entire Z-axis, \f[
        \Sigma_Z = \int_{z_\text{min}}^{z_\text{max}} \rho(0,0,z)\, {\text{d}}z.\f] This
        integral is calculated numerically using 10000 samples along the Z-axis. */
    double SigmaZ() const;

    //====================== Path construction =====================

public:
    /** This function returns a DustGridPath object, corresponding to the path through the mesh
        starting at the position \f${\bf{r}}\f$ into the direction \f${\bf{k}}\f$. This
        DustGridPath consists of three vectors: the first one lists the cell numbers \f$m\f$ of all
        the cells crossed by the path, the second lists the path length \f$\Delta s\f$ covered in
        each of these dust cells, and the third lists the total covered path length \f$s\f$ until
        the end of each cell is encountered.

        In the first stage, the function checks whether the start point is inside the domain. If
        so, the current point is simply initialized to the start point. If not, the function
        computes the path segment to the first intersection with one of the domain walls and moves
        the current point inside the domain. Finally the function determines the current cell, i.e.
        the cell containing the current point.

        In the second stage, the function loops over the algorithm that computes the exit point
        from the current cell, i.e. the intersection of the ray formed by the current point and
        the path direction with the current cell's boundary. By the nature of Voronoi cells, this
        algorithm also produces the ID of the neigboring cell without extra cost. If an exit point
        is found, the loop adds a segment to the output path, updates the current point and the
        current cell, and continues to the next iteration. If the exit is towards a domain wall,
        the path is complete and the loop is terminated. If no exit point is found, which shouldn't
        happen too often, this must be due to computational inaccuracies. In that case, no path
        segment is added, the current point is advanced by a small amount, and the new current cell
        is determined by calling the function whichcell().

        The algorithm that computes the exit point has the following input data:
        <TABLE>
        <TR><TD>\f$\bf{r}\f$</TD>               <TD>%Position of the current point</TD></TR>
        <TR><TD>\f$\bf{k}\f$</TD>               <TD>%Direction of the ray, normalized</TD></TR>
        <TR><TD>\f$m_r\f$</TD>                  <TD>ID of the cell containing the current point</TD></TR>
        <TR><TD>\f$m_i,\;i=0\ldots n-1\f$</TD>  <TD>IDs of the neighboring cells (\f$m_i>=0\f$)
                                                    or domain walls (\f$m_i<0\f$)</TD></TR>
        <TR><TD>\f$\mathbf{p}(m)\f$</TD>        <TD>Particle position for cell \f$m\f$ (implicit)</TD></TR>
        <TR><TD>\f$x_\text{min},x_\text{max},y_\text{min},y_\text{max},z_\text{min},z_\text{max}\f$</TD>
                                                <TD>Domain boundaries (implicit)</TD></TR>
        </TABLE>
        where the domain wall IDs are defined as follows:
        <TABLE>
        <TR><TD><B>Domain wall ID</B></TD>      <TD><B>Domain wall equation</B></TD></TR>
        <TR><TD>-1</TD>                         <TD>\f$x=x_\text{min}\f$</TD></TR>
        <TR><TD>-2</TD>                         <TD>\f$x=x_\text{max}\f$</TD></TR>
        <TR><TD>-3</TD>                         <TD>\f$y=y_\text{min}\f$</TD></TR>
        <TR><TD>-4</TD>                         <TD>\f$y=y_\text{max}\f$</TD></TR>
        <TR><TD>-5</TD>                         <TD>\f$z=z_\text{min}\f$</TD></TR>
        <TR><TD>-6</TD>                         <TD>\f$z=z_\text{max}\f$</TD></TR>
        </TABLE>

        The line containing the ray can be written as \f$\mathcal{L}(s)=\mathbf{r}+s\,\mathbf{k}\f$
        with \f$s\in\mathbb{R}\f$. The exit point can similarly be written as
        \f$\mathbf{q}=\mathbf{r}+s_q\,\mathbf{k}\f$ with \f$s_q>0\f$, and the distance covered within
        the cell is given by \f$s_q\f$. The ID of the cell next to the exit point is denoted \f$m_q\f$
        and is easily determined as explained below.

        The algorithm that computes the exit point proceeds as follows:
         - Calculate the set of values \f$\{s_i\}\f$ for the intersection points between the line
           \f$\mathcal{L}(s)\f$ and the planes defined by the neighbors or walls \f$m_i\f$
           (see below for details on the intersection calculation).
         - Select the smallest nonnegative value \f$s_q=\min\{s_i|s_i>0\}\f$ in the set to determine
           the exit point. The neighbor or wall ID with the same index \f$i\f$ determines the
           corresponding \f$m_q\f$.
         - If there is no nonnegative value in the set, no exit point has been found.

        To calculate \f$s_i\f$ for a regular neighbor \f$m_i\geq 0\f$, intersect the
        line \f$\mathcal{L}(s)=\mathbf{r}+s\,\mathbf{k}\f$ with the plane bisecting the points
        \f$\mathbf{p}(m_i)\f$ and \f$\mathbf{p}(m_r)\f$. An unnormalized vector perpendicular to
        this plane is given by \f[\mathbf{n}=\mathbf{p}(m_i)-\mathbf{p}(m_r)\f]
        and a point on the plane is given by
        \f[\mathbf{p}=\frac{\mathbf{p}(m_i)+\mathbf{p}(m_r)}{2}.\f] The equation
        of the plane can then be written as \f[\mathbf{n}\cdot(\mathbf{x}-\mathbf{p})=0.\f]
        Substituting \f$\mathbf{x}=\mathbf{r}+s_i\,\mathbf{k}\f$ and solving for \f$s_i\f$ provides
        \f[s_i=\frac{\mathbf{n}\cdot(\mathbf{p}-\mathbf{r})}{\mathbf{n}\cdot\mathbf{k}}.\f]
        If \f$\mathbf{n}\cdot\mathbf{k}=0\f$ the line and the plane are parallel and there
        is no intersection. In that case no \f$s_i\f$ is added to the set of candidate
        exit points.

        To calculate \f$s_i\f$ for a wall \f$m_i<0\f$, substitute the appropriate normal and
        position vectors for the wall plane in this last formula. For example, for the left wall
        with \f$m_i=-1\f$ one has \f$\mathbf{n}=(-1,0,0)\f$ and \f$\mathbf{p}=(x_\text{min},0,0)\f$
        so that \f[s_i=\frac{x_\text{min}-r_x}{k_x}.\f]
    */
    DustGridPath path(Position bfr, Direction bfk) const;

    //========================= Data members =======================

private:
    // domain
    Box _extent;                                // full extent
    double _eps;                                // small fraction of extent

    // field values
    QHash<int,int> _storageIndices;             // key: field index g    value: storage index s
    std::vector< std::vector<double> > _fieldvalues;   // indexed on s and m

    // density distribution info
    int _Ndistribs;                             // limit for index h
    QList<int> _densityFields;                  // indexed on h; contains storage index s
    QList<int> _densityMultiplierFields;        // indexed on h; contains storage index s
    QList<double> _densityFractions;            // indexed on h
    double _integratedDensity;                  // total over all h and m (0 if there is no density distribution)

    // Voronoi cells and blocks
    int _Ncells;                                // limit for index m
    int _nb;                                    // number of blocks in each dimension (limit for indices i,j,k)
    int _nb2;                                   // nb*nb
    int _nb3;                                   // nb*nb*nb
    std::vector<VoronoiMesh_Private::VoronoiCell*> _cells;  // cell objects, indexed on m
    std::vector< std::vector<int> > _blocklists;            // list of cell indices per block, indexed on i*_nb2+j*_nb+k
    std::vector< VoronoiMesh_Private::Node* > _blocktrees;  // root node of search tree or null for each block,
                                                            // indexed on i*_nb2+j*_nb+k
};

////////////////////////////////////////////////////////////////////

#endif // VORONOIMESH_HPP
