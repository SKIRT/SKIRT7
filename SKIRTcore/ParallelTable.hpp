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

class ParallelTable
{
public:
    enum class WriteState { COLUMN, ROW };

    //============= Construction - Setup - Destruction =============

    ParallelTable();

    void initialize(QString name, WriteState writeOn, const ProcessAssigner* colAssigner,
                    const ProcessAssigner* rowAssigner, PeerToPeerCommunicator* comm);

    void initialize(QString name, WriteState writeOn, int columns, int rows, PeerToPeerCommunicator* comm);

    //======================== Other Functions =======================

    bool initialized() const;
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
