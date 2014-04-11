/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include <limits>
#include <QMetaObject>
#include <QStringList>
#include "DoubleListPropertyHandler.hpp"
#include "DoublePropertyHandler.hpp"
#include "PropertyHandlerVisitor.hpp"
#include "SimulationItem.hpp"
#include "Units.hpp"

////////////////////////////////////////////////////////////////////

DoubleListPropertyHandler::DoubleListPropertyHandler(SimulationItem* target)
    : PropertyHandler(target)
{
}

////////////////////////////////////////////////////////////////////

void DoubleListPropertyHandler::setValue(QList<double> value)
{
    if (QMetaObject::invokeMethod(_target, setter().constData(), QGenericArgument("QList<double>", &value)))
    {
        _changed = true;
    }
}

////////////////////////////////////////////////////////////////////

QList<double> DoubleListPropertyHandler::value() const
{
    QList<double> result;
    QMetaObject::invokeMethod(_target, getter().constData(), QGenericReturnArgument("QList<double>", &result));
    return result;
}

////////////////////////////////////////////////////////////////////

double DoubleListPropertyHandler::minValue() const
{
    QByteArray value = _attributes["MinValue"];
    if (DoublePropertyHandler::isValidDoubleString(value, quantity(), _target->find<Units>()))
        return DoublePropertyHandler::convertStringtoDouble(value, quantity(), _target->find<Units>());
    else return -std::numeric_limits<double>::infinity();
}

////////////////////////////////////////////////////////////////////

double DoubleListPropertyHandler::maxValue() const
{
    QByteArray value = _attributes["MaxValue"];
    if (DoublePropertyHandler::isValidDoubleString(value, quantity(), _target->find<Units>()))
        return DoublePropertyHandler::convertStringtoDouble(value, quantity(), _target->find<Units>());
    else return std::numeric_limits<double>::infinity();
}

////////////////////////////////////////////////////////////////////

QString DoubleListPropertyHandler::quantity() const
{
    return _attributes["Quantity"];
}

////////////////////////////////////////////////////////////////////

void DoubleListPropertyHandler::acceptVisitor(PropertyHandlerVisitor* visitor)
{
    visitor->visitPropertyHandler(this);
}

////////////////////////////////////////////////////////////////////

bool DoubleListPropertyHandler::isValid(QString value) const
{
    QStringList items = value.split(',');

    // if there are no items, or if any of the items is invalid, return false
    if (items.isEmpty()) return false;
    QString qty = quantity();
    Units* units = _target->find<Units>();
    foreach (QString item, items)
    {
        if (!DoublePropertyHandler::isValidDoubleString(item, qty, units)) return false;
    }

    // otherwise return true
    return true;
}

////////////////////////////////////////////////////////////////////

QList<double> DoubleListPropertyHandler::toDoubleList(QString value) const
{
    QStringList items = value.split(',');
    QString qty = quantity();
    Units* units = _target->find<Units>();

    QList<double> result;
    foreach (QString item, items)
    {
        result << DoublePropertyHandler::convertStringtoDouble(item, qty, units);
    }
    return result;
}

////////////////////////////////////////////////////////////////////

double DoubleListPropertyHandler::toDouble(QString value) const
{
    return DoublePropertyHandler::convertStringtoDouble(value, quantity(), _target->find<Units>());
}

////////////////////////////////////////////////////////////////////

QString DoubleListPropertyHandler::toString(QList<double> value) const
{
    QString qty = quantity();
    Units* units = _target->find<Units>();

    QStringList result;
    foreach (double item, value)
    {
        result << DoublePropertyHandler::convertDoubletoString(item, qty, units);
    }
    return result.join(", ");
}

////////////////////////////////////////////////////////////////////

QString DoubleListPropertyHandler::toString(double value) const
{
    return DoublePropertyHandler::convertDoubletoString(value, quantity(), _target->find<Units>());
}

////////////////////////////////////////////////////////////////////
