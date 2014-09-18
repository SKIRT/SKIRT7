/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef SIMULATIONITEMDISCOVERY_HPP
#define SIMULATIONITEMDISCOVERY_HPP

#include <QHash>
#include <QSet>
#include <QStringList>
#include "PropertyHandler.hpp"
class SimulationItem;

////////////////////////////////////////////////////////////////////

/** This namespace offers a set of utility functions for discovering compile-time class information
    on simulation items. The functions in this namespace assume that the simulation item registry
    has been appropriately initialized and that the simulation item class declarations contain the
    appropriate class info definitions to make the class and its properties discoverable. */
namespace SimulationItemDiscovery
{
    /** Returns the item type (i.e. class name) for the specified simulation item. */
    QByteArray itemType(SimulationItem* item);

    /** Returns the base item type for the specified simulation item, i.e. the nearest abstract
        class in the item's inheritance sequence. */
    QByteArray itemBaseType(SimulationItem* item);

    /** Returns the title (used for display to a user) associated with the specified item type. */
    QString title(QByteArray itemType);

    /** Returns the titles (used for display to a user) associated with the specified item types,
        in the same order. */
    QStringList titles(QList<QByteArray> itemTypes);

    /** Returns true if the specified item type is allowed according to the "AllowedIf" conditions
        defined in its class declaration, as tested against the specified set of keywords. The
        function uses the value of each "AllowedIf" class info attribute defined for the specified
        item type or any of its base classes as a condition to be tested. A condition value can
        have multiple comma-separated segments. If a segment does not start with an exclamation
        mark, the segment fails if its value does not occur as a keyword. If a segment starts with
        an exclamation mark, the segment fails if its value occurs as a keyword. If at least one
        segment in a condition succeeds, the complete condition succeeds. If at least one condition
        fails, the function returns false. In other words, segments are ORed and conditions are
        ANDed. */
    bool isAllowed(QByteArray itemType, QSet<QByteArray> keys);

    /** Returns true if the first simulation item type inherits the second. */
    bool inherits(QByteArray childType, QByteArray parentType);

    /** Returns a list of the names for all classes from which the specified item type inherits,
        directly or indirectly, starting with the class itself up to and including QObject.
        Class names are included even if they are not in the simulation item registry. */
    QList<QByteArray> ascendants(QByteArray itemType);

    /** Returns a list of item types that inherit the specified item type, in order of addition to
        the simulation item registry. */
    QList<QByteArray> descendants(QByteArray parentType);

    /** Returns a list of item types, in order of addition to the simulation item registry,
        which inherit the specified item type and which are allowed according to conditional
        rules (see the isAllowed() function) based on the specified set of keywords. */
    QList<QByteArray> allowedDescendants(QByteArray parentType, QSet<QByteArray> keys);

    /** Returns a list of the keys in the specified enumeration type. If the enumeration type is
        not declared in the class corresponding to the specified simulation item, this function
        returns the empty list. */
    QList<QByteArray> keysInEnum(SimulationItem* item, QByteArray enumType);

    /** Returns the value of the specified key in the specified enumeration type. If the
        enumeration type is not declared in the class corresponding to the specified simulation
        item, or it does not contain the specified key, this function returns -1. */
    int valueForKeyInEnum(SimulationItem* item, QByteArray enumType, QByteArray key);

    /** Returns the key for the specified value in the specified enumeration type. If the enumeration
        type is not declared in the class corresponding to the specified simulation item, or it
        does not contain the specified value, this function returns the empty byte array. */
    QByteArray keyForValueInEnum(SimulationItem* item, QByteArray enumType, int value);

    /** Returns the names of all simulation item properties for the specified simulation item. */
    QList<QByteArray> properties(const SimulationItem* item);

    /** Returns a new property handler for a particular property in a simulation item, or null if
        the specified item does not have a property with the specified name. The returned instance
        is a PropertyHandler subclass appropriate for the property type; the new handler is
        automatically deleted when it is no longer referenced. */
    PropertyHandlerPtr createPropertyHandler(SimulationItem* item, QByteArray property);

    /** Returns a list of new property handlers for all properties in a simulation item, in
        declaration order (base class first, where applicable).
        The returned instances are PropertyHandler subclasses appropriate for the property type;
        the new handlers are automatically deleted when no longer referenced. */
    QList<PropertyHandlerPtr> createPropertyHandlersList(SimulationItem* item);

    /** Returns a list of new property handlers for all properties in a simulation item, sorted on
        type (numbers&strings, items, list of items), keeping declaration order within same type.
        The returned instances are PropertyHandler subclasses appropriate for the property type;
        the new handlers are automatically deleted when no longer referenced. */
    QList<PropertyHandlerPtr> createSortedPropertyHandlersList(SimulationItem* item);

    /** Returns a dictionary of new property handlers for all properties in a simulation item,
        keyed on property name. The returned instances are PropertyHandler subclasses appropriate
        for the property type; the new handlers are automatically deleted when no longer
        referenced. */
    QHash<QByteArray, PropertyHandlerPtr> createPropertyHandlersDict(SimulationItem* item);

    /** Returns a new simulation item of the specified type, or null if it couldn't be created.
        Ownership of the new item is passed to caller, who is responsible for deleting it. */
    SimulationItem* createSimulationItem(QByteArray itemType);
}

////////////////////////////////////////////////////////////////////

#endif // SIMULATIONITEMDISCOVERY_HPP
