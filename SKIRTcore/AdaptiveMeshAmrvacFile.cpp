/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include "AdaptiveMeshAmrvacFile.hpp"
#include "FatalError.hpp"
#include "FilePaths.hpp"
#include "Log.hpp"

////////////////////////////////////////////////////////////////////

AdaptiveMeshAmrvacFile::AdaptiveMeshAmrvacFile()
{
    _nxlone[0] = _nxlone[1] = _nxlone[2] = 1;
}

////////////////////////////////////////////////////////////////////

void AdaptiveMeshAmrvacFile::setLevelOneX(int value)
{
    _nxlone[0] = value;
}

////////////////////////////////////////////////////////////////////

int AdaptiveMeshAmrvacFile::levelOneX() const
{
    return _nxlone[0];
}

////////////////////////////////////////////////////////////////////

void AdaptiveMeshAmrvacFile::setLevelOneY(int value)
{
    _nxlone[1] = value;
}

////////////////////////////////////////////////////////////////////

int AdaptiveMeshAmrvacFile::levelOneY() const
{
    return _nxlone[1];
}

////////////////////////////////////////////////////////////////////

void AdaptiveMeshAmrvacFile::setLevelOneZ(int value)
{
    _nxlone[2] = value;
}

////////////////////////////////////////////////////////////////////

int AdaptiveMeshAmrvacFile::levelOneZ() const
{
    return _nxlone[2];
}

//////////////////////////////////////////////////////////////////////

namespace
{
    const int INT_SIZE = sizeof(qint32);
    const int DOUBLE_SIZE = sizeof(double);

    // read a 4-byte integer from the data stream
    // !!! this function assumes that the incoming data has native byte order !!!
    int readInt(QIODevice& in)
    {
        union { qint32 number; char bytes[4]; } result;
        int n = in.read(result.bytes, INT_SIZE);
        if (n!=INT_SIZE) throw FATALERROR("File error while reading integer value");
        return result.number;
    }

    // state constants (zero or positive values indicate cell index in current block)
    const int STATE_BEFORE_DATA = -1;
    const int STATE_TOPLEVEL_NONLEAF = -2;
    const int STATE_REFINEMENT_NONLEAF = -3;
    const int STATE_BLOCK_NONLEAF = -4;
    const int STATE_AFTER_DATA = -5;
}

//////////////////////////////////////////////////////////////////////

void AdaptiveMeshAmrvacFile::open()
{
    // open the data file
    QString filepath = find<FilePaths>()->input(_filename);
    _infile.setFileName(filepath);
    if (!_infile.open(QIODevice::ReadOnly))
        throw FATALERROR("Could not open the adaptive mesh data file " + filepath);
    if (_infile.isSequential())
        throw FATALERROR("No random access to file " + filepath);
    find<Log>()->info("Reading adaptive mesh data from MPI-AMRVAC file " + filepath + "...");

    // read the parameters at the end of the file (starting at EOF minus 7 integers and 1 double)
    qint64 eof = _infile.size();
    _infile.seek(eof - 7*INT_SIZE - 1*DOUBLE_SIZE);
    _nblocks = readInt(_infile);    // number of active tree leafs 'nleafs' (= #blocks)
               readInt(_infile);    // maximal refinement level present 'levmax'
    _ndims   = readInt(_infile);    // dimensionality 'NDIM'
               readInt(_infile);    // number of vector components 'NDIR'
    _nvars   = readInt(_infile);    // number of variables 'nw'
    int pars = readInt(_infile);    // number of equation-specific variables 'neqpar+nspecialpar'

    // read the block size in each dimension (before equation-specific variables)
    _infile.seek(eof - 7*INT_SIZE - 1*DOUBLE_SIZE - _ndims*INT_SIZE - pars*DOUBLE_SIZE);
    _nx[0] = _nx[1] = _nx[2] = 1;   // provide default of one for missing dimensions
    for (int i=0; i<_ndims; i++) _nx[i] = readInt(_infile);

    // calculate some handy grid characteristics:
    //    number of blocks at the coursest level
    for (int i=0; i<3; i++)
    {
        if (_nxlone[i]%_nx[i])
            throw FATALERROR("Number of cells at the coursest level is not a multiple of block size");
        _ng[i] = _nxlone[i]/_nx[i];
    }
    //    refinement factor: always 2 except for missing dimensions
    _nr[0] = _nr[1] = _nr[2] = 1;
    for (int i=0; i<_ndims; i++) _nr[i] = 2;
    //    number of cells in a block
    _ncells = _nx[0]*_nx[1]*_nx[2];
    //    block size in bytes
    _blocksize = _ncells*_nvars*DOUBLE_SIZE;

    // read the forest representing the grid structure (just after the data blocks)
    // there are are exactly _nblocks "true" values in the forest, but there can be many additional "false" values
    _infile.seek(static_cast<qint64>(_nblocks)*static_cast<qint64>(_blocksize));
    _forest.clear();
    _forest.reserve(_nblocks);
    for (int i=0; i<_nblocks; i++)
    {
        while (true)
        {
            bool leaf = readInt(_infile) ? true : false;
            _forest.push_back(leaf);
            if (leaf) break;
        }
    }
    _forestsize = _forest.size();

    // set the context at the beginning of the file with no current record
    _infile.seek(0);
    _state = STATE_BEFORE_DATA;
    _block.resize(_blocksize);
    _forestindex = 0;
}

