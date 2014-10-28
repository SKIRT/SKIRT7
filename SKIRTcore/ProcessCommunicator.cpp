/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "ProcessCommunicator.hpp"
#include "ProcessManager.hpp"

ProcessCommunicator::ProcessCommunicator()
    : _rank(-1), _Nprocs(-1)
{
    ProcessManager::acquireMPI(_rank,_Nprocs);
}

ProcessCommunicator::~ProcessCommunicator()
{
    ProcessManager::releaseMPI();
}

int ProcessCommunicator::getRank() const
{
    return _rank;
}

int ProcessCommunicator::getSize() const
{
    return _Nprocs;
}

bool ProcessCommunicator::isMultiProc() const
{
    return (_Nprocs > 1);
}
