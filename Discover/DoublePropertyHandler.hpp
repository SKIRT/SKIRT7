/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef DOUBLEPROPERTYHANDLER_HPP
#define DOUBLEPROPERTYHANDLER_HPP

#include "PropertyHandler.hpp"
class Units;

////////////////////////////////////////////////////////////////////

/** This class handles discoverable properties of type "double". A value of this type is externally
    represented as a decimal floating point number with an optional unit specification. */
class DoublePropertyHandler : public PropertyHandler
{
public:
    /** Constructs a property handler for the specified simulation item, without initializing any
        property attributes. The property's attributes must be added separately by calling
        addAttribute(). The property handler keeps a reference to the simulation item but doesn't
        assume ownership. The simulation item should not be deleted during the lifetime of the
        property handler. */
    explicit DoublePropertyHandler(SimulationItem* target);

    /** Sets the value of the handled property in the target item to the specified integer. */
    void setValue(double value);

    /** Returns the value of the handled property in the target item. */
    double value() const;

    /** Returns true if the handled property has a valid default value, or false if not. */
    bool hasDefaultValue() const;

    /** Returns the default value for the handled property, or zero if unavailable. */
    double defaultValue() const;

    /** Returns the minimum value for the handled property. If no minimum value is specified in
        class info, the function returns negative infinity (can be tested with std::isinf()). */
    double minValue() const;

    /** Returns the maximum value for the handled property. If no maximum value is specified in
        class info, the function returns positive infinity (can be tested with std::isinf()). */
    double maxValue() const;

    /** Returns the physical quantity name for the handled property, as described in the Units
        class, or the empty string if the handled property is a dimensionless quantity. If the \em
        raw argument is false or missing (i.e. the default behavior), and the value of the
        "quantity" class info attribute starts with an at sign, the quantity is determined instead
        as the string value of the indicated enumeration property. */
    QString quantity(bool raw = false) const;

    /** Accepts the specified visitor. This function is part of the "visitor" design pattern
        implementation used to handle properties of various types. */
    void acceptVisitor(PropertyHandlerVisitor* visitor);

public:
    /** Returns true if the specified string is non-empty and contains a valid string
        representation of a floating point number with an optional unit specification. Otherwise
        returns false. If present, the unit specification must follow the number, separated by one
        or more spaces. The allowed set of unit specifications is derived from the physical
        quantity attribute of the handled property; the default unit specification is determined
        from the Units subclass instance attached to the simulation hierarchy into which the
        handled property is connected. */
    bool isValid(QString value) const;

    /** Returns the double value represented by the specified string, or zero if the string is
        empty or contains an invalid representation. See isValid() for more information. */
    double toDouble(QString value) const;

    /** Returns a string representation of the specified double value, including an appropriate
        unit specification. See isValid() for more information. */
    QString toString(double value) const;

public:
    /** This is a static implementation of the isValid() function. It is provided here so that
        other classes can use it as well. */
    static bool isValidDoubleString(QString value, QString quantity, const Units* units);

    /** This is a static implementation of the toDouble() function. It is provided here so that
        other classes can use it as well. */
    static double convertStringtoDouble(QString value, QString quantity, const Units* units);

    /** This is a static implementation of the toString() function. It is provided here so that
        other classes can use it as well. */
    static QString convertDoubletoString(double value, QString quantity, const Units* units);
};

////////////////////////////////////////////////////////////////////

#endif // DOUBLEPROPERTYHANDLER_HPP
