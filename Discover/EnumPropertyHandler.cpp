/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include <QMetaObject>
#include "EnumPropertyHandler.hpp"
#include "PropertyHandlerVisitor.hpp"
#include "SimulationItem.hpp"
#include "SimulationItemDiscovery.hpp"

////////////////////////////////////////////////////////////////////

EnumPropertyHandler::EnumPropertyHandler(SimulationItem* target)
    : PropertyHandler(target)
{
}

////////////////////////////////////////////////////////////////////

QStringList EnumPropertyHandler::values() const
{
    QStringList result;
    foreach (QByteArray key, SimulationItemDiscovery::keysInEnum(_target, type())) result << key;
    return result;
}

////////////////////////////////////////////////////////////////////

QStringList EnumPropertyHandler::titlesForValues() const
{
    QStringList result;
    foreach (QByteArray key, SimulationItemDiscovery::keysInEnum(_target, type()))
    {
        if (_attributes.contains(key)) result << _attributes[key];
        else result << key;
    }
    return result;
}

////////////////////////////////////////////////////////////////////

bool EnumPropertyHandler::isValid(QString value) const
{
    return SimulationItemDiscovery::keysInEnum(_target, type()).contains(value.toUtf8());
}

////////////////////////////////////////////////////////////////////

void EnumPropertyHandler::setValue(QString value)
{
    int intvalue = SimulationItemDiscovery::valueForKeyInEnum(_target, type(), value.toUtf8());
    if (intvalue >= 0)
    {
        if (QMetaObject::invokeMethod(_target, setter().constData(), QGenericArgument(type(), &intvalue)))
        {
            _changed = true;
        }
    }
}

////////////////////////////////////////////////////////////////////

QString EnumPropertyHandler::value() const
{
    int result;
    QMetaObject::invokeMethod(_target, getter().constData(), QGenericReturnArgument(type(), &result));
    return SimulationItemDiscovery::keyForValueInEnum(_target, type(), result);
}

////////////////////////////////////////////////////////////////////

QString EnumPropertyHandler::titleForValue() const
{
    QByteArray key = value().toUtf8();
    if (_attributes.contains(key)) return _attributes[key];
    else return key;
}

////////////////////////////////////////////////////////////////////

bool EnumPropertyHandler::hasDefaultValue() const
{
    return isValid(_attributes["Default"]);
}

////////////////////////////////////////////////////////////////////

QString EnumPropertyHandler::defaultValue() const
{
    return _attributes["Default"];
}

////////////////////////////////////////////////////////////////////

bool EnumPropertyHandler::isTrueInCondition() const
{
    return _attributes.contains("TrueIf") && _attributes["TrueIf"] == value();
}

////////////////////////////////////////////////////////////////////

void EnumPropertyHandler::acceptVisitor(PropertyHandlerVisitor* visitor)
{
    visitor->visitPropertyHandler(this);
}

////////////////////////////////////////////////////////////////////
