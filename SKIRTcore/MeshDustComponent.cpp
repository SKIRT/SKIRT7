/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "MeshDustComponent.hpp"
#include "DustMix.hpp"
#include "FatalError.hpp"

using namespace std;

//////////////////////////////////////////////////////////////////////

MeshDustComponent::MeshDustComponent()
    : _densityIndex(-1), _multiplierIndex(-1), _densityFraction(1.), _mix(0)
{
}

//////////////////////////////////////////////////////////////////////

void MeshDustComponent::setupSelfBefore()
{
    SimulationItem::setupSelfBefore();

    // verify property values
    if (_densityIndex < 0) throw FATALERROR("Column index for density must be specified");
    if (!_mix) throw FATALERROR("Dust mix was not set");
}

//////////////////////////////////////////////////////////////////////

void MeshDustComponent::setDensityIndex(int value)
{
    _densityIndex = value;
}

////////////////////////////////////////////////////////////////////

int MeshDustComponent::densityIndex() const
{
    return _densityIndex;
}

////////////////////////////////////////////////////////////////////

void MeshDustComponent::setMultiplierIndex(int value)
{
    _multiplierIndex = value;
}

////////////////////////////////////////////////////////////////////

int MeshDustComponent::multiplierIndex() const
{
    return _multiplierIndex;
}

//////////////////////////////////////////////////////////////////////

void MeshDustComponent::setDensityFraction(double value)
{
    if (value <= 0.0) throw FATALERROR("The density fraction should be positive");
    _densityFraction = value;
}

//////////////////////////////////////////////////////////////////////

double MeshDustComponent::densityFraction() const
{
    return _densityFraction;
}

//////////////////////////////////////////////////////////////////////

void MeshDustComponent::setMix(DustMix* value)
{
    if (_mix) delete _mix;
    _mix = value;
    if (_mix) _mix->setParent(this);
}

////////////////////////////////////////////////////////////////////

DustMix* MeshDustComponent::mix() const
{
    return _mix;
}

//////////////////////////////////////////////////////////////////////
