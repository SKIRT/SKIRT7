#include "ParallelTable.hpp"
#include "FatalError.hpp"
#include "PeerToPeerCommunicator.hpp"
#include "ProcessAssigner.hpp"
#include "TimeLogger.hpp"

namespace {
    typedef std::vector<std::vector<int>> IntTable;
}

////////////////////////////////////////////////////////////////////

ParallelTable::ParallelTable()
    : _totalCols(0), _totalRows(0), _colAssigner(nullptr), _rowAssigner(nullptr),
      _writeOn(WriteState::COLUMN), _comm(nullptr), _log(nullptr),
      _initialized(false), _distributed(false), _switched(false), _modified(false)
{
}

////////////////////////////////////////////////////////////////////

void ParallelTable::initialize(QString name, WriteState writeOn, const ProcessAssigner* colAssigner,
                               const ProcessAssigner* rowAssigner, PeerToPeerCommunicator* comm)
{
    _name = name;
    _colAssigner = colAssigner;
    _rowAssigner = rowAssigner;
    _writeOn = writeOn;
    _comm = comm;
    _log = _comm->find<Log>();
    _totalRows = _rowAssigner->total();
    _totalCols = _colAssigner->total();

    if (!_comm->dataParallel()) throw FATALERROR(_name + " says: The initialize function using assigners should not be"
                                                       + " called when the code runs non-dataparallel.");

    // Inform the user about the mode used and the dimensions (N,M) of the locally stored data
    _log->info(_name + " is distributed. Size of local table will switch between (" + QString::number(_totalRows) + ","
               + QString::number(_colAssigner->assigned()) + ") and (" + QString::number(_rowAssigner->assigned())
               + "," + QString::number(_totalCols) + ")");

    // Set the size of the relevant table
    if (_writeOn == WriteState::COLUMN) allocateColumns();
    else if (_writeOn == WriteState::ROW) allocateRows();
    else throw FATALERROR("Invalid writeState for ParallelTable");

    // Cache the outcomes of the following function calls
    _isValidRowv.resize(_totalRows, false);
    for (size_t i=0; i<_totalRows; i++) _isValidRowv[i] = _rowAssigner->validIndex(i);
    _isValidColv.resize(_totalCols, false);
    for (size_t j=0; j<_totalCols; j++) _isValidColv[j] = _colAssigner->validIndex(j);
    _relativeRowIndexv.resize(_totalRows, 0);
    for (size_t i=0; i<_totalRows; i++) _relativeRowIndexv[i] = _rowAssigner->relativeIndex(i);
    _relativeColIndexv.resize(_totalCols, 0);
    for (size_t j=0; j<_totalCols; j++) _relativeColIndexv[j] = _rowAssigner->relativeIndex(j);

    _initialized = true;
    _distributed = true;
}

////////////////////////////////////////////////////////////////////

void ParallelTable::initialize(QString name, WriteState writeOn, int columns, int rows, PeerToPeerCommunicator* comm)
{
    // Fill in some basic data members
    _name = name;
    _totalCols = columns;
    _totalRows = rows;
    _colAssigner = nullptr;
    _rowAssigner = nullptr;
    _writeOn = writeOn;
    _comm = comm;
    _log = _comm->find<Log>();

    // Report the allocated size
    _log->info(_name + " is not distributed. Size is (" + QString::number(_totalRows) + ","
               + QString::number(_totalCols) + ")");

    // Set the size of one of the tables. The other one will not be used.
    if (writeOn == WriteState::COLUMN) _columns.resize(_totalRows,_totalCols);
    else if (writeOn == WriteState::ROW) _rows.resize(_totalRows,_totalCols);
    else throw FATALERROR("Invalid writeState for ParallelTable");

    _initialized = true;
    _distributed = false;
}

////////////////////////////////////////////////////////////////////

bool ParallelTable::initialized() const
{
    return _initialized;
}

////////////////////////////////////////////////////////////////////

bool ParallelTable::distributed() const
{
    return _distributed;
}

////////////////////////////////////////////////////////////////////

void ParallelTable::switchScheme()
{
    // If any of the processes has modified its paralleltable, all of them need to know this to participate in this
    // global communication
    _comm->or_all(_modified);

    if (!_switched)
    {
        TimeLogger logger(_log->verbose() && _comm->isMultiProc() ? _log : 0, "communication of " + _name);

        if (!_distributed)
        {
            if (_modified) sum_all();
        }
        else if (_writeOn == WriteState::COLUMN)
        {
            allocateRows();
            if (_modified) columsToRows();
            destroyColumns();
        }
        else if (_writeOn == WriteState::ROW)
        {
            allocateColumns();
            if (_modified) rowsToColums();
            destroyRows();
        }
    }
    _switched = true;
    _modified = false;
}

////////////////////////////////////////////////////////////////////

