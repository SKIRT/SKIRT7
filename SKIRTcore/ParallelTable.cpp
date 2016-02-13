#include "ParallelTable.hpp"
#include "FatalError.hpp"
#include "Log.hpp"
#include "PeerToPeerCommunicator.hpp"
#include "ProcessAssigner.hpp"
#include "TimeLogger.hpp"

ParallelTable::ParallelTable()
    : _name(""), _colAssigner(0), _rowAssigner(0), _dist(false), _synced(false), _initialized(false), _comm(0)
{
}

////////////////////////////////////////////////////////////////////

void ParallelTable::initialize(QString name, ProcessAssigner *colAssigner, ProcessAssigner *rowAssigner, writeState writeOn)
{
    _name = name;
    _colAssigner = colAssigner;
    _rowAssigner = rowAssigner;
    _writeOn = writeOn;
    _synced = true;
    _comm = colAssigner->find<PeerToPeerCommunicator>();

    int Nrows = _rowAssigner->total();
    int Ncols = _colAssigner->total();

    printf("initializing %s Table, sizes are (allrows:%d,%d) and (allcols:%d,%d)\n",
           _name.toStdString().c_str(), Nrows, _colAssigner->nvalues(), _rowAssigner->nvalues(), Ncols);

    // distributed memory
    if (colAssigner->parallel() && rowAssigner->parallel() && _comm->isMultiProc())
    {
        _dist = true;
        printf("ParallelTable is distributed.\n");
        _columns.resize(Nrows,colAssigner->nvalues());
        _rows.resize(rowAssigner->nvalues(),Ncols);
    }
    // not distributed
    else
    {
        _dist = false;

        if (writeOn == COLUMN)
            _columns.resize(Nrows,Ncols);
        else if (writeOn == ROW)
            _rows.resize(Nrows,Ncols);
        else
            throw FATALERROR("Invalid writeState for ParallelTable");
    }
    _initialized = true;
}

////////////////////////////////////////////////////////////////////

void ParallelTable::sync()
{
    if (!_synced)
    {
        Log* log = _colAssigner->find<Log>();
        TimeLogger logger(log->verbose() && _comm->isMultiProc() ? log : 0, "communication of " + _name);

        if (!_dist)
            sum_all();
        else if (_writeOn == COLUMN)
            col_to_row();
        else if (_writeOn == ROW)
            row_to_col();
    }
    _synced = true;
}

////////////////////////////////////////////////////////////////////

void ParallelTable::clear()
{
    _columns.clear();
    for (size_t i=0; i<_rows.size(0); i++) _rows[i] *= 0;

    _synced = true;
}

////////////////////////////////////////////////////////////////////

double ParallelTable::sumRow(size_t i) const
{
    if (!_synced) throw FATALERROR(_name + " says: sync() must be called before using summation functions");

    double sum = 0;
    if (_dist)
    {
        if (_rowAssigner->validIndex(i)) // we have the whole row
            return _rows[_rowAssigner->relativeIndex(i)].sum();
        else throw FATALERROR(_name + " says: sumRow(index) called on wrong index");
    }
    else // not distributed -> everything available, so straightforward
    {
        for (size_t j=0; j<_colAssigner->total(); j++)
            sum += (*this)(i,j);
        return sum;
    }
}

double ParallelTable::sumColumn(size_t j) const
{
    if (!_synced) throw FATALERROR(_name + " says: sync() must be called before using summation functions");

    double sum = 0;
    if (_dist)
    {
        if(_colAssigner->validIndex(j)) // we have the whole column
            for (size_t i=0; i<_columns.size(0); i++)
                sum += _columns(i,_colAssigner->relativeIndex(j));
        else throw FATALERROR(_name + " says: sumColumn(index) called on wrong index");
    }
    else // not distributed
    {
        for (size_t i=0; i<_rowAssigner->total(); i++)
            sum += (*this)(i,j);
    }
    return sum;
}

Array ParallelTable::stackColumns() const
{
    if (!_synced) throw FATALERROR(_name + " says: sync() must be called before using summation functions");

    Array result(_rowAssigner->total());

    if(_dist)
    {
        // fastest way: sum only the values in _rows, then do one big sum over the processes
        for (size_t iRel=0; iRel<_rows.size(0); iRel++)
            result[_rowAssigner->absoluteIndex(iRel)] = _rows[iRel].sum();
        _comm->sum_all(result);
    }
    else
    {
        for (size_t i=0; i<_rowAssigner->total(); i++)
            result[i] = sumRow(i);
    }
    return result;
}

Array ParallelTable::stackRows() const
{
    if (!_synced) throw FATALERROR(_name + " says: sync() must be called before using summation functions");

    Array result(_colAssigner->total());

    if(_dist)
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
        for (size_t j=0; j<_colAssigner->total(); j++)
            result[j] = sumColumn(j);
    }
    return result;
}

double ParallelTable::sumEverything() const
{
    if (!_synced) throw FATALERROR(_name + " says: sync() must be called before using summation functions");

    return stackColumns().sum();
}

////////////////////////////////////////////////////////////////////

const double& ParallelTable::operator()(size_t i, size_t j) const
{
    if (!_synced) throw FATALERROR(_name + " says: sync() must be called before using the read operator");

    if (_dist) // read from the table we are NOT writing on
    {
        if (_writeOn == COLUMN)    // read from _rows
            return getFromRows(i,j);
        else                            // read from _columns
            return getFromColumns(i,j);
    }
    else // read from the table that was resized in the constructor
    {
        if (_writeOn == COLUMN)
            return _columns(i,j);
        else
            return _rows(i,j);
    }
}

