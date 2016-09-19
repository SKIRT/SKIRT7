/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "FatalError.hpp"
#include "ParallelDataCube.hpp"
#include "PeerToPeerCommunicator.hpp"
#include "ProcessAssigner.hpp"
#include "WavelengthGrid.hpp"

////////////////////////////////////////////////////////////////////

ParallelDataCube::ParallelDataCube() : _wavelengthAssigner(nullptr), _comm(nullptr), _Nframep(0),
    _partialCube(new Array)
{
}

////////////////////////////////////////////////////////////////////

void ParallelDataCube::initialize(size_t Nframep, SimulationItem* item)
{
    _Nframep = Nframep;
    _comm = item->find<PeerToPeerCommunicator>();

    WavelengthGrid* wg = item->find<WavelengthGrid>();

    if(_comm->dataParallel())
    {
        _wavelengthAssigner = wg->assigner();
        _Nlambda = _wavelengthAssigner->assigned();
    }
    else
    {
        _wavelengthAssigner = nullptr;
        _Nlambda = wg->Nlambda();
    }
    _partialCube->resize(_Nlambda*_Nframep);
}

////////////////////////////////////////////////////////////////////

std::shared_ptr<Array> ParallelDataCube::constructCompleteCube()
{
    if (!_wavelengthAssigner || !_comm->isMultiProc()) // partial cube of equal size as total cube
    {
        _comm->sum(*_partialCube); // sum the data to root
        std::shared_ptr<Array> dummy (new Array(0));
        return _comm->isRoot() ? _partialCube : dummy;
        // give a handle to the summed cube at the root, and a dummy for the other processes
    }
    else // total cube is bigger than partial cube
    {
        std::shared_ptr<Array> completeCube(new Array(0));

        if (_comm->isRoot())
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
    if(!_wavelengthAssigner)
        return (*_partialCube)[ell*_Nframep + pixel];
    else
    {
        if (!_wavelengthAssigner->validIndex(ell))
            throw FATALERROR("Wrong wavelength for this process!");

        return (*_partialCube)[_wavelengthAssigner->relativeIndex(ell)*_Nframep + pixel];
    }
}

////////////////////////////////////////////////////////////////////

const double& ParallelDataCube::operator()(int ell, int pixel) const
{
    if(!_wavelengthAssigner) return (*_partialCube)[ell*_Nframep + pixel];
    else
    {
        if (!_wavelengthAssigner->validIndex(ell))
            throw FATALERROR("Wrong wavelength for this process!");

        return (*_partialCube)[_wavelengthAssigner->relativeIndex(ell)*_Nframep + pixel];
    }
}

////////////////////////////////////////////////////////////////////
