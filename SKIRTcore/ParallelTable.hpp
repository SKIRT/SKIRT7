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

    void initialize(QString name, const ProcessAssigner* colAssigner, const ProcessAssigner* rowAssigner, writeState writeOn);

    //======================== Other Functions =======================

    // Reading and writing operators
    const double& operator()(size_t i, size_t j) const; // read operator
    double& operator()(size_t i, size_t j); // write operator
    Array& operator[](size_t i); // write row
    const Array& operator[](size_t i) const; // read row

    // Basic operations
    void sync(); // communicates between processes to synchronize _colDist with _rowDist or vice versa
    void clear(); // reset contents to zeros

    // Different kinds of summations
    double sumRow(size_t i) const; // sum of the values in a row
    double sumColumn(size_t j) const; // sum of the values in a column
    Array stackColumns() const; // sum of al the columns: each element is a row sum
    Array stackRows() const; // sum of al the rows: each elements is a column sum
    double sumEverything() const;

    bool distributed() const;
    bool initialized() const;

private:
    // Private methods
    void sum_all();
    void col_to_row();
    void row_to_col();
    void experimental_col_to_row();
    void experimental_row_to_col();

    //======================== Data Members ========================

    QString _name;
    const ProcessAssigner* _colAssigner;  // the distribution scheme for the columns
    const ProcessAssigner* _rowAssigner;  // the distribution scheme for the rows
    writeState _writeOn;            // determines which table will be writable, and which one will be readable

    bool _distributed;     // false if memory is not distributed
    bool _synced;   // true if the writable table has not changed since the last sync
    bool _initialized;

    int _totalRows;
    int _totalCols;

    Table<2> _columns;  // the values distributed over processes column wise
    ArrayTable<2> _rows;// the values distributed over processes row wise

    PeerToPeerCommunicator* _comm; // communicator used for synchronizing
    Log* _log;

    std::vector<std::vector<int>> _displacementvv;  // A list of absolute indices for each process.
                                                    // This way the receival positions in the receivebuffer _rows[i] can
                                                    // be dependent on the sending process.
                                                    // An array of MPI_Datatype's will be constructed using the array
                                                    // of displacements.
};

////////////////////////////////////////////////////////////////////

#endif // PARALLELTABLE_HPP
