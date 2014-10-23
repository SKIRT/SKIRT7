/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include <QMetaObject>
#include "BoolPropertyHandler.hpp"
#include "ItemListPropertyHandler.hpp"
#include "PropertyHandlerVisitor.hpp"
#include "SimulationItem.hpp"
#include "SimulationItemDiscovery.hpp"
#include "SimulationItemRegistry.hpp"

////////////////////////////////////////////////////////////////////

ItemListPropertyHandler::ItemListPropertyHandler(SimulationItem* target)
    : PropertyHandler(target)
{
}

////////////////////////////////////////////////////////////////////

QByteArray ItemListPropertyHandler::baseType() const
{
    QByteArray typ = type();
    typ.remove(0, typ.indexOf('<')+1);
    typ.truncate(typ.length()-2);
    return typ;
}

////////////////////////////////////////////////////////////////////

QByteArray ItemListPropertyHandler::ptrType() const
{
    return baseType() + "*";
}

////////////////////////////////////////////////////////////////////

bool ItemListPropertyHandler::addPlainValue(QObject *value)
{
    if (value && SimulationItemDiscovery::inherits(value->metaObject()->className(), baseType()))
    {
        bool success = QMetaObject::invokeMethod(_target, adder().constData(),
                                                 QGenericArgument(ptrType().constData(), &value));
        if (success) _changed = true;
        return success;
    }
    return false;
}

////////////////////////////////////////////////////////////////////

bool ItemListPropertyHandler::addValue(SimulationItem* value)
{
    return addPlainValue(value);
}

////////////////////////////////////////////////////////////////////

bool ItemListPropertyHandler::addNewItemOfType(QByteArray itemType)
{
    const QMetaObject* metaObject = SimulationItemRegistry::metaObject(itemType);
    if (metaObject) return addPlainValue(metaObject->newInstance());
    return false;
}

////////////////////////////////////////////////////////////////////

QList<SimulationItem*> ItemListPropertyHandler::value() const
{
    QList<SimulationItem*> result;
    QMetaObject::invokeMethod(_target, getter().constData(), QGenericReturnArgument(type().constData(), &result));
    return result;
}

////////////////////////////////////////////////////////////////////

bool ItemListPropertyHandler::isOptional() const
{
    return BoolPropertyHandler::toBool(_attributes["Optional"]);
}

////////////////////////////////////////////////////////////////////

bool ItemListPropertyHandler::hasDefaultValue() const
{
    QByteArray itemType = _attributes["Default"];
    return !itemType.isEmpty() && SimulationItemDiscovery::inherits(itemType, baseType());
}

////////////////////////////////////////////////////////////////////

QByteArray ItemListPropertyHandler::defaultItemType() const
{
    return _attributes["Default"];
}

////////////////////////////////////////////////////////////////////

bool ItemListPropertyHandler::isTrueInCondition() const
{
    return !value().isEmpty();
}

////////////////////////////////////////////////////////////////////

int ItemListPropertyHandler::sortIndex() const
{
    return 100;
}

////////////////////////////////////////////////////////////////////

void ItemListPropertyHandler::acceptVisitor(PropertyHandlerVisitor* visitor)
{
    visitor->visitPropertyHandler(this);
}

////////////////////////////////////////////////////////////////////
