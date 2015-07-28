/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef ADAPTIVEMESH_HPP
#define ADAPTIVEMESH_HPP

#include <vector>
#include <QList>
#include <QHash>
#include <QString>
#include "Box.hpp"
#include "Position.hpp"
class AdaptiveMeshFile;
class AdaptiveMeshNode;
class DustGridPath;
class Log;
class Random;

////////////////////////////////////////////////////////////////////

/** The AdaptiveMesh class is used to import data on a cartesian three-dimensional (3D) Adaptive
    Mesh Refinement (AMR) grid into SKIRT. The constructor reads the relevant data from a data file
    in one of the supported formats. The instance methods allow interrogating the resulting data
    structure in various ways. Once an AdaptiveMesh instance has been constructed, its data is
    never modified (which is somewhat ironic in view of the class name). Consequently all methods
    are re-entrant.

    An AdaptiveMesh instance represents one or more scalar fields over a given three-dimensional
    spatial domain. The domain must be a cuboid and is defined using cartesian coordinates. The
    meaning of the fields, and the units in which the values are expressed, are not relevant for
    this class and are determined by the caller in agreement with the supplier of the data file.
    Since at least one of the fields will likely be interpreted as a density distribution over the
    domain, this class offers some specific functionality to support that use case.

    The adaptive mesh partitions the domain in cubodial cells, each holding a single value per
    field (i.e. the fields are considered to be constant over each cell). It uses a recursively
    nested linear grid structure to provide high resolution (i.e. small cells) in areas where it
    matters, without consuming memory in areas where the resolution can be lower. This structure is
    organized in a tree. Each tree node represents a cubodial portion of the domain, called its
    extent. The root node's extent is the complete domain. A nonleaf node distributes its extent over
    its child nodes using a regular linear grid. The number of subdivisions is defined separately
    for each node and can differ for each spatial dimension. A leaf node represents a cell holding
    field data values which are considered to be constant over the leaf node's extent. Collectively
    the leaf nodes form a partition of the domain, i.e. their extents cover the complete domain
    without overlapping one another.

    For more information on the supported adaptive mesh file formats, refer to the AdaptiveMeshFile
    class and its subclasses.
 */
class AdaptiveMesh
{
public:

    //================= Construction - Destruction =================

    /** The constructor reads the AMR data from the specified adaptive mesh file. The \em
        fieldIndices argument specifies a list of indices \f$g\f$ for the scalar fields
        \f$F_g({\bf{r}})\f$ that will be held by the newly constructed instance. The exact meaning
        of the field indices depends on the imported data file format, but usually they correspond
        to zero-based column or variable indices. The data file must contain a sufficient number of
        columns or variables to accommodate the highest index in the list; additional columns or
        variables in the file are ignored. The indices may be specified in any order, and the same
        index may be specified more than once. Negative value are ignored. The argument \em
        extent specifies the extent of the domain as a box lined up with the coordinate axes.
        Finally, the optional \em log argument specifies a logger appropriate for the simulation.
        The logger is used only for issuing warnings about stuck photons in the path() function;
        it can be omitted if the path() function is never called, or if no warnings should be
        issued. */
    AdaptiveMesh(AdaptiveMeshFile* meshfile, QList<int> fieldIndices, const Box& extent, Log* log=0);

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

    /** This function adds neighbor information to all leaf nodes in the adaptive mesh.
        Specifically, each leaf node maintains a list of its most likely neighbor at each of its
        six walls. This information, while optional, substantially accelerates the operation of the
        path() function. */
    void addNeighbors();

    /** The destructor releases the data structures allocated during construction. */
    ~AdaptiveMesh();

    //=============== Basic getters and interrogation ==============

    /** This function returns the number of leaf cells \f$N_\text{cells}\f$ in the mesh represented
        by this instance. */
    int Ncells() const;

    /** This function returns the Morton order cell index \f$0\le m \le N_{cells}-1\f$ for the
        cell containing the specified point \f${\bf{r}}\f$. If the point is outside the domain, the
        function returns -1. */
    int cellIndex(Position bfr) const;

    /** This function returns the volume of the complete domain. */
    double volume() const;

    /** This function returns the volume of the cell with given Morton order index \f$0\le m \le
        N_{cells}-1\f$. If the index is out of range a fatal error is thrown. */
    double volume(int m) const;

    /** This function returns the extent of the complete domain as a box lined up with the
        coordinate axes. */
    Box extent() const;

    /** This function returns the extent of the cell with given Morton order index \f$0\le m \le
        N_{cells}-1\f$ as a box lined up with the coordinate axes. If the index is out of range a
        fatal error is thrown. */
    Box extent(int m) const;

    /** This function returns the center of the cell with given Morton order index \f$0\le m \le
        N_{cells}-1\f$. If the index is out of range a fatal error is thrown. */
    Position centralPosition(int m) const;

    /** This function returns a random point in the cell with given Morton order index \f$0\le m
        \le N_{cells}-1\f$, drawn from a uniform distribution. If the index is out of range a fatal
        error is thrown. The first argument provides the random generator to be used. */
    Position randomPosition(Random* random, int m) const;

