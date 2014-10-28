/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef PEERTOPEERCOMMUNICATOR_HPP
#define PEERTOPEERCOMMUNICATOR_HPP

#include "ProcessCommunicator.hpp"

class Array;

class PeerToPeerCommunicator : public ProcessCommunicator
{
    Q_OBJECT

public:
    /** .. ¨*/
    PeerToPeerCommunicator();

    void sum(Array& arr, bool toall);

    bool isRoot();
};

#endif // PEERTOPEERCOMMUNICATOR_HPP
