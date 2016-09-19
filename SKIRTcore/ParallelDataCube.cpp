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
    _partialCube(std::make_shared<Array>())
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
    // partial cube of equal size as total cube
    if (!_wavelengthAssigner || !_comm->isMultiProc())
    {
        // sum the data to root
        _comm->sum(*_partialCube);

        // give a handle to the summed cube at the root, and a dummy for the other processes
        return _comm->isRoot() ? _partialCube : std::make_shared<Array>();
    }
    // total cube is bigger than partial cube
    else
    {
        // allocate space for complete cube at root process
        auto completeCube = std::make_shared<Array>();
        if (_comm->isRoot()) completeCube->resize(_wavelengthAssigner->total()*_Nframep);

        // displacements parameters for gatherw
        std::vector<std::vector<int>> displacements;
        displacements.reserve(_comm->size());
        for (int i=0; i<_comm->size(); i++) displacements.push_back(_wavelengthAssigner->indicesForRank(i));

        // gather complete cube
        _comm->gatherw(&(*_partialCube)[0], _Nlambda*_Nframep, &(*completeCube)[0], 0, _Nframep, displacements);
        return completeCube;
    }
}

////////////////////////////////////////////////////////////////////

double& ParallelDataCube::operator()(int ell, int pixel)
{
    if (!_wavelengthAssigner)
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
