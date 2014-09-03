/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include <cmath>
#include <limits>
#include <QLocale>
#include <QMetaObject>
#include <QStringList>
#include "DoublePropertyHandler.hpp"
#include "EnumPropertyHandler.hpp"
#include "FatalError.hpp"
#include "PropertyHandlerVisitor.hpp"
#include "SimulationItem.hpp"
#include "SimulationItemDiscovery.hpp"
#include "Units.hpp"

////////////////////////////////////////////////////////////////////

DoublePropertyHandler::DoublePropertyHandler(SimulationItem* target)
    : PropertyHandler(target)
{
}

////////////////////////////////////////////////////////////////////

void DoublePropertyHandler::setValue(double value)
{
    if (QMetaObject::invokeMethod(_target, setter().constData(), QGenericArgument("double", &value)))
    {
        _changed = true;
    }
}

////////////////////////////////////////////////////////////////////

double DoublePropertyHandler::value() const
{
    double result;
    QMetaObject::invokeMethod(_target, getter().constData(), QGenericReturnArgument("double", &result));
    return result;
}

////////////////////////////////////////////////////////////////////

bool DoublePropertyHandler::hasDefaultValue() const
{
    return isValid(_attributes["Default"]);
}

////////////////////////////////////////////////////////////////////

double DoublePropertyHandler::defaultValue() const
{
    return toDouble(_attributes["Default"]);
}

////////////////////////////////////////////////////////////////////

double DoublePropertyHandler::minValue() const
{
    QByteArray value = _attributes["MinValue"];
    if (isValid(value)) return toDouble(value);
    else return -std::numeric_limits<double>::infinity();
}

////////////////////////////////////////////////////////////////////

double DoublePropertyHandler::maxValue() const
{
    QByteArray value = _attributes["MaxValue"];
    if (isValid(value)) return toDouble(value);
    else return std::numeric_limits<double>::infinity();
}

////////////////////////////////////////////////////////////////////

QString DoublePropertyHandler::quantity() const
{
    QString result = _attributes["Quantity"];

    // if the attribute value starts with an at sign, the quantity is determined as the string value
    // of the indicated enumeration property
    if (result.startsWith('@'))
    {
        // construct a handler for the target property and get its string value
        QByteArray property = result.mid(1).toUtf8();
        PropertyHandlerPtr handler = SimulationItemDiscovery::createPropertyHandler(_target, property);
        EnumPropertyHandler* enumhandler = dynamic_cast<EnumPropertyHandler*>(handler.data());
        result = enumhandler ? enumhandler->value() : "";

        // replace unknown quantity values by "dimensionless"
        if (!_target->find<Units>()->isQuantity(result)) result = "";
    }
    return result;
}

////////////////////////////////////////////////////////////////////

void DoublePropertyHandler::acceptVisitor(PropertyHandlerVisitor* visitor)
{
    visitor->visitPropertyHandler(this);
}

////////////////////////////////////////////////////////////////////

bool DoublePropertyHandler::isValid(QString value) const
{
    return isValidDoubleString(value, quantity(), _target->find<Units>());
}

////////////////////////////////////////////////////////////////////

double DoublePropertyHandler::toDouble(QString value) const
{
    return convertStringtoDouble(value, quantity(), _target->find<Units>());
}

////////////////////////////////////////////////////////////////////

QString DoublePropertyHandler::toString(double value) const
{
    return convertDoubletoString(value, quantity(), _target->find<Units>());
}

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

bool DoublePropertyHandler::isValidDoubleString(QString value, QString quantity, const Units* units)
{
    // split into segments; must have exactly one or two segments
    QStringList segments = value.simplified().split(' ');
    if (segments.isEmpty() || segments.size()>2) return false;

    // verify conversion of the first segment to a number
    // use the standard decimal point (.) and reject digit group separators
    QLocale locale(QLocale::C);
    locale.setNumberOptions(QLocale::RejectGroupSeparator);
    bool ok;
    locale.toDouble(segments[0], &ok);
    if (!ok) return false;

    // handle the physical quantity
    if (quantity.isEmpty())
    {
        // a dimensionless quantity may have no unit spec
        return segments.size()==1;
    }
    else
    {
        // the physical quantity must be known to the Units class; if not this is a programming error
        // so we let this function throw if the physical quantity is unknown
        units->in(quantity);

        // a physical quantity may have a unit spec, which must be known to the Units class
        // since this is an input error rather than a programming error, we catch the exception
        if (segments.size()==2)
        {
            try { units->in(quantity, segments[1]); }
            catch (FatalError&) { return false; }
        }
        return true;
    }
}

////////////////////////////////////////////////////////////////////

double DoublePropertyHandler::convertStringtoDouble(QString value, QString quantity, const Units* units)
{
    // split into segments; must have exactly one or two segments
    QStringList segments = value.simplified().split(' ');
    if (segments.isEmpty() || segments.size()>2) return 0;

    // convert the first segment to a number
    // use the standard decimal point (.) and reject digit group separators
    QLocale locale(QLocale::C);
    locale.setNumberOptions(QLocale::RejectGroupSeparator);
    double resultWithoutUnits = locale.toDouble(segments[0]);

    // handle the physical quantity
    if (quantity.isEmpty())
    {
        // a dimensionless quantity may have no unit spec and needs no conversion
        return segments.size()==1 ? resultWithoutUnits : 0;
    }
    else
    {
        // the physical quantity must be known to the Units class; if not this is a programming error
        // so we let this function throw if the physical quantity is unknown
        double resultWithDefaultUnits = units->in(quantity, resultWithoutUnits);
        if (segments.size()==1) return resultWithDefaultUnits;

        // a physical quantity may have a unit spec, which must be known to the Units class
        // since this is an input error rather than a programming error, we catch the exceptions
        try { return units->in(quantity, segments[1], resultWithoutUnits); }
        catch (FatalError&) { return 0; }
    }
}

////////////////////////////////////////////////////////////////////

QString DoublePropertyHandler::convertDoubletoString(double value, QString quantity, const Units* units)
{
    // get unit specification and convert value to external units if needed
    QString unitspec;
    if (!quantity.isEmpty())
    {
        value = units->out(quantity, value);         // overwrite incoming value
        unitspec = " " + units->unit(quantity);      // include separating space
    }

    // for conversions in this direction QString always uses the C locale without group separators
    // but use a decent representation for not-a-number and infinity
    QString number = QString::number(value, 'g', 10);
    if (std::isnan(value)) number = "∅";
    if (std::isinf(value)) number = value<0 ? "-∞" : "∞";

    // remove leading zeroes and the + sign in the exponent
    number.replace("e-0","e-");
    number.replace("e+0","e");
    number.replace("e+","e");

    // replace 4 or more trailing zeroes by exponent
    int zeroes = 0;
    for (int i = number.length()-1; i >=0 ; i--, zeroes++) if (number[i] != '0') break;
    if (zeroes > 3) number = number.left(number.length()-zeroes) + "e" + QString::number(zeroes);

    return number + unitspec;
}

////////////////////////////////////////////////////////////////////
