#include "DistMemTable.hpp"
#include "FatalError.hpp"
#include "Log.hpp"
#include "PeerToPeerCommunicator.hpp"
#include "ProcessAssigner.hpp"
#include "TimeLogger.hpp"

DistMemTable::DistMemTable()
    : _name(""), _colAssigner(0), _rowAssigner(0), _dist(false), _synced(false), _initialized(false), _comm(0)
{
}

////////////////////////////////////////////////////////////////////

void DistMemTable::initialize(QString name, ProcessAssigner *colAssigner, ProcessAssigner *rowAssigner, writeState writeOn)
{
    _name = name;
    _colAssigner = colAssigner;
    _rowAssigner = rowAssigner;
    _writeOn = writeOn;
    _synced = true;
    _comm = colAssigner->find<PeerToPeerCommunicator>();

    int rows = _rowAssigner->total();
    int cols = _colAssigner->total();

    printf("initializing %s Table, sizes are (allrows:%d,%d) and (allcols:%d,%d)\n",_name.toStdString().c_str(),
           rows,_colAssigner->nvalues(),
           _rowAssigner->nvalues(),cols);

    // distributed memory
    if (colAssigner->parallel() && rowAssigner->parallel() && _comm->isMultiProc()) {
        _dist = true;
        _colDist.resize(rows,colAssigner->nvalues());
        _rowDist.resize(rowAssigner->nvalues(),cols);
        printf("(made dist)\n");
    }
    // not distributed
    else
    {
        _dist = false;
        _notDist.resize(rows,cols);
        printf("(made notdist)\n");
    }
    _initialized = true;
}

////////////////////////////////////////////////////////////////////

void DistMemTable::sync()
{
    Log* log = _colAssigner->find<Log>();
    TimeLogger logger(log->verbose() && _comm->isMultiProc() ? log : 0, "communication of " + _name);
    if(!_synced)
    {
        if (!_dist) sum_all_notDist();
        else if (_writeOn == COLUMN) col_to_row();
        else if (_writeOn == ROW) row_to_col();
    }
    _synced = true;
}

////////////////////////////////////////////////////////////////////

void DistMemTable::clear()
{
    if (_dist)
    {
        _colDist.clear();
        for (size_t i=0; i<_rowDist.size(0); i++) _rowDist[i] *= 0;
    }
    else for (size_t i=0; i<_notDist.size(0); i++) _notDist[i] *= 0;
    _synced = true;
}

////////////////////////////////////////////////////////////////////

const double& DistMemTable::operator()(size_t i, size_t j) const
{
    if (!_synced) throw FATALERROR("DistMemTable::sync() must be called before using the read operator");

    if (!_dist) return _notDist(i,j);
    else if (_writeOn == COLUMN) // read from row representation
        return fetchRowDist(i,j);
    else // if (_writeOn == ROW) read from column representation
        return fetchColDist(i,j);
}

double& DistMemTable::operator()(size_t i, size_t j)
{
    _synced = false;

    if (!_dist) return _notDist(i,j);
    else if (_writeOn == COLUMN) // return writable reference from row representation
        return fetchColDist(i,j);
    else // if (_writeOn == ROW) return writable reference from column representation
        return fetchRowDist(i,j);
}

////////////////////////////////////////////////////////////////////

Array& DistMemTable::operator[](size_t i)
{
    _synced = false;

    if (!_dist) return _notDist[i];
    else if (_writeOn == ROW) // return writable reference to a row
    {
        if (!_rowAssigner->validIndex(i))
            throw FATALERROR("Row of DistMemTable not available on this process");
        else return _rowDist[_rowAssigner->relativeIndex(i)];
    }
    else throw FATALERROR("DistMemTable row representation not writable");
}

const Array& DistMemTable::operator[](size_t i) const
{
    if (!_synced) throw FATALERROR("DistMemTable::sync() must be called before asking a read only reference");

    if (!_dist) return _notDist[i];
    else if(_writeOn == COLUMN) // return read only reference to a row
    {
        if (!_rowAssigner->validIndex(i))
            throw FATALERROR("Row of DistMemTable not available on this process");
        else return _rowDist[_rowAssigner->relativeIndex(i)];
    }
    else throw FATALERROR("DistMemTable row representation not readable");
}

