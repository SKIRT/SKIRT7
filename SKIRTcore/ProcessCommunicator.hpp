/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef PROCESSCOMMUNICATOR_HPP
#define PROCESSCOMMUNICATOR_HPP

#include "SimulationItem.hpp"

class ProcessCommunicator : public SimulationItem
{
    Q_OBJECT

protected:

    ProcessCommunicator();

public:

    virtual ~ProcessCommunicator();

    int getRank() const;

    int getSize() const;

    bool isMultiProc() const;

protected:

    int _rank;

    int _Nprocs;
};

#endif // PROCESSCOMMUNICATOR_HPP
