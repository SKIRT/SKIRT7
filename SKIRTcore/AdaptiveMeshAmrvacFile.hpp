/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef ADAPTIVEMESHAMRVACFILE_HPP
#define ADAPTIVEMESHAMRVACFILE_HPP

#include <vector>
#include <QFile>
#include "AdaptiveMeshFile.hpp"

////////////////////////////////////////////////////////////////////

/** The AdaptiveMeshAmrvacFile class can read the relevant information on a cartesian
    three-dimensional (3D) Adaptive Mesh Refinement (AMR) grid from a file in the format produced
    by the MPI-AMRVAC code developed in Leuven; see for example Keppens et al 2012. The web pages
    http://homes.esat.kuleuven.be/~keppens/amrstructure.html and
    http://homes.esat.kuleuven.be/~keppens/fileformat.html provide an overview of the data format.
    Specific information was obtained from a python script kindly written for this purpose by Tom
    Hendrix.

    This class only supports cartesian grids. It converts 1D and 2D grids to 3D grids by assuming
    a thickness of 1 cell in the missing directions.

    For some reason the MPI-AMRVAC data file does not contain the size of the mesh at the coursest
    level. This information must be provided separately as part of the ski file.
*/
class AdaptiveMeshAmrvacFile : public AdaptiveMeshFile
{
    Q_OBJECT
    Q_CLASSINFO("Title", "an adaptive mesh data file in MPI-AMRVAC format")

    //================= Construction - Destruction =================

public:
    /** The default constructor. */
    Q_INVOKABLE AdaptiveMeshAmrvacFile();

    Q_CLASSINFO("Property", "levelOneX")
    Q_CLASSINFO("Title", "the number of mesh cells at the coursest level, in the X direction")
    Q_CLASSINFO("MinValue", "1")
    Q_CLASSINFO("MaxValue", "10000")

    Q_CLASSINFO("Property", "levelOneY")
    Q_CLASSINFO("Title", "the number of mesh cells at the coursest level, in the Y direction")
    Q_CLASSINFO("MinValue", "1")
    Q_CLASSINFO("MaxValue", "10000")

    Q_CLASSINFO("Property", "levelOneZ")
    Q_CLASSINFO("Title", "the number of mesh cells at the coursest level, in the Z direction")
    Q_CLASSINFO("MinValue", "1")
    Q_CLASSINFO("MaxValue", "10000")

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the number of mesh cells at the coursest level, in the X direction. */
    Q_INVOKABLE void setLevelOneX(int value);

    /** Returns the number of mesh cells at the coursest level, in the X direction. */
    Q_INVOKABLE int levelOneX() const;

    /** Sets the number of mesh cells at the coursest level, in the Y direction. */
    Q_INVOKABLE void setLevelOneY(int value);

    /** Returns the number of mesh cells at the coursest level, in the Y direction. */
    Q_INVOKABLE int levelOneY() const;

    /** Sets the number of mesh cells at the coursest level, in the Z direction. */
    Q_INVOKABLE void setLevelOneZ(int value);

    /** Returns the number of mesh cells at the coursest level, in the Z direction. */
    Q_INVOKABLE int levelOneZ() const;

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
    // information about the mesh provided by the user
    int _nxlone[3];    // number of mesh cells at the coursest level, in each direction

    // information about the mesh read from the input file
    int _nblocks;      // total number of blocks in the mesh
    int _ndims;        // dimension of the mesh (1D, 2D or 3D)
    int _nvars;        // number of variables in each cell
    int _nx[3];        // number of mesh cells in each block, in each direction
    int _ng[3];        // number of blocks at the coursest level, in each direction
    int _nr[3];        // refinement factor for nested level, in each direction
    int _ncells;       // number of cells in a block
    int _blocksize;    // the size of a block in bytes
    std::vector<bool> _forest; // the forest representing the grid structure
    int _forestsize;   // the size of the forest vector

    // the input file and the current record
    QFile _infile;     // the input file
    int _state;        // indication of the current record (maintained by the "state machine" in read())
    std::vector<double> _block; // the values for all cells in the current block, if there is one
    int _forestindex;  // the index of the next forest item to be read
};

////////////////////////////////////////////////////////////////////

#endif // ADAPTIVEMESHAMRVACFILE_HPP
