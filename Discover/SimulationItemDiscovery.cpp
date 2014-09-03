/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include <QMetaClassInfo>
#include <QMetaObject>
#include "BoolPropertyHandler.hpp"
#include "DoublePropertyHandler.hpp"
#include "DoubleListPropertyHandler.hpp"
#include "EnumPropertyHandler.hpp"
#include "IntPropertyHandler.hpp"
#include "ItemListPropertyHandler.hpp"
#include "ItemPropertyHandler.hpp"
#include "SimulationItem.hpp"
#include "SimulationItemDiscovery.hpp"
#include "SimulationItemRegistry.hpp"
#include "StringPropertyHandler.hpp"

////////////////////////////////////////////////////////////////////

QByteArray SimulationItemDiscovery::itemType(SimulationItem* item)
{
    return item->metaObject()->className();
}

////////////////////////////////////////////////////////////////////

QByteArray SimulationItemDiscovery::itemBaseType(SimulationItem* item)
{
    const QMetaObject* current = item->metaObject();
    while (current)
    {
        QByteArray name = current->className();
        // return the first item type that does NOT occur in the list of concrete classes
        if (!SimulationItemRegistry::concreteItemTypes().contains(name)) return name;
        current = current->superClass();
    }
    return QByteArray();
}

////////////////////////////////////////////////////////////////////

QString SimulationItemDiscovery::title(QByteArray itemType)
{
    const QMetaObject* object = SimulationItemRegistry::metaObject(itemType);
    if (object) for (int index = object->classInfoOffset(); index < object->classInfoCount(); index++)
    {
        QMetaClassInfo info = object->classInfo(index);
        if (strcmp(info.name(), "Title") == 0) return info.value();
    }
    return "Unknown Simulation Item";
}

////////////////////////////////////////////////////////////////////

QStringList SimulationItemDiscovery::titles(QList<QByteArray> itemTypes)
{
    QStringList result;
    foreach (QByteArray itemType, itemTypes)
    {
        result << title(itemType);
    }
    return result;
}

////////////////////////////////////////////////////////////////////

namespace
{
    bool isConditionAllowed(QByteArray condition, QSet<QByteArray> keys)
    {
        // segments are "ORed" -> detect any segment that succeeds with the specified set of keys
        QList<QByteArray> segments = condition.split(',');
        foreach (QByteArray segment, segments)
        {
            if (segment.startsWith('!'))
            {
                if (!keys.contains(segment.right(segment.size()-1))) return true;
            }
            else
            {
                if (keys.contains(segment)) return true;
            }
        }
        return false;
    }
}

bool SimulationItemDiscovery::isAllowed(QByteArray itemType, QSet<QByteArray> keys)
{
    const QMetaObject* object = SimulationItemRegistry::metaObject(itemType);
    if (!object) return false;

    // assemble the values of all "AllowedIf" attributes for the item type
    QList<QByteArray> conditions;
    for (int index = 0; index < object->classInfoCount(); index++)
    {
        QMetaClassInfo info = object->classInfo(index);
        if (strcmp(info.name(), "AllowedIf") == 0) conditions << info.value();
    }

    // conditions are "ANDed" -> detect any condition that fails with the specified set of keys
    foreach (QByteArray condition, conditions)
    {
        if (!isConditionAllowed(condition, keys)) return false;
    }

    // if we reach this point, the item type is allowed
    return true;
}

////////////////////////////////////////////////////////////////////

bool SimulationItemDiscovery::inherits(QByteArray childType, QByteArray parentType)
{
    const QMetaObject* current = SimulationItemRegistry::metaObject(childType);
    while (current)
    {
        if (strcmp(current->className(),parentType.constData()) == 0) return true;
        current = current->superClass();
    }
    return false;
}

////////////////////////////////////////////////////////////////////

QList<QByteArray> SimulationItemDiscovery::ascendants(QByteArray itemType)
{
    QList<QByteArray> result;
    const QMetaObject* current = SimulationItemRegistry::metaObject(itemType);
    while (current)
    {
        result << current->className();
        current = current->superClass();
    }
    return result;
}

////////////////////////////////////////////////////////////////////

QList<QByteArray> SimulationItemDiscovery::descendants(QByteArray parentType)
{
    QList<QByteArray> result;

    foreach (QByteArray candidateType, SimulationItemRegistry::concreteItemTypes())
    {
        if (inherits(candidateType, parentType)) result << candidateType;
    }
    return result;
}

////////////////////////////////////////////////////////////////////

QList<QByteArray> SimulationItemDiscovery::allowedDescendants(QByteArray parentType, QSet<QByteArray> keys)
{
    QList<QByteArray> result;

    foreach (QByteArray candidateType, SimulationItemRegistry::concreteItemTypes())
    {
        if (inherits(candidateType, parentType) && isAllowed(candidateType, keys)) result << candidateType;
    }
    return result;
}

////////////////////////////////////////////////////////////////////

QList<QByteArray> SimulationItemDiscovery::keysInEnum(SimulationItem* item, QByteArray enumType)
{
    QList<QByteArray> result;

    // find the appropriate enum declaration
    const QMetaObject* object = item->metaObject();
    for (int index = 0; index < object->enumeratorCount(); index++)
    {
        QMetaEnum enumerator = object->enumerator(index);
        if (strcmp(enumerator.name(), enumType.constData()) == 0)
        {
            // accumulate the keys
            for (int k = 0; k < enumerator.keyCount(); k++) result << enumerator.key(k);
            break;
        }
    }
    return result;
}

////////////////////////////////////////////////////////////////////

