/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "SimulationItemTools.hpp"

#include "SimulationItem.hpp"
#include "SimulationItemDiscovery.hpp"
#include <QVariant>

////////////////////////////////////////////////////////////////////

void SimulationItemTools::setPropertyConfigured(SimulationItem* item, QByteArray property, bool configured)
{
    item->setProperty(property+"_configured", configured);
}

////////////////////////////////////////////////////////////////////

void SimulationItemTools::setHierarchyConfigured(SimulationItem* root)
{
    // process all immediate properties of the specified root item
    for (auto property : SimulationItemDiscovery::properties(root))
    {
        setPropertyConfigured(root, property, true);
    }

    // process all children of the specified root item
    for (auto child : root->children())
    {
        SimulationItem* item = dynamic_cast<SimulationItem*>(child);
        if (item) setHierarchyConfigured(item);
    }
}

////////////////////////////////////////////////////////////////////

bool SimulationItemTools::isPropertyConfigured(SimulationItem* item, QByteArray property)
{
    return item->property(property+"_configured").toBool();
}

////////////////////////////////////////////////////////////////////

void SimulationItemTools::setItemComplete(SimulationItem* item)
{
    item->setProperty("item_complete", true);
}

////////////////////////////////////////////////////////////////////

void SimulationItemTools::setHierarchyComplete(SimulationItem* root)
{
    setItemComplete(root);

    // process all children of the specified root item
    for (auto child : root->children())
    {
        SimulationItem* item = dynamic_cast<SimulationItem*>(child);
        if (item) setHierarchyComplete(item);
    }
}

////////////////////////////////////////////////////////////////////

void SimulationItemTools::setItemIncomplete(SimulationItem* item)
{
    QObject* current = item;
    while (current)
    {
        current->setProperty("item_complete", false);
        current = current->parent();
    }
}

////////////////////////////////////////////////////////////////////

bool SimulationItemTools::isItemComplete(SimulationItem* item)
{
    return item->property("item_complete").toBool();
}

////////////////////////////////////////////////////////////////////

void SimulationItemTools::storeSelectedRow(SimulationItem* item, QByteArray property, int row)
{
    item->setProperty(property+"_row", row);
}

////////////////////////////////////////////////////////////////////

int SimulationItemTools::retrieveSelectedRow(SimulationItem* item, QByteArray property)
{
    return item->property(property+"_row").toInt();
}

////////////////////////////////////////////////////////////////////

SimulationItem* SimulationItemTools::getRoot(SimulationItem* item)
{
    SimulationItem* current = item;
    while (true)
    {
        SimulationItem* parent = dynamic_cast<SimulationItem*>(current->parent());
        if (!parent) return current;
        current = parent;
    }
}

////////////////////////////////////////////////////////////////////

void SimulationItemTools::addHierarchyTypeNames(SimulationItem* root, QSet<QByteArray>& keys)
{
    // process the specified root item
    for (QByteArray key : SimulationItemDiscovery::ascendants(SimulationItemDiscovery::itemType(root)))
    {
        keys << key;
    }

    // process all children of the specified root item
    for (auto child : root->children())
    {
        SimulationItem* item = dynamic_cast<SimulationItem*>(child);
        if (item) addHierarchyTypeNames(item, keys);
    }
}

////////////////////////////////////////////////////////////////////

QList<QByteArray> SimulationItemTools::allowedDescendants(QByteArray parentType, SimulationItem* item)
{
    QSet<QByteArray> keys;
    addHierarchyTypeNames(getRoot(item), keys);
    return SimulationItemDiscovery::allowedDescendants(parentType, keys);
}

////////////////////////////////////////////////////////////////////
