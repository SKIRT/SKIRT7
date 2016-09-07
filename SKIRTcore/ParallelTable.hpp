/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef DISTMEMTABLE_HPP
#define DISTMEMTABLE_HPP

#include <QString>

#include "ArrayTable.hpp"
#include "Log.hpp"
#include "Table.hpp"
class ProcessAssigner;
class PeerToPeerCommunicator;

////////////////////////////////////////////////////////////////////

enum writeState { COLUMN, ROW };

////////////////////////////////////////////////////////////////////

class ParallelTable
{
    //============= Construction - Setup - Destruction =============

public:
    ParallelTable();

    void initialize(QString name, writeState writeOn, const ProcessAssigner* colAssigner,
                    const ProcessAssigner* rowAssigner, PeerToPeerCommunicator* comm);

    void initialize(QString name, writeState writeOn, int columns, int rows, PeerToPeerCommunicator* comm);

    //======================== Other Functions =======================

    // Reading and writing operators
    const double& operator()(size_t i, size_t j) const; // read operator
    double& operator()(size_t i, size_t j); // write operator

    void switchScheme(); // Change from a column based write mode to a row based read-only mode, or vice versa.
    void reset(); // Switch back to the writing mode, and set any contents to zero.

    // Summation functions without communication. Only work for currently available rows/columns.
    double sumRow(size_t i) const; // sum of the values in a row
    double sumColumn(size_t j) const; // sum of the values in a column

    // Summation functions including communication. This communication is collective, hence all processes need to call
    // the same function
    Array stackColumns() const; // sum of al the columns: each element is a row sum
    Array stackRows() const; // sum of al the rows: each elements is a column sum
    double sumEverything() const;

    bool distributed() const;
    bool initialized() const;

private:
    // Private methods
    void sum_all();
    void columsToRows();
    void rowsToColums();

    void allocateColumns();
    void destroyColumns();
    void allocateRows();
    void destroyRows();

    //======================== Data Members ========================

    QString _name;
    const ProcessAssigner* _colAssigner;    // the distribution scheme for the columns
    const ProcessAssigner* _rowAssigner;    // the distribution scheme for the rows
    writeState _writeOn;    // determines the format for writing to the table before switching
                            // and the format for reading after switching

    bool _distributed;  // false if memory is not distributed
    bool _modified; // true if the table has been written to since initialization or resetting
    bool _initialized;
    bool _switched; // true after switchSchemes has been called. Disallows the writing operator.

    // the dimensions of the table represented by this data structure
    size_t _totalRows;
    size_t _totalCols;

    Table<2> _columns;  // the values distributed over processes column wise
    Table<2> _rows;// the values distributed over processes row wise

    PeerToPeerCommunicator* _comm;
    Log* _log;

    std::vector<size_t> _relativeRowIndexv; // caches the values of _rowAssigner->relativeIndex(i)
    std::vector<size_t> _relativeColIndexv; // caches the values of _colAssigner->relativeindex(j)
    std::vector<bool> _isValidRowv; // caches the values of _rowAssigner->validIndex(i)
    std::vector<bool> _isValidColv; // caches the values of _colAssigner->validIndex(j)
};

////////////////////////////////////////////////////////////////////

#endif // PARALLELTABLE_HPP
