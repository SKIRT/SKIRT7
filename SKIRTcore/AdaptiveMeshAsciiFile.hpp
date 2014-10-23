/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef ADAPTIVEMESHASCIIFILE_HPP
#define ADAPTIVEMESHASCIIFILE_HPP

#include <QFile>
#include "AdaptiveMeshFile.hpp"

////////////////////////////////////////////////////////////////////

/**
The AdaptiveMeshAsciiFile class can read the relevant information on a cartesian
three-dimensional (3D) Adaptive Mesh Refinement (AMR) grid from a file in an ASCII format
defined specifically for this purpose.

The mesh data must be supplied as a regular text file with the format described here. The
overall makeup of the file reflects the tree structure described for the AdaptiveMeshFile
class. Each line describes a particular tree node (nonleaf or leaf), and the lines are given in
Morton order. Specifically, each line in the file is treated a separate entity and can be of
one of the following types:
 - Comment: lines having a crosshatch (#) as the first non-whitespace character, lines
   containing only whitespace and empty lines are ignored (and do not count in the Morton order).
 - Nonleaf: a nonleaf line has an exclamation mark (!) as the first non-whitespace character,
   followed by optional whitespace and then three whitespace-separated positive integer numbers
   \f$N_x,N_y,N_z\f$. These three numbers specify the number of child nodes carried by this node
   in each spatial direction. The child nodes are on a regular linear grid as described above.
 - Leaf: a leaf node contains one or more whitespace-separated floating point numbers, giving
   the \f$N_{fields}\f$ values of the fields in the cell represented by this leaf node. The
   first number provides the value for field \f$F_0\f$, the second for \f$F_1\f$, and so on.
   All leaf node lines in the file must contain the same number of field values.

The mesh shown in the illustration for the AdaptiveMeshFile class could be defined as follows:

\verbatim
# Example file for importing adaptive mesh data into SKIRT
# It defines the 2D mesh described in the class documentation
# For illustrative purposes:
#  - the mesh is assumed to have a single cell in the z direction
#  - the first field value indicates the Morton order index of the leaf cell
#  - the second and third field values indicate the x and y coordinates of
#    the cell's center using a domain with size 4. x 3. placed at the origin
#
! 4 3 1
 0 0.50 0.50
 1 1.50 0.50
 2 2.50 0.50
 3 3.50 0.50
 4 0.50 1.50
 5 1.50 1.50
! 2 2 1
 6 2.25 1.25
 7 2.75 1.25
 8 2.25 1.75
! 2 2 1
 9 2.63 1.63
10 2.88 1.63
11 2.63 1.88
12 2.88 1.88
! 2 2 1
13 3.25 1.25
14 3.75 1.25
15 3.25 1.75
16 3.75 1.75
17 0.50 2.50
18 1.50 2.50
! 2 2 1
19 2.25 2.25
20 2.75 2.25
21 2.25 2.75
22 2.75 2.75
! 2 2 1
23 3.25 3.25
24 3.75 3.25
25 3.25 3.75
26 3.75 3.75
\endverbatim
 */
class AdaptiveMeshAsciiFile : public AdaptiveMeshFile
{
    Q_OBJECT
    Q_CLASSINFO("Title", "an adaptive mesh data file in ASCII format")

    //================= Construction - Destruction =================

public:
    /** The default constructor. */
    Q_INVOKABLE AdaptiveMeshAsciiFile();

    //======================== Other Functions =======================

public:

    /** This function opens the adaptive mesh data file, or throws a fatal error if the file can't
        be opened. It does not yet read any records. */
    void open();

    /** This function closes the adaptive mesh data file. */
    void close();

    /** This function reads the next record from the file, and holds its information ready for
        inspection through the other functions of this class. The function returns true if a record
        was successfully read, or false if the end of the file was reached or another error
        occurred. */
    bool read();

    /** This function returns true if the current record represents a nonleaf node, or false if
        the current record represents a leaf node. If there is no current record, the result is
        undefined. */
    bool isNonLeaf() const;

    /** If the current record represents a nonleaf node, this function returns \f$N_x,N_y,N_z\f$,
        i.e. the number of child nodes carried by the node in each spatial direction. If the
        current record represents a leaf node or if there is no current record, the result is
        undefined. */
    void numChildNodes(int& nx, int& ny, int& nz) const;

    /** If the current record represents a leaf node, this function returns the value \f$F_g\f$ of
        the field with given zero-based index \f$0\le g \le N_{fields}-1\f$. If the index is out of
        range, a fatal error is thrown. If the current record represents a nonleaf node or if there
        is no current record, the result is undefined. */
     double value(int g) const;

    //========================= Data members =======================

private:
     QFile _infile;               // the input file
     QList<QByteArray> _columns;  // the columns of the current record, or empty if there is no current record
     bool _isNonLeaf;             // true if the current record is a nonleaf node, false otherwise
};

////////////////////////////////////////////////////////////////////

#endif // ADAPTIVEMESHASCIIFILE_HPP