void ParallelTable::reset()
{
    if (_distributed)
    {
        if (_writeOn == WriteState::COLUMN)
        {
            destroyRows();
            allocateColumns();
        }
        else
        {
            destroyColumns();
            allocateRows();
        }
    }
    else
    {
        _columns.clear();
        _rows.clear();
    }
    _switched = false;
    _modified = false;
}

////////////////////////////////////////////////////////////////////

double& ParallelTable::operator()(size_t i, size_t j)
{
    if (!_modified) _modified = true;

    // WORKING DISTRIBUTED: Writable reference to the table we _writeOn.
    if (_distributed)
    {
        if (_writeOn == WriteState::COLUMN) // Write on _columns
        {
            if (!_isValidColv[j])
                throw FATALERROR(_name + " says: Column of ParallelTable not available on this process");
            return _columns(i,_relativeColIndexv[j]);
        }
        else                    // Write on _rows
        {
            if (!_isValidRowv[i])
                throw FATALERROR(_name + " says: Row of ParallelTable not available on this process");
            return _rows(_relativeRowIndexv[i],j);
        }
    }
    // WORKING NON-DISTRIBUTED: Reading and writing happens in the same table.
    else
    {
        if (_writeOn == WriteState::COLUMN)
            return _columns(i,j);
        else
            return _rows(i,j);
    }
}

////////////////////////////////////////////////////////////////////

const double& ParallelTable::operator()(size_t i, size_t j) const
{
    if (!_switched) throw FATALERROR(_name + " says: switchScheme() must be called before using the read operator");

    // WORKING DISTRIBUTED: Read from the table opposite to the table we _writeOn.
    if (_distributed)
    {
        if (_writeOn == WriteState::COLUMN) // read from _rows
        {
            if (!_isValidRowv[i])
                throw FATALERROR(_name + " says: Row of ParallelTable not available on this process");
            return _rows(_relativeRowIndexv[i],j);
        }
        else                    // read from _columns
        {
            if (!_isValidColv[j])
                throw FATALERROR(_name + " says: Column of ParallelTable not available on this process");
            return _columns(i,_relativeColIndexv[j]);
        }
    }
    // WORKING NON-DISTRIBUTED: Reading and writing happens in the same table.
    else
    {
        if (_writeOn == WriteState::COLUMN)
            return _columns(i,j);
        else
            return _rows(i,j);
    }
}

////////////////////////////////////////////////////////////////////

double ParallelTable::sumColumn(size_t j) const
{
    if (!_switched) throw FATALERROR(_name + " says: switchScheme() must be called before using summation functions");
    if (_writeOn == WriteState::COLUMN) throw FATALERROR("Not available in COLUMN mode.");

    double sum = 0;
    if (_distributed)
    {
        if (_writeOn == WriteState::ROW) // read from columns
        {
            if(_isValidColv[j]) // we have the whole column
                for (size_t i=0; i<_totalRows; i++)
                    sum += _columns(i,_relativeColIndexv[j]);
            else throw FATALERROR("Column not available.");
        }
    }
    else // not distributed
    {
        for (size_t i=0; i<_totalRows; i++)
            sum += (*this)(i,j);
    }
    return sum;
}

////////////////////////////////////////////////////////////////////

double ParallelTable::sumRow(size_t i) const
{
    if (!_switched) throw FATALERROR(_name + " says: switchScheme() must be called before using summation functions");
    if (_writeOn == WriteState::ROW) throw FATALERROR("Not available in ROW mode.");

    double sum = 0;
    if (_distributed)
    {
        if (_writeOn == WriteState::COLUMN) // read from _rows
        {
            if (_isValidRowv[i]) // we have the whole row
                for (size_t j=0; j<_totalCols; j++)
                    sum += _rows(_relativeRowIndexv[i],j);
            else throw FATALERROR("Row not available.");
        }
    }
    else // not distributed -> everything available, so straightforward
    {
        for (size_t j=0; j<_totalCols; j++)
            sum += (*this)(i,j);
    }
    return sum;
}

////////////////////////////////////////////////////////////////////

Array ParallelTable::stackColumns() const
{
    if (!_switched) throw FATALERROR(_name + " says: switchScheme() must be called before using summation functions");

    Array result(_totalRows);

    if (_distributed)
    {
        if (_writeOn == WriteState::COLUMN) // use rows to get a part of the stacked column
        {
            for (size_t iRel=0; iRel<_rows.size(0); iRel++)
            {
                size_t i = _rowAssigner->absoluteIndex(iRel);
                for (size_t j=0; j<_totalCols; j++)
                    result[i] += _rows(iRel,j);
            }
        }
        else // sum the local columns
        {
            for (size_t i=0; i<_totalRows; i++)
                for (size_t jRel=0; jRel<_columns.size(1); jRel++)
                    result[i] += _columns(i,jRel);
        }
        _comm->sum_all(result);
    }
    else
    {
        for (size_t i=0; i<_totalRows; i++)
            result[i] = sumRow(i);
    }
    return result;
}

