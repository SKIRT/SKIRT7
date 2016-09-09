/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef DISTMEMTABLE_HPP
#define DISTMEMTABLE_HPP

#include <QString>

#include "Log.hpp"
#include "Table.hpp"
class ProcessAssigner;
class PeerToPeerCommunicator;

////////////////////////////////////////////////////////////////////

/** An object of this class represents a table of which the data is distributed across all MPI
    processes. When correctly set up and used, it will appear similar to an ordinary 2D table
    concerning element access. Elements can be retrieved or written using a ()-operator that takes
    two positive integers, the first one indicating the row index, and the second one the column
    index. However, under data-parallelization, only a small part of all the data will actually be
    stored at the calling process, and only the available index combinations can be called (see \c
    operator()).

    One of the most important features of this class, is that it can switch between two ways of
    parallelized data storage. A 2D block of data can be cut either vertically or horizontally,
    letting each process store a set of column or rows respectively. When a certain operation works
    on the data column wise, one can opt to only store the relevant columns. It is often the case
    however, that another calculation needs this same data in a row format. For such situations,
    performing a \em global \em transposition is key. A \c ParallelTable can therefore be used in
    two ways. The parallel program either fills in the data in a column format, and then transposes
    it to a row format, or it performs this operation the other way round. At initialization, one
    of these two operating modes has to be chosen using an enum argument (see \c enum \c class \c WriteState).

    Exactly which columns or rows will be stored by each process, can be almost freely decided.
    Deciding which columns (or rows) need to be stored on a certain process, happens through the \c
    ProcessAssigner class. Because a \c ProcessAssigner can also be used to decide the work
    distribution between processes, using a common asigner for both work and data parallelization
    will ensure that the two are consistent. Any index conversions that need to happen to access
    the data, can be performed by calling one of the indexing function on a \c ProcessAssigner
    object. Thus the freedom for dividing the data is essentially limited by the number of \c ProcessAssigner
    implementations.

    The switching from a column-based to a row-based parallel storage scheme or vice versa is implemented
    with a call to the alltoallw function of MPI. This is a very general communication
    function, where every process can be made to send a certain set of data to every other
    process (see \c switchScheme()).

    This class can also store data in a regular way, when data-parallelization is not desired. This way,
    the code can run with or without data parallelization using the same data structure. This mode is
    called the \em non-distributed mode, as opposed to the \em distributed mode used for data-
    parallelization. */
class ParallelTable
{
public:
    /** This enum is used to indicate the mode that should be used. An argument of this type should
        be given at initialization and is stored by the paralleltable for the rest of its lifetime.
        The meaning of the two available values can be summarized as follows:
        + \em COLUMNS: Write in a column format -> communicate -> read in a row format
        + \em ROW: Write in a row format -> communicate -> read in a column format
        .
    The chosen mode hence determies how the data is stored before and after performing the global
    transposition, defining a direction for the flow of data. If one wants to switch back to the
    initial format, the \c reset() function can be used. This clears all data however, and hence it
    is made sure that data/information can only flow in a fixed direction. */
    enum class WriteState { COLUMN, ROW };

    //============= Construction - Setup - Destruction =============

    /** The default constructor creates an uninitialized ParallelTable object. Before it can be used,
     one of the initialization functions below needs to be called. */
     ParallelTable();

    /** This function prepares the ParallelTable for use. This version of \c initialize should be
        used if one wants to activate data paralellization. As arguments it takes:
    - A name, which will be used in log messages.
    - A \c WriteState argument, determining the mode of operation as described above
    - Two \c ProcessAssigner pointers, the former indicates which columns should be stored by each
    process, and the latter does the same for the rows. When the execution of the work and the
    storage of the corresponding data are based on the same assigner, consistency between the two
    is ensured.
    - All MPI operations can be performed through the given communicator.
    .
    The table derives its dimensions from the properties of the given assigners, and allocates the
    necessary memory.
    */
    void initialize(QString name, WriteState writeOn, const ProcessAssigner* colAssigner,
                    const ProcessAssigner* rowAssigner, PeerToPeerCommunicator* comm);

    /** This version of initialize sets up the ParallelTable to operate without data parallelization.
        The dimensions are simply given as arguments. A WriteState argument is still required is still
        required for consistency reasons. The communicator is used to sum the contents of the table
        across all processes when performing the scheme switch.*/
    void initialize(QString name, WriteState writeOn, int columns, int rows, PeerToPeerCommunicator* comm);

    //======================== Other Functions =======================

    /** This function indicates whether the ParallelTable has been initialized. */
    bool initialized() const;
    /** This function indicates whether the data is distributed across all processes or not. */
    bool distributed() const;

    void switchScheme(); // Change from a column based write mode to a row based read-only mode, or vice versa.
    void reset(); // Switch back to the writing mode, and set any contents to zero.

    // Access operators
    double& operator()(size_t i, size_t j); // write operator
    const double& operator()(size_t i, size_t j) const; // read operator

    // Summation functions without communication. Only work for currently available rows/columns.
    double sumColumn(size_t j) const; // sum of the values in a column
    double sumRow(size_t i) const; // sum of the values in a row

    // Summation functions including communication. This communication is collective, hence all processes need to call
    // the same function
    Array stackColumns() const; // sum of al the columns: each element is a row sum
    Array stackRows() const; // sum of al the rows: each elements is a column sum
    double sumEverything() const;

private:
    // Different ways of communicating the data between processes
    void sum_all();
    void columsToRows();
    void rowsToColums();
    // Sets the specified table to its correct size
    void allocateColumns();
    void allocateRows();
    // Frees the memory occupied by the specified table
    void destroyColumns();
    void destroyRows();

    //======================== Data Members ========================

    // Member to be set during initialization
    QString _name;
    size_t _totalCols;
    size_t _totalRows;                      // the dimensions of the table represented by this data structure
    const ProcessAssigner* _colAssigner;    // the distribution scheme for the columns
    const ProcessAssigner* _rowAssigner;    // the distribution scheme for the rows
    WriteState _writeOn;                    // determines the format for writing to the table before switching
                                            // and the format for reading after switching
    PeerToPeerCommunicator* _comm;
    Log* _log;

    // Flags representing various aspects of this ParallelTables's status
    bool _initialized;
    bool _distributed;                      // false if memory is not distributed
    bool _switched;                         // true after switchSchemes has been called. Disallows the writing operator
    bool _modified;                         // true if the table has been written to since initialization or resetting

    // Storage of the data
    Table<2> _columns;  // the values distributed over processes column wise
    Table<2> _rows;// the values distributed over processes row wise

    // Cached function outcomes for optimizing performance under frequent access
    std::vector<size_t> _relativeRowIndexv; // caches the values of _rowAssigner->relativeIndex(i)
    std::vector<size_t> _relativeColIndexv; // caches the values of _colAssigner->relativeindex(j)
    std::vector<bool> _isValidRowv; // caches the values of _rowAssigner->validIndex(i)
    std::vector<bool> _isValidColv; // caches the values of _colAssigner->validIndex(j)
};

////////////////////////////////////////////////////////////////////

#endif // PARALLELTABLE_HPP
