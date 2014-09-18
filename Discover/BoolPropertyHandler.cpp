/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include <QMetaObject>
#include "BoolPropertyHandler.hpp"
#include "PropertyHandlerVisitor.hpp"
#include "SimulationItem.hpp"

////////////////////////////////////////////////////////////////////

BoolPropertyHandler::BoolPropertyHandler(SimulationItem* target)
    : PropertyHandler(target)
{
}

////////////////////////////////////////////////////////////////////

void BoolPropertyHandler::setValue(bool value)
{
    if (QMetaObject::invokeMethod(_target, setter().constData(), QGenericArgument("bool", &value)))
    {
        _changed = true;
    }
}

////////////////////////////////////////////////////////////////////

bool BoolPropertyHandler::value() const
{
    bool result;
    QMetaObject::invokeMethod(_target, getter().constData(), QGenericReturnArgument("bool", &result));
    return result;
}

////////////////////////////////////////////////////////////////////

bool BoolPropertyHandler::hasDefaultValue() const
{
    return isValid(_attributes["Default"]);
}

////////////////////////////////////////////////////////////////////

bool BoolPropertyHandler::defaultValue() const
{
    return toBool(_attributes["Default"]);
}

////////////////////////////////////////////////////////////////////

bool BoolPropertyHandler::isTrueInCondition() const
{
    return value();
}

////////////////////////////////////////////////////////////////////

void BoolPropertyHandler::acceptVisitor(PropertyHandlerVisitor* visitor)
{
    visitor->visitPropertyHandler(this);
}

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

bool BoolPropertyHandler::isValid(QString value)
{
    QString lower = value.simplified().toLower();
    if (lower == "true"  || lower == "t" || lower == "yes" || lower == "y" || lower == "1" ||
        lower == "false" || lower == "f" || lower == "no"  || lower == "n" || lower == "0") return true;
    return false;
}

////////////////////////////////////////////////////////////////////

bool BoolPropertyHandler::toBool(QString value)
{
    QString lower = value.simplified().toLower();
    if (lower == "true" || lower == "t" || lower == "yes" || lower == "y" || lower == "1") return true;
    return false;
}

////////////////////////////////////////////////////////////////////

QString BoolPropertyHandler::toString(bool value)
{
    return value ? "true" : "false";
}

////////////////////////////////////////////////////////////////////