    /** This function returns the value \f$F_g(m)\f$ of the specified field in the cell with given
        Morton order index \f$0\le m \le N_{cells}-1\f$. The field is specified through its
        zero-based index \f$g\f$. If either index is out of range a fatal error is thrown. */
    double value(int g, int m) const;

    /** This function returns the value \f$F_g({\bf{r}})\f$ of the specified field at a given point
        \f${\bf{r}}\f$. The field is specified through its zero-based index \f$g\f$. If the field
        index is out of range a fatal error is thrown. If the point is outside the domain, the
        function returns zero. */
    double value(int g, Position bfr) const;

    //================ Density related interrogation ===============

    /** This function returns the value \f$D_h(m)\f$ of the density distribution with index \f$0\le
        h \le N_{distributions}-1\f$ in the cell with given Morton order index \f$0\le m \le
        N_{cells}-1\f$. If no density distribution was added or if one of the indices is out of
        range, a fatal error is thrown. */
    double density(int h, int m) const;

    /** This function returns the value \f$D_h({\bf{r}})\f$ of the density distribution with index
        \f$0\le h \le N_{distributions}-1\f$ at a given point \f${\bf{r}}\f$. If no density
        distribution was added or the index is out of range, a fatal error is thrown. If the point
        is outside the domain, the function returns zero. */
    double density(int h, Position bfr) const;

    /** This function returns the value \f$\sum_h D_h(m)\f$ of the total density distribution in the cell with given
        Morton order index \f$0\le m \le N_{cells}-1\f$. If no density distribution was added
        or if the cell index is out of range, a fatal error is thrown. */
    double density(int m) const;

    /** This function returns the value \f$\sum_h D_h({\bf{r}})\f$ of the total density distribution at a given point
        \f${\bf{r}}\f$. If no density distribution was added, a fatal error is
        thrown. If the point is outside the domain, the function returns zero. */
    double density(Position bfr) const;

    /** This function returns the value of the total density distribution integrated over the complete domain,
        with index \f$0\le h \le N_{distributions}-1\f$. In other words, it computes \f[ \iiint_\text{domain}
        D_h({\bf{r}}) \,\text{d}{\bf{r}} \approx \sum_{m=0}^{N_\text{cells}-1} D_h(m)\times V_m \f] This information
        is stored internally. */
    double integratedDensity(int h) const;

    /** This function returns the value of the total density distribution integrated over the complete domain,
        in other words it computes \f[ \iiint_\text{domain} \sum_h D_h({\bf{r}}) \,\text{d}{\bf{r}} \approx
        \sum_{m=0}^{N_\text{cells}-1} \sum_h D_h(m)\times V_m \f] This can be useful to help normalize the
        density distribution over the domain. If no density distribution was added,
        a fatal error is thrown. */
    double integratedDensity() const;

    /** This function returns the X-axis surface density of the total density distribution,
        defined as the integration of the density along the entire X-axis, \f[
        \Sigma_X = \int_{x_\text{min}}^{x_\text{max}} \rho(x,0,0)\, {\text{d}}x.\f] This
        integral is calculated numerically using 10000 samples along the X-axis. */
    double SigmaX() const;

    /** This function returns the Y-axis surface density of the total density distribution,
        defined as the integration of the density along the entire Y-axis, \f[
        \Sigma_Y = \int_{y_\text{min}}^{y_\text{max}} \rho(0,y,0)\, {\text{d}}y.\f] This
        integral is calculated numerically using 10000 samples along the Y-axis. */
    double SigmaY() const;

    /** This function returns the Z-axis surface density of the total density distribution,
        defined as the integration of the density along the entire Z-axis, \f[
        \Sigma_Z = \int_{z_\text{min}}^{z_\text{max}} \rho(0,0,z)\, {\text{d}}z.\f] This
        integral is calculated numerically using 10000 samples along the Z-axis. */
    double SigmaZ() const;

    //====================== Path construction =====================

    /** This function calculates a path through the grid. The DustGridPath object passed as an
        argument specifies the starting position \f${\bf{r}}\f$ and the direction \f${\bf{k}}\f$
        for the path. The data on the calculated path are added back into the same object. */
    void path(DustGridPath* path) const;

    //========================= Data members =======================

private:
    // logger appropriate for the simulation; used only for issuing warnings about stuck photons in path();
    // can be left at zero if the path() function is never called, or if no warnings should be issued
    Log* _log;

    // small fraction of domain extent
    double _eps;

    // field values
    QHash<int,int> _storageIndices;             // key: field index g    value: storage index s
    int _Ncells;                                // limit for index m
    std::vector< std::vector<double> > _fieldvalues;   // indexed on s and m

    // density distribution info
    int _Ndistribs;                             // limit for index h
    QList<int> _densityFields;                  // indexed on h; contains storage index s
    QList<int> _densityMultiplierFields;        // indexed on h; contains storage index s
    QList<double> _densityFractions;            // indexed on h
    double _integratedDensity;                  // total over all h and m (0 if there is no density distribution)
    std::vector<double> _integratedDensityv;    // the previous split per component

    // node tree
    AdaptiveMeshNode* _root;                    // root node representing the complete domain
    std::vector<AdaptiveMeshNode*> _leafnodes;  // leaf nodes indexed on m
};

////////////////////////////////////////////////////////////////////

#endif // ADAPTIVEMESH_HPP
