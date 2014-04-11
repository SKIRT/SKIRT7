/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include "FatalError.hpp"
#include "SimulationItem.hpp"

////////////////////////////////////////////////////////////////////

SimulationItem::SimulationItem()
    : _state(Created)
{
}

////////////////////////////////////////////////////////////////////

void SimulationItem::setup()
{
    if (_state > Created) return;

    _state = SetupInProgress;
    setupSelfBefore();
    foreach (QObject* child, children())
    {
        SimulationItem* item = dynamic_cast<SimulationItem*>(child);
        if (item) item->setup();
    }
    setupSelfAfter();
    _state = SetupDone;
}

////////////////////////////////////////////////////////////////////

void SimulationItem::setupSelfBefore()
{
}

////////////////////////////////////////////////////////////////////

void SimulationItem::setupSelfAfter()
{
}

////////////////////////////////////////////////////////////////////

QObject* SimulationItem::find(const char* className) const
{
    // loop over all ancestors
    QObject* ancestor = const_cast<SimulationItem*>(this);  // cast away const
    while (ancestor)
    {
        // test the ancestor
        if (ancestor->inherits(className)) return ancestor;

        // test its children
        foreach (QObject* child, ancestor->children())
        {
            if (child->inherits(className)) return child;
        }

        // next ancestor
        ancestor = ancestor->parent();
    }

    throw FATALERROR("No simulation item of type " + QString(className) + " found in hierarchy");
}

////////////////////////////////////////////////////////////////////

QList<SimulationItem*> SimulationItem::interfaceCandidates(const std::type_info& /*interfaceTypeInfo*/)
{
    return QList<SimulationItem*>() << this;
}

////////////////////////////////////////////////////////////////////
