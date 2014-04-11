/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include <QMetaObject>
#include "PropertyHandlerVisitor.hpp"
#include "SimulationItem.hpp"
#include "StringPropertyHandler.hpp"

////////////////////////////////////////////////////////////////////

StringPropertyHandler::StringPropertyHandler(SimulationItem* target)
    : PropertyHandler(target)
{
}

////////////////////////////////////////////////////////////////////

void StringPropertyHandler::setValue(QString value)
{
    if (QMetaObject::invokeMethod(_target, setter().constData(), QGenericArgument("QString", &value)))
    {
        _changed = true;
    }
}

////////////////////////////////////////////////////////////////////

QString StringPropertyHandler::value() const
{
    QString result;
    QMetaObject::invokeMethod(_target, getter().constData(), QGenericReturnArgument("QString", &result));
    return result;
}

////////////////////////////////////////////////////////////////////

bool StringPropertyHandler::hasDefaultValue() const
{
    QByteArray value = _attributes["Default"];
    return !value.isEmpty();
}

////////////////////////////////////////////////////////////////////

QString StringPropertyHandler::defaultValue() const
{
    return _attributes["Default"];
}

////////////////////////////////////////////////////////////////////

void StringPropertyHandler::acceptVisitor(PropertyHandlerVisitor *visitor)
{
    visitor->visitPropertyHandler(this);
}

////////////////////////////////////////////////////////////////////
