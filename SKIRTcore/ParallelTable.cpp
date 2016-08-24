#include "ParallelTable.hpp"
#include "FatalError.hpp"
#include "PeerToPeerCommunicator.hpp"
#include "ProcessAssigner.hpp"
#include "TimeLogger.hpp"

ParallelTable::ParallelTable()
    : _name(""), _colAssigner(0), _rowAssigner(0), _distributed(false), _synced(false), _initialized(false), _comm(0),
      _log(0)
{
}

////////////////////////////////////////////////////////////////////

void ParallelTable::initialize(QString name, const ProcessAssigner *colAssigner, const ProcessAssigner *rowAssigner,
                               writeState writeOn)
{
    _name = name;
    _colAssigner = colAssigner;
    _rowAssigner = rowAssigner;
    _writeOn = writeOn;

    _comm = colAssigner->find<PeerToPeerCommunicator>();
    _log = colAssigner->find<Log>();

    _totalRows = _rowAssigner->total();
    _totalCols = _colAssigner->total();

    _isValidRowv.resize(_totalRows, false);
    _isValidColv.resize(_totalCols, false);
    for (size_t i=0; i<_totalRows; i++) _isValidRowv[i] = _rowAssigner->validIndex(i);
    for (size_t j=0; j<_totalCols; j++) _isValidColv[j] = _colAssigner->validIndex(j);

    // Use the distributed memory scheme
    if (colAssigner->parallel() && rowAssigner->parallel() && _comm->isMultiProc())
    {
        _log->info(
                   _name + " is distributed. Sizes of local tables are ("
                   + QString::number(_totalRows) + "," + QString::number(_colAssigner->nvalues())
                   + ") and ("
                   + QString::number(_rowAssigner->nvalues()) + "," + QString::number(_totalCols)
                   + ")"
                   );

        _distributed = true;
        _columns.resize(_totalRows,colAssigner->nvalues());
        _rows.resize(rowAssigner->nvalues(),_totalCols);

        int Nprocs = _comm->size();
        _displacementvv.reserve(Nprocs);
        for (int r=0; r<Nprocs; r++) _displacementvv.push_back(_colAssigner->indicesForRank(r));

        _relativeRowIndexv.resize(_totalRows, 0);
        _relativeColIndexv.resize(_totalCols, 0);
        for (size_t i=0; i<_totalRows; i++) _relativeRowIndexv[i] = _rowAssigner->relativeIndex(i);
        for (size_t j=0; j<_totalCols; j++) _relativeColIndexv[j] = _rowAssigner->relativeIndex(j);
    }
    // not distributed
    else
    {
        _log->info(_name + " is not distributed. Size is ("
                    + QString::number(_totalRows) + "," + QString::number(_totalCols) + ")"
                   );

        _distributed = false;
        if (writeOn == COLUMN) _columns.resize(_totalRows,_totalCols);
        else if (writeOn == ROW) _rows.resize(_totalRows,_totalCols);
        else throw FATALERROR("Invalid writeState for ParallelTable");
    }
    _initialized = true;
    _synced = true;
}

////////////////////////////////////////////////////////////////////

const double& ParallelTable::operator()(size_t i, size_t j) const
{
    if (!_synced) throw FATALERROR(_name + " says: sync() must be called before using the read operator");

    // WORKING DISTRIBUTED: Read from the table opposite to the table we _writeOn.
    if (_distributed)
    {
        if (_writeOn == COLUMN) // read from _rows
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
        if (_writeOn == COLUMN)
            return _columns(i,j);
        else
            return _rows(i,j);
    }
}

////////////////////////////////////////////////////////////////////

double& ParallelTable::operator()(size_t i, size_t j)
{
    if (_synced) _synced = false;

    // WORKING DISTRIBUTED: Writable reference to the table we _writeOn.
    if (_distributed)
    {
        if (_writeOn == COLUMN) // Write on _columns
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
        if (_writeOn == COLUMN)
            return _columns(i,j);
        else
            return _rows(i,j);
    }
}