////////////////////////////////////////////////////////////////////

const double& DistMemTable::fetchRowDist(size_t i, size_t j) const
{
    if (!_rowAssigner->validIndex(i))
        throw FATALERROR("Row of DistMemTable not available on this process");
    else
    {
        size_t iRel = _rowAssigner->relativeIndex(i);
        return _rowDist(iRel,j);
    }
}

const double& DistMemTable::fetchColDist(size_t i, size_t j) const
{
    if (!_colAssigner->validIndex(j))
        throw FATALERROR("Column of DistMemTable not available on this process");
    else
    {
        size_t jRel = _colAssigner->relativeIndex(j);
        return _colDist(i,jRel);
    }
}

double& DistMemTable::fetchRowDist(size_t i, size_t j)
{
    if (!_rowAssigner->validIndex(i))
        throw FATALERROR("Row of DistMemTable not available on this process");
    else
    {
        size_t iRel = _rowAssigner->relativeIndex(i);
        return _rowDist(iRel,j);
    }
}

double& DistMemTable::fetchColDist(size_t i, size_t j)
{
    if (!_colAssigner->validIndex(j))
        throw FATALERROR("Column of DistMemTable not available on this process");
    else
    {
        size_t jRel = _colAssigner->relativeIndex(j);
        return _colDist(i,jRel);
    }
}

////////////////////////////////////////////////////////////////////

void DistMemTable::sum_all_notDist()
{
    for (size_t i=0; i<_notDist.size(0); i++)
    {
        Array& arr = _notDist[i];
        _comm->sum_all(arr);
    }
}

////////////////////////////////////////////////////////////////////

void DistMemTable::col_to_row()
{
    int totalRows = _colDist.size(0);
    int totalCols = _rowDist.size(1);
    int thisRank = _comm->rank();

    for (int j=0; j<totalCols; j++) // for each possible column of the big array
        for (int i=0; i<totalRows; i++) // for each possible row of the big array
        {
            int tgtRank = _rowAssigner->rankForIndex(i); // the rank where target has row i in it
            int srcRank = _colAssigner->rankForIndex(j); // the rank where the source has col j in it
            int tag = i*totalCols + j; // unique for each position in the big array
            if (thisRank==srcRank) // if this process has column j, do send
            {
                double& sendbuf = _colDist(i,_colAssigner->relativeIndex(j));
                // copy it if my target has row i, else send it
                if (thisRank==tgtRank) _rowDist(_rowAssigner->relativeIndex(i), j) = sendbuf;
                else _comm->sendDouble(sendbuf,tgtRank,tag);
            }
            else if (thisRank==tgtRank) // receive if target needs i
            {
                double& recvbuf = _rowDist(_rowAssigner->relativeIndex(i),j);
                _comm->receiveDouble(recvbuf,srcRank,tag);
            }
        }
    _comm->wait("syncing " + _name);
}

////////////////////////////////////////////////////////////////////

void DistMemTable::row_to_col()
{
    int totalRows = _colDist.size(0);
    int totalCols = _rowDist.size(1);
    int thisRank = _comm->rank();

    for (int j=0; j<totalCols; j++)
        for (int i=0; i<totalRows; i++)
        {
            int tgtRank = _colAssigner->rankForIndex(j);
            int srcRank = _rowAssigner->rankForIndex(i);
            int tag = i*totalCols + j;
            if (thisRank==srcRank)
            {
                double& sendbuf = _rowDist(_rowAssigner->relativeIndex(i),j);
                if (thisRank==tgtRank) _colDist(i,_colAssigner->relativeIndex(j)) = sendbuf;
                else _comm->sendDouble(sendbuf,tgtRank,tag);
            }
            else if (thisRank==tgtRank)
            {
                double& recvbuf = _colDist(i,_colAssigner->relativeIndex(j));
                _comm->receiveDouble(recvbuf,srcRank,tag);
            }
        }
    _comm->wait("syncing DistMemTable");
}

////////////////////////////////////////////////////////////////////

bool DistMemTable::distributed()
{
    return _dist;
}

////////////////////////////////////////////////////////////////////

bool DistMemTable::initialized()
{
    return _initialized;
}
