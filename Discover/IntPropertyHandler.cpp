/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include <QMetaObject>
#include "IntPropertyHandler.hpp"
#include "PropertyHandlerVisitor.hpp"
#include "SimulationItem.hpp"

////////////////////////////////////////////////////////////////////

namespace
{
    // use a nice "round" maximum close to 2**31
    const int MAXINT = 2*1000*1000*1000;
}

////////////////////////////////////////////////////////////////////

IntPropertyHandler::IntPropertyHandler(SimulationItem* target)
    : PropertyHandler(target)
{
}

////////////////////////////////////////////////////////////////////

void IntPropertyHandler::setValue(int value)
{
    if (QMetaObject::invokeMethod(_target, setter().constData(), QGenericArgument("int", &value)))
    {
        _changed = true;
    }
}

////////////////////////////////////////////////////////////////////

int IntPropertyHandler::value() const
{
    int result;
    QMetaObject::invokeMethod(_target, getter().constData(), QGenericReturnArgument("int", &result));
    return result;
}

////////////////////////////////////////////////////////////////////

bool IntPropertyHandler::hasDefaultValue() const
{
    return isValid(_attributes["Default"]);
}

////////////////////////////////////////////////////////////////////

int IntPropertyHandler::defaultValue() const
{
    return toInt(_attributes["Default"]);
}

////////////////////////////////////////////////////////////////////

int IntPropertyHandler::minValue() const
{
    QByteArray value = _attributes["MinValue"];
    if (isValid(value)) return toInt(value);
    else return -MAXINT;
}

////////////////////////////////////////////////////////////////////

int IntPropertyHandler::maxValue() const
{
    QByteArray value = _attributes["MaxValue"];
    if (isValid(value)) return toInt(value);
    else return MAXINT;
}

////////////////////////////////////////////////////////////////////

bool IntPropertyHandler::isTrueInCondition() const
{
    return value() != 0;
}

////////////////////////////////////////////////////////////////////

void IntPropertyHandler::acceptVisitor(PropertyHandlerVisitor* visitor)
{
    visitor->visitPropertyHandler(this);
}

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

bool IntPropertyHandler::isValid(QString value)
{
    if (value.simplified().isEmpty()) return false;
    bool ok;
    value.toInt(&ok);
    return ok;
}

////////////////////////////////////////////////////////////////////

int IntPropertyHandler::toInt(QString value)
{
    return value.toInt();
}

////////////////////////////////////////////////////////////////////

QString IntPropertyHandler::toString(int value)
{
    return QString::number(value);
}

////////////////////////////////////////////////////////////////////
