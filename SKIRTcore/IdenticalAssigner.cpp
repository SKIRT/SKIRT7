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

    if (blocks > 1)
    {
        // Create a SequentialAssigner to assign the blocks
        if (!_blockassigner) _blockassigner = new SequentialAssigner(this);
        _blockassigner->assign(blocks);

        // Set the number of values for this process and the starting index
        _nvalues = _blockassigner->nvalues() * size;
        _start = _blockassigner->absoluteIndex(0) * size;
    }
    else
    {
        delete _blockassigner;
        _blockassigner = 0;
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