////////////////////////////////////////////////////////////////////
/*
Array& ParallelTable::operator[](_displacementvvsize_t i)
{
    if (_synced) _synced = false;

    if (_writeOn == ROW) // return writable reference to a complete row
    {
        if (!_isValidRowv[i])
            throw FATALERROR(_name + " says: Row of ParallelTable not available on this process");
        else return _rows[_distributed ? _relativeRowIndexv[i] : i];
    }
    else throw FATALERROR(_name + " says: This ParallelTable does not have writable rows.");
}

////////////////////////////////////////////////////////////////////

const Array& ParallelTable::operator[](size_t i) const
{
    if (!_synced) throw FATALERROR(_name + " says: sync() must be called before asking a read reference");

    if (_distributed && _writeOn == COLUMN) // return read only reference to a complete row
    {
        if (!_isValidRowv[i])
            throw FATALERROR(_name + " says: Row of ParallelTable not available on this process");
        else return _rows[_relativeRowIndexv[i]];
    }
    else throw FATALERROR(_name + " says: This ParallelTable does not have readable rows.");
}
*/
////////////////////////////////////////////////////////////////////

void ParallelTable::sync()
{
    if (_comm->rank() == 0)
    for (int row = 0; row < 10; row++)
    {
       for (int column = 0; column < 16; column++)
           printf(" %1.1e ", _columns(row,column));

       printf("\n");
    }
    // Get a global value for the synced flag, in case one of the processes never wrote something
    _comm->and_all(_synced);
    if (!_synced)
    {
        TimeLogger logger(_log->verbose() && _comm->isMultiProc() ? _log : 0, "communication of " + _name);

        if (!_distributed) sum_all();
        else if (_writeOn == COLUMN) allAtOnceCtR();
        else if (_writeOn == ROW) row_to_col();
    }
    _synced = true;

    if (_comm->rank() == 0)
    for (int row = 0; row < 10; row++)
    {
       for (int column = 0; column < 32; column++)
           printf(" %1.1e ", _rows(row,column));

       printf("\n");
    }
}

////////////////////////////////////////////////////////////////////

void ParallelTable::clear()
{
    _columns.clear();
    //for (size_t i=0; i<_rows.size(0); i++) _rows[i] *= 0;
    _rows.clear();
    _synced = true;
}

////////////////////////////////////////////////////////////////////

double ParallelTable::sumRow(size_t i) const
{
    if (!_synced) throw FATALERROR(_name + " says: sync() must be called before using summation functions");

    double sum = 0;
    if (_distributed)
    {
        if (_isValidRowv[i]) // we have the whole row
            //return _rows[_relativeRowIndexv[i]].sum();
            for (size_t j=0; j<_totalCols; j++)
                sum += _rows(_relativeRowIndexv[i],j);
        else throw FATALERROR(_name + " says: sumRow(index) called on wrong index");
    }
    else // not distributed -> everything available, so straightforward
    {
        for (size_t j=0; j<_totalCols; j++)
            sum += (*this)(i,j);
    }
    return sum;
}

////////////////////////////////////////////////////////////////////

double ParallelTable::sumColumn(size_t j) const
{
    if (!_synced) throw FATALERROR(_name + " says: sync() must be called before using summation functions");

    double sum = 0;
    if (_distributed)
    {
        if(_isValidColv[j]) // we have the whole column
            for (size_t i=0; i<_totalRows; i++)
                sum += _columns(i,_relativeColIndexv[j]);
        else throw FATALERROR(_name + " says: sumColumn(index) called on wrong index");
    }
    else // not distributed
    {
        for (size_t i=0; i<_totalRows; i++)
            sum += (*this)(i,j);
    }
    return sum;
}

////////////////////////////////////////////////////////////////////

