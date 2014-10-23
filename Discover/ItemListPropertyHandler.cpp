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

bool ItemListPropertyHandler::insertPlainValue(int index, QObject *value)
{
    if (value && SimulationItemDiscovery::inherits(value->metaObject()->className(), baseType()))
    {
        bool success = QMetaObject::invokeMethod(_target, inserter().constData(), Q_ARG(int, index),
                                                 QGenericArgument(ptrType().constData(), &value));
        if (success) _changed = true;
        return success;
    }
    return false;
}

////////////////////////////////////////////////////////////////////

bool ItemListPropertyHandler::addValue(SimulationItem* value)
{
    return insertValue(this->value().size(), value);
}

////////////////////////////////////////////////////////////////////

bool ItemListPropertyHandler::addNewItemOfType(QByteArray itemType)
{
    return insertNewItemOfType(this->value().size(), itemType);
}

////////////////////////////////////////////////////////////////////

bool ItemListPropertyHandler::insertValue(int index, SimulationItem* value)
{
    return insertPlainValue(index, value);
}

////////////////////////////////////////////////////////////////////

bool ItemListPropertyHandler::insertNewItemOfType(int index, QByteArray itemType)
{
    const QMetaObject* metaObject = SimulationItemRegistry::metaObject(itemType);
    if (metaObject) return insertPlainValue(index, metaObject->newInstance());
    return false;
}

////////////////////////////////////////////////////////////////////

bool ItemListPropertyHandler::removeValueAt(int index)
{
    if (index >= 0 && index < value().size())
    {
        bool success = QMetaObject::invokeMethod(_target, remover().constData(), Q_ARG(int, index));
        if (success) _changed = true;
        return success;
    }
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