////////////////////////////////////////////////////////////////////

Array ParallelTable::stackRows() const
{
    if (!_switched) throw FATALERROR(_name + " says: switchScheme() must be called before using summation functions");

    Array result(_totalCols);

    if (_distributed)
    {
        if (_writeOn == WriteState::COLUMN) // sum the local rows
        {
            for (size_t j=0; j<_totalCols; j++)
                for (size_t iRel=0; iRel<_rows.size(0); iRel++)
                    result[j] += _rows(iRel,j);
        }
        else // use columns to get a part of the stacked row
        {
            for (size_t jRel=0; jRel<_columns.size(1); jRel++)
            {
                size_t j = _colAssigner->absoluteIndex(jRel);
                for (size_t i=0; i<_totalRows; i++)
                    result[j] += _columns(i,jRel);
            }
        }
        _comm->sum_all(result);
    }
    else
    {
        for (size_t j=0; j<_totalCols; j++)
            result[j] = sumColumn(j);
    }
    return result;
}

////////////////////////////////////////////////////////////////////

double ParallelTable::sumEverything() const
{
    if (!_switched) throw FATALERROR(_name + " says: switchScheme() must be called before using summation functions");

    return stackColumns().sum();
}

////////////////////////////////////////////////////////////////////

void ParallelTable::sum_all()
{
    if (_writeOn == WriteState::COLUMN)
    {
        Array& arr = _columns.getArray();
        _comm->sum_all(arr);
    }
    else
    {
        Array& arr = _rows.getArray();
        _comm->sum_all(arr);
    }
}

////////////////////////////////////////////////////////////////////

void ParallelTable::columsToRows()
{
    int Nprocs = _comm->size();

    // All the partial rows from _columns are sent in one block per receiving process. Therefore, the sendcount is 1.
    // The rows to send are indicated by the absolute row-indices assigned to each process. As each block in the send
    // pattern will represent a single partial row, the blocklength is equal to the width of _columns.
    double* sendBuffer = &_columns(0,0);
    IntTable sendDispvv;
    sendDispvv.reserve(Nprocs);
    for (int r=0; r<Nprocs; r++) sendDispvv.push_back(_rowAssigner->indicesForRank(r));
    int sendBlockLength = _columns.size(1);
    int sendExtent = _totalRows*sendBlockLength;

    // All the complete rows stored at a process will be filled up by repeating the pattern used to fill a single one.
    // The pattern consists of doubles displaced over a distance of their absolute column index. To ensure that the
    // receive patterns are repeated correctly, the extent of each pattern should be equal to the length of a complete
    // row, or the width of _rows.
    double* recvBuffer = &_rows(0,0);
    IntTable recvDispvv;
    recvDispvv.reserve(Nprocs);
    for (int r=0; r<Nprocs; r++) recvDispvv.push_back(_colAssigner->indicesForRank(r));
    int recvCount = _rows.size(0);
    int recvExtent = _totalCols;

    _comm->displacedBlocksAllToAll(sendBuffer, 1, sendDispvv, sendBlockLength, sendExtent,
                                   recvBuffer, recvCount, recvDispvv, 1, recvExtent);
}

////////////////////////////////////////////////////////////////////

void ParallelTable::rowsToColums()
{
    int Nprocs = _comm->size();

    double* sendBuffer = &_rows(0,0);
    IntTable sendDispvv;
    sendDispvv.reserve(Nprocs);
    for (int r=0; r<Nprocs; r++) sendDispvv.push_back(_colAssigner->indicesForRank(r));
    int sendCount = _rows.size(0);
    int sendExtent = _totalCols;

    double* recvBuffer = &_columns(0,0);
    IntTable recvDispvv;
    recvDispvv.reserve(Nprocs);
    for (int r=0; r<Nprocs; r++) recvDispvv.push_back(_rowAssigner->indicesForRank(r));
    int recvBlockLength = _columns.size(1);
    int recvExtent = _totalRows*recvBlockLength;

    _comm->displacedBlocksAllToAll(sendBuffer, sendCount, sendDispvv, 1, sendExtent,
                                   recvBuffer, 1, recvDispvv, recvBlockLength, recvExtent);
}

////////////////////////////////////////////////////////////////////

void ParallelTable::allocateColumns()
{
    _columns.resize(_totalRows, _colAssigner->assigned());
}

////////////////////////////////////////////////////////////////////

void ParallelTable::allocateRows()
{
    _rows.resize(_rowAssigner->assigned(),_totalCols);
}

////////////////////////////////////////////////////////////////////

void ParallelTable::destroyColumns()
{
    _columns.resize(0,0);
}

////////////////////////////////////////////////////////////////////

void ParallelTable::destroyRows()
{
    _rows.resize(0,0);
}

////////////////////////////////////////////////////////////////////