Array ParallelTable::stackColumns() const
{
    if (!_synced) throw FATALERROR(_name + " says: sync() must be called before using summation functions");

    Array result(_totalRows);

    if(_distributed)
    {
        // fastest way: sum only the values in _rows, then do one big sum over the processes
        for (size_t iRel=0; iRel<_rows.size(0); iRel++)
            for (size_t j=0; j<_totalCols; j++)
                result[_rowAssigner->absoluteIndex(iRel)] += _rows(iRel,j);
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
    if (!_synced) throw FATALERROR(_name + " says: sync() must be called before using summation functions");

    Array result(_totalCols);

    if(_distributed)
    {
        for (size_t jRel=0; jRel<_columns.size(1); jRel++)
        {
            size_t j = _colAssigner->absoluteIndex(jRel);
            for (size_t i=0; i<_columns.size(0); i++)
                result[j] += _columns(i,jRel);
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
    if (!_synced) throw FATALERROR(_name + " says: sync() must be called before using summation functions");

    return stackColumns().sum();
}

////////////////////////////////////////////////////////////////////

bool ParallelTable::distributed() const
{
    return _distributed;
}

////////////////////////////////////////////////////////////////////

bool ParallelTable::initialized() const
{
    return _initialized;
}

////////////////////////////////////////////////////////////////////

void ParallelTable::sum_all()
{
    if (_writeOn == COLUMN)
    {
        Array& arr = _columns.getArray();
        _comm->sum_all(arr);
    }
    else
    {
        /*
        for (int i=0; i<_totalRows; i++)
        {
            Array& arr = _rows[i];
            _comm->sum_all(arr);
        }
        */
        Array& arr = _rows.getArray();
        _comm->sum_all(arr);
    }
}

////////////////////////////////////////////////////////////////////

// To be removed
void ParallelTable::onebyone_col_to_row()
{
    int thisRank = _comm->rank();

    int sendCount = 0;
    int N = 750;

    for (int i=0; i<_totalRows; i++) // for each possible row of the big array (determines receiver)
    {
        int tgtRank = _rowAssigner->rankForIndex(i); // the rank where target has row i in it

        // post some receives
        for (int j=0; j<_totalCols; j++) // for each possible column of the big array (determines sender)
        {
            int srcRank = _colAssigner->rankForIndex(j); // the rank where the source has col j in it
            int tag = i*_totalCols + j; // unique for each position in the big array

            if (thisRank!= srcRank && thisRank == tgtRank) // receive if target needs row i
            {
                double& recvbuf = _rows(_rowAssigner->relativeIndex(i),j);
                _comm->receiveDouble(recvbuf,srcRank,tag);
            }
        }
        // post some sends
        for (int j=0; j<_totalCols; j++) // for each possible column of the big array (determines sender)
        {
            int srcRank = _colAssigner->rankForIndex(j); // the rank where the source has col j in it
            int tag = i*_totalCols + j; // unique for each position in the big array

            if (thisRank == srcRank) // if this process has column j, do send
            {
                double& sendbuf = _columns(i,_colAssigner->relativeIndex(j));
                if (thisRank == tgtRank) _rows(_rowAssigner->relativeIndex(i), j) = sendbuf;
                else _comm->sendDouble(sendbuf,tgtRank,tag);
            }
            sendCount++;
        }
        // clear the request vector every N
        if (sendCount > N)
        {
            _comm->finishRequests();
            sendCount = 0;
        }
    }
    _comm->finishRequests();
}

////////////////////////////////////////////////////////////////////

// To be removed
void ParallelTable::onebyone_row_to_col()
{
    int thisRank = _comm->rank();

    int sendCount = 0;
    int N = 750;

    for (int i=0; i<_totalRows; i++) // for each row (determines sender)
    {
        int srcRank = _rowAssigner->rankForIndex(i);

        // post some receives
        for (int j=0; j<_totalCols; j++) // for each column (determines receiver)
        {
            int tgtRank = _colAssigner->rankForIndex(j);
            int tag = i*_totalCols + j;

            if (thisRank != srcRank && thisRank == tgtRank)
            {
                double& recvbuf = _columns(i,_colAssigner->relativeIndex(j));
                _comm->receiveDouble(recvbuf,srcRank,tag);
            }
        }
        // post some sends
        for (int j=0; j<_totalCols; j++) // for each column (determines receiver)
        {
            int tgtRank = _colAssigner->rankForIndex(j);
            int tag = i*_totalCols + j;

            if (thisRank == srcRank)
            {
                double& sendbuf = _rows(_rowAssigner->relativeIndex(i),j);
                if (thisRank == tgtRank) _columns(i,_colAssigner->relativeIndex(j)) = sendbuf;
                else _comm->sendDouble(sendbuf,tgtRank,tag);
            }
            sendCount++;
        }
        // clear the request vector every N
        if (sendCount > N)
        {
            _comm->finishRequests();
            sendCount = 0;
        }
    }
    _comm->finishRequests();
}

////////////////////////////////////////////////////////////////////

void ParallelTable::col_to_row()
{
    // prepare to receive using doubles according to the given displacements for each process
    _comm->presetConfigure(1,_displacementvv);

    // for each row
    for (size_t i=0; i<_totalRows; i++)
    {
        double* sendBuffer = &_columns(i,0);
        double* recvBuffer = _isValidRowv[i] ? &_rows(_relativeRowIndexv[i],0) : 0;
        int recvRank = _rowAssigner->rankForIndex(i);

        _comm->presetGatherw(sendBuffer, _columns.size(1), recvBuffer, recvRank);

        //_comm->gatherw(sendBuffer, _columns.size(1), recvBuffer, recvRank, 1, _displacementvv);
        // Elk proces i zendt vanuit sendBuffer, _columns.size(1) doubles naar recvBuffer op recvRank in de vorm
        // van datatype i bepaald door blocksize 1 en _displacementvv[i].
    }
    // free the memory taken by the datatypes
    _comm->presetClear();
}

////////////////////////////////////////////////////////////////////

void ParallelTable::row_to_col()
{
    // prepare to send using doubles according to the given displacements for each process
    _comm->presetConfigure(1,_displacementvv);

    for (size_t i=0; i<_totalRows; i++)
    {
        double* sendBuffer = _isValidRowv[i] ? &_rows(_relativeRowIndexv[i],0) : 0;
        double* recvBuffer = &_columns(i,0);
        int sendRank = _rowAssigner->rankForIndex(i);

        _comm->presetScatterw(sendBuffer, sendRank, recvBuffer, _columns.size(1));

        //_comm->scatterw(sendBuffer, sendRank, 1, _displacementvv, recvBuffer, _columns.size(1));
        // Proces sendRank zendt vanuit sendBuffer een aantal doubles in de vorm van datatype i bepaald door
        // _displacementvv[i] naar recvBuffer op rank i.
    }
    _comm->presetClear();
}

////////////////////////////////////////////////////////////////////

void ParallelTable::allAtOnceCtR()
{

    int Nprocs = _comm->size();

    double* sendBuffer = &_columns(0,0);

    IntTable sendDispvv;
    sendDispvv.reserve(Nprocs);
    for (int r=0; r<Nprocs; r++)
    {
        sendDispvv.push_back(_rowAssigner->indicesForRank(r));
    }

    int sendBlockLength = _columns.size(1);
    int sendExtent = _totalRows*sendBlockLength;

    double* recvBuffer = &_rows(0,0);
    int recvCount = _rows.size(0);
    int recvExtent = _totalCols;

    _comm->displacedBlocksAllToAll(sendBuffer, 1, sendDispvv, sendBlockLength, sendExtent,
                                   recvBuffer, recvCount, _displacementvv, 1, recvExtent);
}