double& ParallelTable::operator()(size_t i, size_t j)
{
    _synced = false;

    if (_dist)
    {
        if (_writeOn == COLUMN)    // return writable reference to _rows
            return getFromColumns(i,j);
        else                            // return writable reference to _columns
            return getFromRows(i,j);
    }
    else
    {
        if (_writeOn == COLUMN)
            return _columns(i,j);
        else
            return _rows(i,j);
    }
}

////////////////////////////////////////////////////////////////////

Array& ParallelTable::operator[](size_t i)
{
    _synced = false;

    if (_writeOn == ROW) // return writable reference to a complete row
    {
        if (!_rowAssigner->validIndex(i))
            throw FATALERROR(_name + " says: Row of ParallelTable not available on this process");
        else return _rows[_rowAssigner->relativeIndex(i)];
    }
    else throw FATALERROR(_name + " says: This ParallelTable does not have writable rows.");
}

const Array& ParallelTable::operator[](size_t i) const
{
    if (!_synced) throw FATALERROR(_name + " says: sync() must be called before asking a read reference");

    if (_writeOn == COLUMN) // return read only reference to a complete row
    {
        if (!_rowAssigner->validIndex(i))
            throw FATALERROR(_name + " says: Row of ParallelTable not available on this process");
        else return _rows[_rowAssigner->relativeIndex(i)];
    }
    else throw FATALERROR(_name + " says: This ParallelTable does not have readable rows.");
}

////////////////////////////////////////////////////////////////////

const double& ParallelTable::read(size_t i, size_t j) const
{
    return (*this)(i,j);
}

double& ParallelTable::write(size_t i, size_t j)
{
    return (*this)(i,j);
}

////////////////////////////////////////////////////////////////////

const double& ParallelTable::getFromRows(size_t i, size_t j) const
{
    if (!_rowAssigner->validIndex(i))
        throw FATALERROR(_name + " says: Row of ParallelTable not available on this process");

    size_t iRel = _rowAssigner->relativeIndex(i);
    return _rows(iRel,j);
}

double& ParallelTable::getFromRows(size_t i, size_t j)
{
    if (!_rowAssigner->validIndex(i))
        throw FATALERROR(_name + " says: Row of ParallelTable not available on this process");

    size_t iRel = _rowAssigner->relativeIndex(i);
    return _rows(iRel,j);
}

////////////////////////////////////////////////////////////////////

const double& ParallelTable::getFromColumns(size_t i, size_t j) const
{
    if (!_colAssigner->validIndex(j))
        throw FATALERROR(_name + " says: Column of ParallelTable not available on this process");

    size_t jRel = _colAssigner->relativeIndex(j);
    return _columns(i,jRel);
}

double& ParallelTable::getFromColumns(size_t i, size_t j)
{
    if (!_colAssigner->validIndex(j))
        throw FATALERROR(_name + " says: Column of ParallelTable not available on this process");

    size_t jRel = _colAssigner->relativeIndex(j);
    return _columns(i,jRel);
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
        for (size_t i=0; i<_rows.size(0); i++)
        {
            Array& arr = _rows[i];
            _comm->sum_all(arr);
        }
    }
}

////////////////////////////////////////////////////////////////////

void ParallelTable::col_to_row()
{
    int totalRows = _columns.size(0);
    int totalCols = _rows.size(1);
    int thisRank = _comm->rank();

    int sendCount = 0;
    int N = 750;

    for (int i=0; i<totalRows; i++) // for each possible row of the big array (determines receiver)
    {
        int tgtRank = _rowAssigner->rankForIndex(i); // the rank where target has row i in it

        // post some receives
        for (int j=0; j<totalCols; j++) // for each possible column of the big array (determines sender)
        {
            int srcRank = _colAssigner->rankForIndex(j); // the rank where the source has col j in it
            int tag = i*totalCols + j; // unique for each position in the big array

            if (thisRank!= srcRank && thisRank == tgtRank) // receive if target needs row i
            {
                double& recvbuf = _rows(_rowAssigner->relativeIndex(i),j);
                _comm->receiveDouble(recvbuf,srcRank,tag);
            }
        }
        // post some sends
        for (int j=0; j<totalCols; j++) // for each possible column of the big array (determines sender)
        {
            int srcRank = _colAssigner->rankForIndex(j); // the rank where the source has col j in it
            int tag = i*totalCols + j; // unique for each position in the big array

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

void ParallelTable::row_to_col()
{
    int totalRows = _columns.size(0);
    int totalCols = _rows.size(1);
    int thisRank = _comm->rank();

    int sendCount = 0;
    int N = 750;

    for (int i=0; i<totalRows; i++) // for each row (determines sender)
    {
        int srcRank = _rowAssigner->rankForIndex(i);

        // post some receives
        for (int j=0; j<totalCols; j++) // for each column (determines receiver)
        {
            int tgtRank = _colAssigner->rankForIndex(j);
            int tag = i*totalCols + j;

            if (thisRank != srcRank && thisRank == tgtRank)
            {
                double& recvbuf = _columns(i,_colAssigner->relativeIndex(j));
                _comm->receiveDouble(recvbuf,srcRank,tag);
            }
        }
        // post some sends
        for (int j=0; j<totalCols; j++) // for each column (determines receiver)
        {
            int tgtRank = _colAssigner->rankForIndex(j);
            int tag = i*totalCols + j;

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

bool ParallelTable::distributed() const
{
    return _dist;
}

////////////////////////////////////////////////////////////////////

bool ParallelTable::initialized() const
{
    return _initialized;
}
