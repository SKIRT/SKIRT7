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
    stored at the calling process, and only the available index combinations can be called.

    One of the most important features of this class, is that it can switch between two ways of
    parallelized data storage. The idea is that a 2D block of data can be cut either vertically or
    horizontally, letting each process store a set of column or rows respectively. When a certain
    operation works on the data in a column-wise manner for example, one could opt to only store
    the relevant columns. It is often the case however, that after a certain column-wise
    calculation, another calculation needs this same data in a row format. For such situations, an
    operation needs to be perfomed where all processes exchange data to achieve this switch of
    storage formats. This kind of operation is often called a \em global \em transposition. \c
    ParallelTable implements the storing and exchanging of data in two ways: The parallel program
    either fills in the data in a column format, and then transposes it to a row format, or it
    performs this operation the other way round. At initialization, one of these two operating
    modes has to be chosen using an enum argument (see \c enum \c class \c WriteState).

    Deciding which columns (or rows) need to be stored on a certain process, happens through the \c
    ProcessAssigner class. Because a \c ProcessAssigner can also be used to decide the work
    distribution between processes, using a common assigner for both work and data parallelization
    will ensure that the two are consistent. Any index conversions that need to happen to access
    the data, can be performed by calling one of the indexing function on a \c ProcessAssigner
    object. Thus the freedom for dividing the data is essentially limited by the number of \c
    ProcessAssigner implementations.

    The switching from a column-based to a row-based parallel storage scheme or vice versa is
    implemented with a call to the alltoallw function of MPI. This is a very general communication
    function, where every process can be made to send a certain set of data to every other process
    (see \c switchScheme()).

    This class can also store data in a regular way, when data-parallelization is not desired. This
    way, the code can run with or without data parallelization using the same data structure. This
    mode is called the \em non-distributed mode, as opposed to the \em distributed mode used for
    data-parallelization.

    The figure below illustrates the operation of a ParallelTable instance.

    \image html paralleltable_elementacces.png "Indexing of ParallelTable"

    The block at the top left shows the availability of elements. The arrows show how the
    absolute index can be converted to a relative one to find the correct element of the stored
    data. The red elements are available for writing before switching, the yellow ones are
    available after switching.

    The workflow for using a ParallelTable usually comes down to the following steps:
    - Create a ParallelTable object through the default constructor.
    - Initialize the ParallelTable in the desired mode, choosing to start with either the column
      based or the row based scheme.
    - Perform some calculation in parallel, where each process writes some data to the ParallelTable.
    - Call \c switchScheme()
    - Perform another calculation, where each process reads the data from the ParallelTable.
    - Call \c reset() if the table is to be re-used with the same parameters.

    When the two assigners divide the columns and the rows evenly between the processes, the memory
    usage per process of a \c ParallelTable is expected to scale as 1/N, with N the number of
    processes. During the switch, the memory usage temporarily becomes 2/N. */
class ParallelTable
{
public:
    /** This enum is used to indicate the mode that should be used. An argument of this type should
        be given at initialization and is stored by the paralleltable for the rest of its lifetime.
        The meaning of the two available values can be summarized as follows:
        - \em COLUMNS: Write in a column format -> communicate -> read in a row format
        - \em ROW: Write in a row format -> communicate -> read in a column format

        The chosen mode hence determines how the data is stored before and after performing the
        global transposition, defining a fixed direction for the flow of data. If one wants to
        switch back to the initial format, the \c reset() function can be used. This clears all
        data however, and hence it is made sure that data/information can only flow in the chosen
        direction. */
    enum class WriteState { COLUMN, ROW };

    //============= Construction - Setup - Destruction =============

    /** The default constructor creates an uninitialized ParallelTable object. Before it can be
        used, one of the \c initialize() functions needs to be called. */
    ParallelTable();

