/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef BOOLPROPERTYHANDLER_HPP
#define BOOLPROPERTYHANDLER_HPP

#include "PropertyHandler.hpp"

////////////////////////////////////////////////////////////////////

/** This class handles discoverable properties of type "bool". */
class BoolPropertyHandler : public PropertyHandler
{
public:
    /** Constructs a property handler for the specified simulation item, without initializing any
        property attributes. The property's attributes must be added separately by calling
        addAttribute(). The property handler keeps a reference to the simulation item but doesn't
        assume ownership. The simulation item should not be deleted during the lifetime of the
        property handler. */
    explicit BoolPropertyHandler(SimulationItem* target);

    /** Sets the value of the handled property in the target item to the specified integer. */
    void setValue(bool value);

    /** Returns the value of the handled property in the target item. */
    bool value() const;

    /** Returns true if the handled property has a valid default value, or false if not. */
    bool hasDefaultValue() const;

    /** Returns the default value for the handled property, or false if unavailable. */
    bool defaultValue() const;

    /** Returns the value of the handled property. */
    bool isTrueInCondition() const;

    /** Accepts the specified visitor. This function is part of the "visitor" design pattern
        implementation used to handle properties of various types. */
    void acceptVisitor(PropertyHandlerVisitor* visitor);

public:
    /** Returns true if the specified string is non-empty and contains a valid representation of a
        boolean. Otherwise returns false. After converting the specified string to lowercase, the
        following contents are considered valid representations: "true", "t", "yes", "y", "1" (for
        boolean true) and "false", "f", "no", "n", "0" (for boolean false). */
    static bool isValid(QString value);

    /** Returns the boolean value represented by the specified string, or false if the string is
        empty or contains an invalid representation. */
    static bool toBool(QString value);

    /** Returns the string "true" or "false" depending on the specified boolean value. */
    static QString toString(bool value);
};

////////////////////////////////////////////////////////////////////

#endif // BOOLPROPERTYHANDLER_HPP