int SimulationItemDiscovery::valueForKeyInEnum(SimulationItem* item, QByteArray enumType, QByteArray key)
{
    const QMetaObject* object = item->metaObject();
    for (int index = 0; index < object->enumeratorCount(); index++)
    {
        QMetaEnum enumerator = object->enumerator(index);
        if (strcmp(enumerator.name(), enumType.constData()) == 0) return enumerator.keyToValue(key);
    }
    return -1;
}

////////////////////////////////////////////////////////////////////

QByteArray SimulationItemDiscovery::keyForValueInEnum(SimulationItem* item, QByteArray enumType, int value)
{
    const QMetaObject* object = item->metaObject();
    for (int index = 0; index < object->enumeratorCount(); index++)
    {
        QMetaEnum enumerator = object->enumerator(index);
        if (strcmp(enumerator.name(), enumType.constData()) == 0)
        {
            const char* key = enumerator.valueToKey(value);
            return key ? QByteArray(key) : QByteArray();
        }
    }
    return QByteArray();
}

////////////////////////////////////////////////////////////////////

QList<QByteArray> SimulationItemDiscovery::properties(const SimulationItem* item)
{
    QList<QByteArray> result;

    const QMetaObject* object = item->metaObject();
    for (int index = 0; index < object->classInfoCount(); index++)
    {
        QMetaClassInfo info = object->classInfo(index);
        if (strcmp(info.name(), "Property") == 0) result << info.value();
    }
    return result;
}

////////////////////////////////////////////////////////////////////

PropertyHandlerPtr SimulationItemDiscovery::createPropertyHandler(SimulationItem* item, QByteArray property)
{
    PropertyHandler* result = 0;
    const QMetaObject* object = item->metaObject();

    // find the index of the class info entry for this property
    int infoIndex = -1;
    for (int index = 0; index < object->classInfoCount(); index++)
    {
        QMetaClassInfo info = object->classInfo(index);
        if (strcmp(info.name(), "Property") == 0 && strcmp(info.value(), property.constData()) == 0)
        {
            infoIndex = index;
            break;
        }
    }

    // discover the type of the property from its getter method, which should have the same name
    QByteArray type;
    QByteArray signature = property + "()";
    for (int index = 0; index < object->methodCount(); index++)
    {
        QMetaMethod method = object->method(index);
        if (method.methodSignature() == signature)
        {
            type = method.typeName();
            break;
        }
    }

    // if the class declares valid information about the property
    if (infoIndex >= 0 && !type.isEmpty())
    {
        // create a property handler of the appropriate type
        if (type == "bool") result = new BoolPropertyHandler(item);
        else if (type == "int") result = new IntPropertyHandler(item);
        else if (type == "double") result = new DoublePropertyHandler(item);
        else if (type == "QList<double>") result = new DoubleListPropertyHandler(item);
        else if (type == "QString") result = new StringPropertyHandler(item);
        else if (type.endsWith("*")     // also verify that the base type is in the registry
                 && SimulationItemRegistry::metaObject(type.mid(0,type.length()-1)))
            result = new ItemPropertyHandler(item);
        else if (type.startsWith("QList<") && type.endsWith("*>") // also verify that the base type is in the registry
                 && SimulationItemRegistry::metaObject(type.mid(6,type.length()-8)))
            result = new ItemListPropertyHandler(item);
        else if (!keysInEnum(item, type).isEmpty())
            result = new EnumPropertyHandler(item);

        // if the type is supported
        if (result)
        {
            // add key/value pairs for the property name and type
            result->addAttribute("Property", property);
            result->addAttribute("Type", type);

            // add key/value pairs for the property's class info entries
            for (int index = infoIndex+1; index < object->classInfoCount(); index++)
            {
                QMetaClassInfo info = object->classInfo(index);
                if (strcmp(info.name(), "Property") == 0) break;
                result->addAttribute(info.name(), info.value());
            }
        }
    }
    return QSharedPointer<PropertyHandler>(result);
}

////////////////////////////////////////////////////////////////////

QList<PropertyHandlerPtr> SimulationItemDiscovery::createPropertyHandlersList(SimulationItem* item)
{
    // create a list of handlers
    QList<PropertyHandlerPtr> handlers;
    foreach (QByteArray property, properties(item))
    {
        handlers << createPropertyHandler(item, property);
    }
    return handlers;
}

////////////////////////////////////////////////////////////////////

namespace
{
    bool lessThanForHandlers(const PropertyHandlerPtr handler1, const PropertyHandlerPtr handler2)
    {
        return handler1->sortIndex() < handler2->sortIndex();
    }
}

////////////////////////////////////////////////////////////////////

QList<PropertyHandlerPtr> SimulationItemDiscovery::createSortedPropertyHandlersList(SimulationItem* item)
{
    QList<PropertyHandlerPtr> handlers = createPropertyHandlersList(item);
    qStableSort(handlers.begin(), handlers.end(), lessThanForHandlers);
    return handlers;
}

////////////////////////////////////////////////////////////////////

QHash<QByteArray, PropertyHandlerPtr> SimulationItemDiscovery::createPropertyHandlersDict(SimulationItem* item)
{
    QHash<QByteArray, PropertyHandlerPtr> handlers;
    foreach (QByteArray property, properties(item))
    {
        handlers[property] = createPropertyHandler(item, property);
    }
    return handlers;
}

////////////////////////////////////////////////////////////////////

SimulationItem* SimulationItemDiscovery::createSimulationItem(QByteArray itemType)
{
    const QMetaObject* metaObject = SimulationItemRegistry::metaObject(itemType);
    if (metaObject) return dynamic_cast<SimulationItem*>(metaObject->newInstance());
    return 0;
}

////////////////////////////////////////////////////////////////////
