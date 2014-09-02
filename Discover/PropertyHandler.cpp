/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include "PropertyHandler.hpp"
#include "SimulationItemDiscovery.hpp"
#include "BoolPropertyHandler.hpp"

////////////////////////////////////////////////////////////////////

PropertyHandler::PropertyHandler(SimulationItem *target)
    : _target(target), _changed(false)
{
}

////////////////////////////////////////////////////////////////////

PropertyHandler::~PropertyHandler()
{
}

////////////////////////////////////////////////////////////////////

void PropertyHandler::addAttribute(QByteArray key, QByteArray value)
{
    // don't replace a previous value of the attribute
    // so that key/value pairs of subsequent class declarations can't override property attributes
    if (!_attributes.contains(key)) _attributes[key] = value;
}

////////////////////////////////////////////////////////////////////

SimulationItem *PropertyHandler::target() const
{
    return _target;
}

////////////////////////////////////////////////////////////////////

QByteArray PropertyHandler::name() const
{
    return _attributes["Property"];
}

////////////////////////////////////////////////////////////////////

QByteArray PropertyHandler::type() const
{
    return _attributes["Type"];
}

////////////////////////////////////////////////////////////////////

QString PropertyHandler::title() const
{
    if (_attributes.contains("Title")) return _attributes["Title"];
    return "unknown property";
}

////////////////////////////////////////////////////////////////////

bool PropertyHandler::isSilent() const
{
    return BoolPropertyHandler::toBool(_attributes["Silent"]);
}

////////////////////////////////////////////////////////////////////

bool PropertyHandler::isOptional() const
{
    return false;
}

////////////////////////////////////////////////////////////////////

bool PropertyHandler::hasDefaultValue() const
{
    return false;
}

////////////////////////////////////////////////////////////////////

bool PropertyHandler::hasChanged() const
{
    return _changed;
}

////////////////////////////////////////////////////////////////////

bool PropertyHandler::isTrueInCondition() const
{
    return false;
}

////////////////////////////////////////////////////////////////////

bool PropertyHandler::isRelevant() const
{
    if (!_attributes.contains("RelevantIf")) return true;
    QByteArray property = _attributes["RelevantIf"];

    // construct a handler for the target property and evaluate our relevancy
    PropertyHandlerPtr handler = SimulationItemDiscovery::createPropertyHandler(_target, property);
    return handler && handler->isRelevant() && handler->isTrueInCondition();
}

QByteArray PropertyHandler::isRelevantPropertyName() const
{
    if (_attributes.contains("RelevantIf")) return _attributes["RelevantIf"];
    else return QByteArray();
}

////////////////////////////////////////////////////////////////////

int PropertyHandler::sortIndex() const
{
    return 0; // default value for scalars
}

////////////////////////////////////////////////////////////////////

void PropertyHandler::acceptVisitor(PropertyHandlerVisitor* /*visitor*/)
{
    // default implementation does nothing
}

////////////////////////////////////////////////////////////////////

QByteArray PropertyHandler::setter() const
{
    QByteArray result = name();
    result[0] = toupper(result[0]);
    result.prepend("set");
    return result;
}

////////////////////////////////////////////////////////////////////

QByteArray PropertyHandler::adder() const
{
    QByteArray result = name();
    result[0] = toupper(result[0]);     // first letter to uppercase
    result.truncate(result.length()-1); // remove the plural "s"
    result.prepend("add");              // add prefix
    return result;
}

////////////////////////////////////////////////////////////////////

QByteArray PropertyHandler::getter() const
{
    return name();
}

////////////////////////////////////////////////////////////////////