    /** This function prepares the ParallelTable for use. This version of \c initialize should be
        used if one wants to activate the distributed storage of data (a.k.a data parallelization).
        As arguments it takes:
            - A name, which will be used in log messages.
            - A \c WriteState argument, determining the mode of operation, see the description
            of \c WriteState.
            - Two \c ProcessAssigner pointers. The first one indicates which columns should be
            stored by each process, and the second one does the same for the rows. When the
            execution of the work and the storage of the corresponding data are based on the same
            assigner, consistency between work division and data distribution can be easily
            achieved.
            - All MPI operations will be performed through the given communicator.

        The table derives its dimensions from the properties of the given assigners, and allocates
        the necessary memory. After this, the table is ready for writing to it. A ParallelTable can
        only be initialized once. If this function is called a second time, an error will be
        thrown. */
    void initialize(QString name, WriteState writeOn, const ProcessAssigner* colAssigner,
                    const ProcessAssigner* rowAssigner, PeerToPeerCommunicator* comm);

    /** This version of \c initialize sets up the ParallelTable to operate without data
        parallelization. The dimensions are simply given as arguments. A WriteState argument is
        still required is still required for consistency reasons. The communicator is used to sum
        the contents of the table across all processes when performing the scheme switch. A
        ParallelTable can only be initialized once. If this function is called a second time, an
        error will be thrown. */
    void initialize(QString name, WriteState writeOn, int columns, int rows, PeerToPeerCommunicator* comm);

    //======================== Other Functions =======================

    /** This function indicates whether the ParallelTable has been initialized. */
    bool initialized() const;

    /** This function indicates whether the data is distributed across the processes or not. */
    bool distributed() const;

    /** This function switches the the data storage from a scheme where each process holds a set of
        columns to a scheme where each process holds a set of rows (for \c COLUMN mode), or vice
        versa (for \c ROW mode). This happens trough an MPI communication. First, a new table is
        allocated, which will serve as the destination for the end result of the data exchange.
        Then, a call to one of three private communication functions is made, depending on the mode
        of operation. After the communication, the table that originally contained the data is
        deleted, as it will be no longer used. This also means that during the communication, this
        ParallelTable consumes roughly twice the memory. Before calling \c switchScheme(), only
        the writing operator can be used. After the call, only reading is allowed. ParallelTable
        also keeps a flag (\c _modified) that indicates whether the write operator has already been
        callled. If it has not been called yet, this means that the data contained consists of
        zeros, and hence no communication is necessary. The re-allocation of the memory is still
        performed though, to make sure that the dimensions of the data remain consistent. When the
        table is running in non-distributed mode, the functionality of \c switchScheme() changes:
        as all processes have access to all columns and rows, the contained data will be summed
        over all processes to make it consistent. No re-allocations will happen in that case. In
        all cases, this function needs to be called collectively. */
    void switchScheme();

    /** This function resets the ParallelTable, reverting it to the same state as if it was just
        initialized, and setting all data to zero. After resetting, only the writing operator can
        be called. */
    void reset();

    /** The non-const ()-operator returns a writable reference to an element of the ParallelTable,
        and will hence be called the writing operator. This operator should only be called \em
        before the invocation of \c switchScheme(). In \c COLUMN mode, this operator assumes that
        all elements of the columns indicated by the column assigner are available. Because only a
        subset of the columns is stored in this case, it is first checked if the specified column
        \c j is actually available. If this is not the case, an error is thrown. Therefore, the
        client should make sure that this operator is only called for \c j that are available, as
        indicated by the same column assigner that was given to the ParallelTable at
        initialization. If the \c j 'th column is available, the column assigner can be used to
        convert this absolute index to a relative one, allowing the implementation easily find the
        correct element of data that is actually stored by this process. This operator functions
        analogously for the \c ROW mode, where only certain rows will be available. In
        non-distributed mode, this operator simply returns the requested element. */
    double& operator()(size_t i, size_t j);

    /** The const version of the ()-operator returns a read-only reference to an element of the
        ParallelTable and is therefore called the read operator. This operator can only be called
        \em after the invocation of \c switchScheme(). In \c COLUMN mode, this operator asserts
        that all elements of the rows indicated by the row assigner are available. This means that
        an error will be thrown if the \c switched flag is false. For every call, it is checked if
        the specified row \c i is stored by the calling process. If this is not the case, another
        potential error is thrown. The client can make sure that only the available \c i are used
        by making use of the same assigner that was given the the ParallelTable as its row
        assigner. When called on an available absolute row index \c i, the row index is converted
        to a relative index to find the right element of the data stored at this process. This
        operator functions analogously for the \c ROW mode, where only certain columns will be
        available, after performing \c switchScheme(). */
    const double& operator()(size_t i, size_t j) const;

