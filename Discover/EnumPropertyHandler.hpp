/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef ENUMPROPERTYHANDLER_HPP
#define ENUMPROPERTYHANDLER_HPP

#include "PropertyHandler.hpp"

////////////////////////////////////////////////////////////////////

/** This class handles discoverable properties of enumeration types. */
class EnumPropertyHandler : public PropertyHandler
{
public:
    /** Constructs a property handler for the specified simulation item, without initializing any
        property attributes. The property's attributes must be added separately by calling
        addAttribute(). The property handler keeps a reference to the simulation item but doesn't
        assume ownership. The simulation item should not be deleted during the lifetime of the
        property handler. */
    explicit EnumPropertyHandler(SimulationItem* target);

    /** Returns a list of all enumeration keys declared for the type of the handled property. */
    QStringList values() const;

    /** Returns a list of the titles corresponding to all enumeration keys declared for the type of
        the handled property, in the order corresponding to values(). If there is no title, the
        enumeration key is returned instead. */
    QStringList titlesForValues() const;

    /** Returns true if the specified string contains one of the enumeration keys for the handled
        property; otherwise returns false. */
    bool isValid(QString value) const;

    /** Sets the value of the handled property in the target item to the value corresponding to the
        specified enumeration key. If the specified key is invalid for this property, nothing
        happens. */
    void setValue(QString value);

    /** Returns the enumeration key corresponding to the value of the handled property in the
        target item. */
    QString value() const;

    /** Returns the title corresponding to the value of the handled property in the
        target item. If there is no title, the enumeration key is returned instead. */
    QString titleForValue() const;

    /** Returns true if the handled property has a valid default value, or false if not. */
    bool hasDefaultValue() const;

    /** Returns the enumeration key corresponding to the default value for the handled property, or
        the empty string if unavailable. */
    QString defaultValue() const;

    /** Returns true if the handled property has a "TrueIf" attribute, and if that attribute's
        value matches the current property value; otherwise returns false. */
    bool isTrueInCondition() const;

    /** Accepts the specified visitor. This function is part of the "visitor" design pattern
        implementation used to handle properties of various types. */
    void acceptVisitor(PropertyHandlerVisitor* visitor);
};

////////////////////////////////////////////////////////////////////

#endif // ENUMPROPERTYHANDLER_HPP
