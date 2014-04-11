/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef INTPROPERTYHANDLER_HPP
#define INTPROPERTYHANDLER_HPP

#include "PropertyHandler.hpp"

////////////////////////////////////////////////////////////////////

/** This class handles discoverable properties of type "int". */
class IntPropertyHandler : public PropertyHandler
{
public:
    /** Constructs a property handler for the specified simulation item, without initializing any
        property attributes. The property's attributes must be added separately by calling
        addAttribute(). The property handler keeps a reference to the simulation item but doesn't
        assume ownership. The simulation item should not be deleted during the lifetime of the
        property handler. */
    explicit IntPropertyHandler(SimulationItem* target);

    /** Sets the value of the handled property in the target item to the specified integer. */
    void setValue(int value);

    /** Returns the value of the handled property in the target item. */
    int value() const;

    /** Returns true if the handled property has a valid default value, or false if not. */
    bool hasDefaultValue() const;

    /** Returns the default value for the handled property, or zero if unavailable. */
    int defaultValue() const;

    /** Returns the minimum value for the handled property. If no minimum value is specified in
        class info, the function returns a default value close to the smallest representable
        integer (i.e. a negative integer with large absolute value). */
    int minValue() const;

    /** Returns the maximum value for the handled property. If no maximum value is specified in
        class info, the function returns a default value close to the largest representable
        integer. */
    int maxValue() const;

    /** Returns true if the value of the handled property is nonzero, and false if it is zero. */
    bool isTrueInCondition() const;

    /** Accepts the specified visitor. This function is part of the "visitor" design pattern
        implementation used to handle properties of various types. */
    void acceptVisitor(PropertyHandlerVisitor* visitor);

public:
    /** Returns true if the specified string is non-empty and contains a valid string
        representation of an integer that fits in 32 bits (signed). Otherwise returns false. */
    static bool isValid(QString value);

    /** Returns the integer value represented by the specified string, or zero if the string is
        empty or contains an invalid representation. */
    static int toInt(QString value);

    /** Returns a string representation of the specified integer value. */
    static QString toString(int value);
};

////////////////////////////////////////////////////////////////////

#endif // INTPROPERTYHANDLER_HPP
