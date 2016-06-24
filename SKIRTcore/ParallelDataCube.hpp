/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef PARALLELDATACUBE_HPP
#define PARALLELDATACUBE_HPP

#include <memory>

#include "Array.hpp"
#include "PeerToPeerCommunicator.hpp"
class ProcessAssigner;

////////////////////////////////////////////////////////////////////

class ParallelDataCube
{
    //============= Construction - Setup - Destruction =============

public:
    ParallelDataCube();

    void initialize(const ProcessAssigner* wavelengthAssigner, size_t Nframep);

    //======================== Other Methods =======================

    std::shared_ptr<Array> constructCompleteCube();

    double& operator()(int ell, int pixel);
    const double& operator()(int ell, int pixel) const;

    //======================== Data Members ========================

private:
    const ProcessAssigner* _wavelengthAssigner;
    PeerToPeerCommunicator* _comm;
    size_t _Nlambda;
    size_t _Nframep;

    std::shared_ptr<Array> _partialCube;
};

////////////////////////////////////////////////////////////////////

#endif // PARALLELDATACUBE_HPP