    /** This function returns the sum of all elements of a certain column. In distributed mode,
        this column needs to be available at the calling process. The column index is checked and
        converted by the column assigner, and an error can be thrown when a column is not
        available. This function can only be called when the contained data is consistent between
        all processes. Therefore, \c switchScheme() should be called before invoking this function.
        Because this function needs all data from a certain column after switching schemes, it is
        only available in \c ROW mode. */
    double sumColumn(size_t j) const;

    /** This function does the same as \c sumColumn(), but for a certain row. Because it needs all
        data from from a certain row after performing a scheme switch, it is only available in \c
        COLUMN mode. */
    double sumRow(size_t i) const;

    /** This function calculates the sum of all the columns contained by all the processes. It is
        important to know that this is a \em collective function, meaning that all processes need
        to call it together. This is necessary because a collective MPI communication needs to
        happen if all the columns are to be stacked. This function can be used in any mode, but
        only after \c switchScheme() has been called to ensure consistency of the result. */
    Array stackColumns() const;

    /** This function calculatese the sum of all the rows contained by all the processes. Functions
        analogously to \c stackColumns(). Needs to be called collectively. */
    Array stackRows() const;

    /** This function calculates the sum of all the elements in the ParallelTable. Needs to be
        called collectively. */
    double sumEverything() const;

private:
    /** Private function to sum the contained data over all processes, used during the
        communication step in non-distributed mode. */
    void sum_all();

    /** Performs a communication between all processes to switch from a column based to a row based
        data distribution scheme. Both data members that serve for storage (\c _columns and \c
        _rows) need to have their correct size when this function is called. */
    void columsToRows();

    /** The inverse operation of \c columnsToRows(). */
    void rowsToColums();

    /** Sets _columns to its correct size. Its height is set to encompass an entire column of the
        ParallelTable, while its width is set to the number of columns stored at this process. */
    void allocateColumns();

    /** Sets _rows to its correct size. Its height is set to the number of rows stored at this
        process, while its width is set to encompass an entire row of the ParallelTable. */
    void allocateRows();

    /** Frees the memory occupied by _columns by resizing it to 0. */
    void destroyColumns();

    /** Frees the memory occupied by _rows by resizing it to 0. */
    void destroyRows();

    //======================== Data Members ========================

    // Members to be set during initialization
    QString _name;
    size_t _totalCols;
    size_t _totalRows;                      // the dimensions of the table represented by this data structure
    const ProcessAssigner* _colAssigner;    // the distribution scheme for the columns
    const ProcessAssigner* _rowAssigner;    // the distribution scheme for the rows
    WriteState _writeOn;                    // the mode, either COLUMN or ROW
    PeerToPeerCommunicator* _comm;
    Log* _log;

    // Flags representing various aspects of this ParallelTables's status
    bool _initialized;
    bool _distributed;  // false if memory is not distributed
    bool _switched;     // true after switchScheme has been called, disallows the writing operator
    bool _modified;     // true if the table has been written to since initialization or resetting

    // Storage of the data
    Table<2> _columns;  // the values distributed over processes column wise
    Table<2> _rows;     // the values distributed over processes row wise

    // Cached function outcomes for optimizing performance under frequent access
    std::vector<size_t> _relativeRowIndexv; // caches the values of _rowAssigner->relativeIndex(i)
    std::vector<size_t> _relativeColIndexv; // caches the values of _colAssigner->relativeindex(j)
    std::vector<bool> _isValidRowv;         // caches the values of _rowAssigner->validIndex(i)
    std::vector<bool> _isValidColv;         // caches the values of _colAssigner->validIndex(j)
};

////////////////////////////////////////////////////////////////////

#endif // PARALLELTABLE_HPP
