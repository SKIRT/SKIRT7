#ifndef DISTMEMTABLE_HPP
#define DISTMEMTABLE_HPP

#include <QString>

#include "ArrayTable.hpp"
#include "Table.hpp"

class ProcessAssigner;
class PeerToPeerCommunicator;

enum writeState { COLUMN, ROW };

class ParallelTable
{
public:
    // Constructor
    ParallelTable();

    // Initialization
    void initialize(QString name, ProcessAssigner* colAssigner, ProcessAssigner* rowAssigner, writeState writeOn);

    // Reading and writing operators
    const double& operator()(size_t i, size_t j) const; // read operator
    double& operator()(size_t i, size_t j); // write operator
    Array& operator[](size_t i); // write column
    const Array& operator[](size_t i) const; // read column

    // Basic operations
    void sync(); // communicates between processes to synchronize _colDist with _rowDist or vice versa
    void clear(); // reset contents to zeros

    // Different kinds of summations
    double sumRow(size_t i) const; // sum of the values in a row
    double sumColumn(size_t j) const; // sum of the values in a column
    Array stackColumns() const; // sum of al the columns: each element is a row sum
    Array stackRows() const; // sum of al the rows: each elements is a column sum
    double sumEverything() const;

    // Some extra stuff
    const double& read(size_t i, size_t j) const;
    double& write(size_t i, size_t j);
    bool distributed() const;
    bool initialized() const;

private:
    // Private methods

    // Read only references (called if member function is const)
    const double& getFromRows(size_t i, size_t j) const;
    const double& getFromColumns(size_t i, size_t j) const;
    // Writable refenences (called if member function is not const)
    double& getFromRows(size_t i, size_t j);
    double& getFromColumns(size_t i, size_t j);

    void sum_all();
    void col_to_row();
    void row_to_col();

    // Members
    QString _name;
    ProcessAssigner* _colAssigner;  // the distribution scheme for the columns
    ProcessAssigner* _rowAssigner;  // the distribution scheme for the rows
    writeState _writeOn;            // determines which table will be writable, and which one will be readable

    bool _dist;     // false if memory is not distributed
    bool _synced;   // true if the writable table has not changed since the last sync
    bool _initialized;

    int _totalRows;
    int _totalCols;

    Table<2> _columns;  // the values distributed over processes column wise
    ArrayTable<2> _rows;// the values distributed over processes row wise

    PeerToPeerCommunicator* _comm; // communicator used for synchronizing

    // For experimental functions
    std::vector<std::vector<int>> _displacementvv;  // A list of absolute indices for each process.
                                                    // This way the receival positions in the receivebuffer _rows[i] can
                                                    // be dependent on the sending process.
                                                    // An array of MPI_Datatype's will be constructed using the array
                                                    // of displacements.

    void experimental_col_to_row();

};

#endif // DISTMEMTABLE_HPP
