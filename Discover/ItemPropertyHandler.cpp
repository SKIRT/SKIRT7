/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include <QMetaObject>
#include "BoolPropertyHandler.hpp"
#include "ItemPropertyHandler.hpp"
#include "PropertyHandlerVisitor.hpp"
#include "SimulationItem.hpp"
#include "SimulationItemDiscovery.hpp"
#include "SimulationItemRegistry.hpp"

////////////////////////////////////////////////////////////////////

ItemPropertyHandler::ItemPropertyHandler(SimulationItem* target)
    : PropertyHandler(target)
{
}

////////////////////////////////////////////////////////////////////

QByteArray ItemPropertyHandler::baseType() const
{
    QByteArray typ = type();
    typ.truncate(typ.length()-1);
    return typ;
}

////////////////////////////////////////////////////////////////////

bool ItemPropertyHandler::setPlainValue(QObject *value)
{
    if (value && SimulationItemDiscovery::inherits(value->metaObject()->className(), baseType()))
    {
        bool success = QMetaObject::invokeMethod(_target, setter().constData(),
                                                 QGenericArgument(type().constData(), &value));
        if (success) _changed = true;
        return success;
    }
    return false;
}

////////////////////////////////////////////////////////////////////

bool ItemPropertyHandler::setValue(SimulationItem* value)
{
    return setPlainValue(value);
}

////////////////////////////////////////////////////////////////////

bool ItemPropertyHandler::setToNewItemOfType(QByteArray itemType)
{
    const QMetaObject* metaObject = SimulationItemRegistry::metaObject(itemType);
    if (metaObject) return setPlainValue(metaObject->newInstance());
    return false;
}

////////////////////////////////////////////////////////////////////

SimulationItem* ItemPropertyHandler::value() const
{
    SimulationItem* result;
    QMetaObject::invokeMethod(_target, getter().constData(), QGenericReturnArgument(type().constData(), &result));
    return result;
}

////////////////////////////////////////////////////////////////////

bool ItemPropertyHandler::isOptional() const
{
    return BoolPropertyHandler::toBool(_attributes["Optional"]);
}

////////////////////////////////////////////////////////////////////

bool ItemPropertyHandler::hasDefaultValue() const
{
    QByteArray itemType = _attributes["Default"];
    return !itemType.isEmpty() && SimulationItemDiscovery::inherits(itemType, baseType());
}

////////////////////////////////////////////////////////////////////

QByteArray ItemPropertyHandler::defaultItemType() const
{
    return _attributes["Default"];
}

////////////////////////////////////////////////////////////////////

bool ItemPropertyHandler::isTrueInCondition() const
{
    return value() != 0;
}

////////////////////////////////////////////////////////////////////

int ItemPropertyHandler::sortIndex() const
{
    return 10;
}

////////////////////////////////////////////////////////////////////

void ItemPropertyHandler::acceptVisitor(PropertyHandlerVisitor* visitor)
{
    visitor->visitPropertyHandler(this);
}

////////////////////////////////////////////////////////////////////
