/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef SPHERICALADAPTIVEMESH_HPP
#define SPHERICALADAPTIVEMESH_HPP

#include <vector>
#include <QList>
#include <QHash>
#include <QString>
#include "Box.hpp"
#include "Position.hpp"
class AdaptiveMeshFile;
class AdaptiveMeshNode;
class Random;

////////////////////////////////////////////////////////////////////

/** The SphericalAdaptiveMesh class is used to import data into SKIRT defined on a
    three-dimensional Adaptive Mesh Refinement (AMR) grid using spherical coordinates
    \f$(r,\theta,\phi)\f$. The constructor reads the relevant data from a data file in one of the
    supported formats. The instance methods allow interrogating the resulting data structure in
    various ways. Once an AdaptiveMesh instance has been constructed, its data is never modified
    (which is somewhat ironic in view of the class name). Consequently all methods are re-entrant.

    An SphericalAdaptiveMesh instance represents one or more scalar fields over a given shell between
    inner radius \f$r_\text{in}\f$ and outer radius \f$r_\text{out}\f$. The meaning of the fields, and
    the units in which the values are expressed, are not relevant for this class and are determined
    by the caller in agreement with the supplier of the data file. Since at least one of the fields
    will likely be interpreted as a density distribution over the domain, this class offers some
    specific functionality to support that use case.

    For more information on the nested cell structure of an adaptive mesh, refer to the AdaptiveMesh
    class. For more information on the supported adaptive mesh file formats, refer to the
    AdaptiveMeshFile class and its subclasses. */
class SphericalAdaptiveMesh
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
        index may be specified more than once. Negative value are ignored. The arguments \em
        rin and \em rout specify the inner and outer radius of the shell comprising the domain. */
    SphericalAdaptiveMesh(AdaptiveMeshFile* meshfile, QList<int> fieldIndices, double rin, double rout);

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
    ~SphericalAdaptiveMesh();

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

    //========================= Data members =======================

private:
    // domain extent and small fraction of it
    double _rin;
    double _rout;
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

    // node tree
    AdaptiveMeshNode* _root;                    // root node representing the complete domain
    std::vector<AdaptiveMeshNode*> _leafnodes;  // leaf nodes indexed on m
};

////////////////////////////////////////////////////////////////////

#endif // SPHERICALADAPTIVEMESH_HPP
