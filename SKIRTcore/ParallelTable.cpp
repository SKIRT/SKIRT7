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

    printf("initializing %s Table, sizes are (allrows:%d,%d) and (allcols:%d,%d\n",
           _name.toStdString().c_str(), Nrows, _colAssigner->nvalues(), _rowAssigner->nvalues(), Ncols);

    // distributed memory
    if (colAssigner->parallel() && rowAssigner->parallel() && _comm->isMultiProc())
    {
        _dist = true;

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
        if(_writeOn == COLUMN)
            return getFromColumns(i,j);
        else
            return getFromRows(i,j);
    }
}

double& ParallelTable::operator()(size_t i, size_t j)
{
    _synced = false;

    if (_writeOn == COLUMN)    // return writable reference to _rows
        return getFromColumns(i,j);
    else                            // return writable reference to _columns
        return getFromRows(i,j);
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

    for (int j=0; j<totalCols; j++) // for each possible column of the big array
        for (int i=0; i<totalRows; i++) // for each possible row of the big array
        {
            int tgtRank = _rowAssigner->rankForIndex(i); // the rank where target has row i in it
            int srcRank = _colAssigner->rankForIndex(j); // the rank where the source has col j in it
            int tag = i*totalCols + j; // unique for each position in the big array
            if (thisRank == srcRank) // if this process has column j, do send
            {
                double& sendbuf = _columns(i,_colAssigner->relativeIndex(j));
                // copy it if my target has row i, else send it
                if (thisRank == tgtRank) _rows(_rowAssigner->relativeIndex(i), j) = sendbuf;
                else _comm->sendDouble(sendbuf,tgtRank,tag);
            }
            else if (thisRank == tgtRank) // receive if target needs i
            {
                double& recvbuf = _rows(_rowAssigner->relativeIndex(i),j);
                _comm->receiveDouble(recvbuf,srcRank,tag);
            }
        }
    _comm->wait("syncing " + _name);
}

////////////////////////////////////////////////////////////////////

void ParallelTable::row_to_col()
{
    int totalRows = _columns.size(0);
    int totalCols = _rows.size(1);
    int thisRank = _comm->rank();

    for (int j=0; j<totalCols; j++)
        for (int i=0; i<totalRows; i++)
        {
            int tgtRank = _colAssigner->rankForIndex(j);
            int srcRank = _rowAssigner->rankForIndex(i);
            int tag = i*totalCols + j;
            if (thisRank == srcRank)
            {
                double& sendbuf = _rows(_rowAssigner->relativeIndex(i),j);
                if (thisRank == tgtRank) _columns(i,_colAssigner->relativeIndex(j)) = sendbuf;
                else _comm->sendDouble(sendbuf,tgtRank,tag);
            }
            else if (thisRank == tgtRank)
            {
                double& recvbuf = _columns(i,_colAssigner->relativeIndex(j));
                _comm->receiveDouble(recvbuf,srcRank,tag);
            }
        }
    _comm->wait("syncing ParallelTable");
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
