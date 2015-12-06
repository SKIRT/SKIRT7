#ifndef DISTMEMTABLE_HPP
#define DISTMEMTABLE_HPP

#include <QString>

#include "ArrayTable.hpp"
#include "Table.hpp"


class ProcessAssigner;
class PeerToPeerCommunicator;

enum writeState { COLUMN, ROW };

class DistMemTable
{
public:
    // Constructor
    DistMemTable();

    void initialize(QString name, ProcessAssigner* colAssigner, ProcessAssigner* rowAssigner, writeState writeOn);

    // Public methods
    const double& operator()(size_t i, size_t j) const; // read operator
    double& operator()(size_t i, size_t j); // write operator
    Array& operator[](size_t i);
    const Array& operator[](size_t i) const;

    const double& read(size_t i, size_t j) const;

    void sync(); // communicates between processes to sync _colDist with _rowDist or vice versa
    void clear(); // reset contents to zeros

    bool distributed() const;
    bool initialized() const;

private:
    // Private methods

    // Read only references (called if member function is const)
    const double& fetchRowDist(size_t i, size_t j) const;
    const double& fetchColDist(size_t i, size_t j) const;
    // Writable refenences (called if member function is not const)
    double& fetchRowDist(size_t i, size_t j);
    double& fetchColDist(size_t i, size_t j);

    void sum_all_notDist();
    void col_to_row();
    void row_to_col();

    // Members
    QString _name;
    ProcessAssigner* _colAssigner; // the distribution scheme for the columns
    ProcessAssigner* _rowAssigner; // the distribution scheme for the rows
    writeState _writeOn; // determines which table will be writable, and which one will be readable

    bool _dist; // false if memory is not distributed
    bool _synced; // true if the writable table has not changed since the last sync
    bool _initialized;

    Table<2> _colDist; // the values distributed over processes column wise
    ArrayTable<2> _rowDist; // the values distributed over processes row wise
    ArrayTable<2> _notDist; // will be used for both reading and writing if not _dist

    PeerToPeerCommunicator* _comm; // communicator used for synchronizing
};

#endif // DISTMEMTABLE_HPP