//////////////////////////////////////////////////////////////////////

void AdaptiveMeshAmrvacFile::close()
{
    _infile.close();
    _forest.clear();
    _block.clear();
    _state = STATE_AFTER_DATA;
}

//////////////////////////////////////////////////////////////////////

bool AdaptiveMeshAmrvacFile::read()
{
    if (_state == STATE_BEFORE_DATA)
    {
        _state = STATE_TOPLEVEL_NONLEAF;
        return true;
    }
    if (_state == STATE_TOPLEVEL_NONLEAF || _state == STATE_REFINEMENT_NONLEAF)
    {
        if (_forestindex < _forestsize)
        {
            _state = _forest[_forestindex++] ? STATE_BLOCK_NONLEAF : STATE_REFINEMENT_NONLEAF;
            return true;
        }
        else
        {
            _state = STATE_AFTER_DATA;
            return false;
        }
    }
    if (_state == STATE_BLOCK_NONLEAF)
    {
        // !!! this statement assumes that the incoming data has native byte order !!!
        int n = _infile.read((char*)&_block[0], _blocksize);
        if (n!=_blocksize) throw FATALERROR("File error while reading cell data");
        _state = 0;
        return true;
    }
    if (_state >= 0)
    {
        _state++;
        if (_state >= _ncells)
        {
            if (_forestindex < _forestsize)
            {
                _state = _forest[_forestindex++] ? STATE_BLOCK_NONLEAF : STATE_REFINEMENT_NONLEAF;
                return true;
            }
            else
            {
                _state = STATE_AFTER_DATA;
                return false;
            }
        }
        return true;
    }
    return false;
}

//////////////////////////////////////////////////////////////////////

bool AdaptiveMeshAmrvacFile::isNonLeaf() const
{
    return _state < 0;
}

//////////////////////////////////////////////////////////////////////

void AdaptiveMeshAmrvacFile::numChildNodes(int &nx, int &ny, int &nz) const
{
    if (_state == STATE_TOPLEVEL_NONLEAF)
    {
        nx = _ng[0]; ny = _ng[1]; nz = _ng[2];
    }
    else if (_state == STATE_REFINEMENT_NONLEAF)
    {
        nx = _nr[0]; ny = _nr[1]; nz = _nr[2];
    }
    else if (_state == STATE_BLOCK_NONLEAF)
    {
        nx = _nx[0]; ny = _nx[1]; nz = _nx[2];
    }

    // we expect three positive integers
    if (nx<1 || ny<1 || nz<1) throw FATALERROR("Invalid nonleaf information in mesh data");
}

//////////////////////////////////////////////////////////////////////

double AdaptiveMeshAmrvacFile::value(int g) const
{
    // verify index range
    if (g < 0) throw FATALERROR("Field index out of range");
    if (g >= _nvars) throw FATALERROR("Insufficient number of field values in mesh data");
    if (_state < 0) throw FATALERROR("Invocation of value function for nonleaf node");

    return _block[g*_ncells+_state];
}

//////////////////////////////////////////////////////////////////////
