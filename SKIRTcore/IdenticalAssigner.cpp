/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "FatalError.hpp"
#include "IdenticalAssigner.hpp"
#include "SequentialAssigner.hpp"

////////////////////////////////////////////////////////////////////

IdenticalAssigner::IdenticalAssigner()
    : _blockassigner(0)
{
}

////////////////////////////////////////////////////////////////////

IdenticalAssigner::IdenticalAssigner(SimulationItem *parent)
    : _blockassigner(0)
{
    setParent(parent);
    setup();
}

////////////////////////////////////////////////////////////////////

void IdenticalAssigner::setupSelfBefore()
{
    ProcessAssigner::setupSelfBefore();

    if (!_comm) throw FATALERROR("Could not find an object of type PeerToPeerCommunicator in the simulation hierarchy");
}

////////////////////////////////////////////////////////////////////

void IdenticalAssigner::assign(size_t size, size_t blocks)
{
    _nvalues = size;
    _start = 0;

    // If this assigner was used before with blocks > 1, the _blockassigner is still set. Since it depends on
    // the current number of blocks whether a new _blockassigner is set, we set the corresponding pointer to null
    // for now. Note that the actual object does not have to be deleted since it is part of the simulation hierarchy.
    if (_blockassigner) _blockassigner = 0;

    if (blocks > 1)
    {
        // Create a SequentialAssigner to assign the blocks
        _blockassigner = new SequentialAssigner(this);
        _blockassigner->assign(blocks);

        // Set the number of values for this process and the starting index
        _nvalues = _blockassigner->nvalues() * size;
        _start = _blockassigner->absoluteIndex(0) * size;
    }
}

////////////////////////////////////////////////////////////////////

size_t IdenticalAssigner::absoluteIndex(size_t relativeIndex)
{
    return (_start + relativeIndex);
}

////////////////////////////////////////////////////////////////////

size_t IdenticalAssigner::relativeIndex(size_t absoluteIndex)
{
    return (absoluteIndex - _start);
}

////////////////////////////////////////////////////////////////////

int IdenticalAssigner::rankForIndex(size_t index) const
{
    if (!_blockassigner) throw FATALERROR("This function should never be called when blocks=1");

    // Calculate the block to which the index corresponds and subsequently get the assigned process from
    // the _blockassigner object
    size_t block = index / (_nvalues/_blockassigner->nvalues());
    return _blockassigner->rankForIndex(block);
}

////////////////////////////////////////////////////////////////////

bool IdenticalAssigner::parallel() const
{
    return _blockassigner!=0;
}

////////////////////////////////////////////////////////////////////
