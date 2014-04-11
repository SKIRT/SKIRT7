/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include <QHash>
#include <QList>
#include "SimulationItemRegistry.hpp"

////////////////////////////////////////////////////////////////////

// unnamed namespace to make the static data structures private
namespace
{
    // list of registered concrete item types, in order of addition to the registry
    QList<QByteArray> _itemTypes;

    // hash map of the registered meta objects keyed on item type
    QHash<QByteArray, const QMetaObject*> _items;
}

////////////////////////////////////////////////////////////////////

void SimulationItemRegistry::addPrivate(const QMetaObject* item, bool concrete)
{
    QByteArray type(item->className());
    if (concrete) _itemTypes << type;  // don't put abstract types in the list used for inheritance candidates
    _items[type] = item;
}

////////////////////////////////////////////////////////////////////

QList<QByteArray> SimulationItemRegistry::concreteItemTypes()
{
    return _itemTypes;
}

////////////////////////////////////////////////////////////////////

const QMetaObject* SimulationItemRegistry::metaObject(QByteArray itemType)
{
    return _items[itemType];
}

////////////////////////////////////////////////////////////////////
