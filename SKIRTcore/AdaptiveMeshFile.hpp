/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef ADAPTIVEMESHFILE_HPP
#define ADAPTIVEMESHFILE_HPP

#include "SimulationItem.hpp"

////////////////////////////////////////////////////////////////////

/** AdaptiveMeshFile is an abstract class used to read the relevant information on a cartesian
    three-dimensional (3D) Adaptive Mesh Refinement (AMR) grid from a data file. The
    AdaptiveMeshFile subclasses implement specific file formats, including a generic ASCII format.

    For the purposes of this class, an adaptive mesh represents one or more scalar fields over a
    given three-dimensional spatial domain. The domain must be a cuboid and is defined using
    cartesian coordinates. The meaning of the fields, and the units in which the values are
    expressed, are not relevant for this class.

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

    The cells in this three-dimensional data structure can be arranged in a linear sequence using
    Morton ordering. This ordering is obtained by performing a depth-first traversal of the tree,
    where each nonleaf node outputs its children in the order x-first, then y, then z. The process
    is illustrated below for a two-dimensional structure.

    \image html MortonOrder.png
    \image latex MortonOrder.png

    An instance of this class (or rather of one of its subclasses) supplies the mesh data to the
    caller as a sequence of \em node \em records reflecting the tree structure described above. In
    other words, the records are given in Morton order, and each record describes a particular
    nonleaf or leaf tree node:
      - Nonleaf: a nonleaf record contains three positive integers
        \f$N_x,N_y,N_z\f$ specifying the number of child nodes carried by this node in each spatial
        direction. The child nodes are on a regular linear grid as described above.
      - Leaf: a leaf record contains one or more floating point numbers, giving the \f$N_{fields}\f$
        values of the fields in the cell represented by this leaf node. The first number provides
        the value for field \f$F_0\f$, the second for \f$F_1\f$, and so on. All leaf records in the
        file must contain the same number of field values.

    Note that the data provided by this class does not specify the domain size, the meaning of the
    represented fields, nor the units in which the field values are expressed.
 */
class AdaptiveMeshFile : public SimulationItem
{
    Q_OBJECT
    Q_CLASSINFO("Title", "an adaptive mesh data file")

    Q_CLASSINFO("Property", "filename")
    Q_CLASSINFO("Title", "the name of the adaptive mesh data file")

    //================= Construction - Destruction =================

protected:
    /** The default constructor. It is declared protected since this is an abstract class. */
    AdaptiveMeshFile();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the name of the file containing the adaptive mesh data.  */
    Q_INVOKABLE void setFilename(QString value);

    /** Returns the name of the file containing the adaptive mesh data. */
    Q_INVOKABLE QString filename() const;

    //======================== Other Functions =======================

public:

    /** This function opens the adaptive mesh data file, or throws a fatal error if the file can't
        be opened. It does not yet read any records. */
    virtual void open() = 0;

    /** This function closes the adaptive mesh data file. */
    virtual void close() = 0;

    /** This function reads the next record from the file, and holds its information ready for
        inspection through the other functions of this class. The function returns true if a record
        was successfully read, or false if the end of the file was reached or another error
        occurred. */
    virtual bool read() = 0;

    /** This function returns true if the current record represents a nonleaf node, or false if
        the current record represents a leaf node. If there is no current record, the result is
        undefined. */
    virtual bool isNonLeaf() const = 0;

    /** If the current record represents a nonleaf node, this function returns \f$N_x,N_y,N_z\f$,
        i.e. the number of child nodes carried by the node in each spatial direction. If the
        current record represents a leaf node or if there is no current record, the result is
        undefined. */
    virtual void numChildNodes(int& nx, int& ny, int& nz) const = 0;

    /** If the current record represents a leaf node, this function returns the value \f$F_g\f$ of
        the field with given zero-based index \f$0\le g \le N_{fields}-1\f$. If the index is out of
        range, a fatal error is thrown. If the current record represents a nonleaf node or if there
        is no current record, the result is undefined. */
    virtual double value(int g) const = 0;

    //========================= Data members =======================

protected:
    // discoverable attributes
    QString _filename;
};

////////////////////////////////////////////////////////////////////

#endif // ADAPTIVEMESHFILE_HPP
