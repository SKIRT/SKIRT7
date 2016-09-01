/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "FatalError.hpp"
#include "ParallelDataCube.hpp"
#include "PeerToPeerCommunicator.hpp"
#include "ProcessAssigner.hpp"

////////////////////////////////////////////////////////////////////

ParallelDataCube::ParallelDataCube() : _wavelengthAssigner(0), _comm(0), _Nframep(0), _partialCube(new Array)
{
}

////////////////////////////////////////////////////////////////////

void ParallelDataCube::initialize(const ProcessAssigner *wavelengthAssigner, size_t Nframep)
{
    _wavelengthAssigner = wavelengthAssigner;
    _Nframep = Nframep;
    _Nlambda = _wavelengthAssigner->nvalues();

    _partialCube->resize(_Nlambda*_Nframep);

    _comm = _wavelengthAssigner->find<PeerToPeerCommunicator>();
}

////////////////////////////////////////////////////////////////////

std::shared_ptr<Array> ParallelDataCube::constructCompleteCube()
{
    if (!_wavelengthAssigner->parallel() || !_comm->isMultiProc()) // partial cube of equal size as total cube
    {
        _comm->sum(*_partialCube); // sum the data to root
        std::shared_ptr<Array> dummy (new Array(0));
        return _comm->isRoot() ? _partialCube : dummy;
        // give a handle to the summed cube at the root, and a dummy for the other processes
    }
    else // total cube is bigger than partial cube
    {
        std::shared_ptr<Array> completeCube(new Array(0));

        if (_comm->rank()==0)
            completeCube->resize(_wavelengthAssigner->total()*_Nframep);

        // displacements parameters for gatherw
        std::vector<std::vector<int>> displacements;
        displacements.reserve(_comm->size());
        for (int i=0; i<_comm->size(); i++) displacements.push_back(_wavelengthAssigner->indicesForRank(i));

        _comm->gatherw(&(*_partialCube)[0], _Nlambda*_Nframep, &(*completeCube)[0], 0, _Nframep, displacements);

        return completeCube;
    }
}

////////////////////////////////////////////////////////////////////

double& ParallelDataCube::operator()(int ell, int pixel)
{
    if (!_wavelengthAssigner->validIndex(ell)) throw FATALERROR("Wrong wavelength for this process!");

    return (*_partialCube)[_wavelengthAssigner->relativeIndex(ell)*_Nframep + pixel];
}

////////////////////////////////////////////////////////////////////

const double& ParallelDataCube::operator()(int ell, int pixel) const
{
    if (!_wavelengthAssigner->validIndex(ell)) throw FATALERROR("Wrong wavelength for this process!");

    return (*_partialCube)[_wavelengthAssigner->relativeIndex(ell)*_Nframep + pixel];
}

////////////////////////////////////////////////////////////////////
