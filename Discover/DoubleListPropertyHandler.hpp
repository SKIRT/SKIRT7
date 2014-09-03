/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef DOUBLELISTPROPERTYHANDLER_HPP
#define DOUBLELISTPROPERTYHANDLER_HPP

#include "PropertyHandler.hpp"

////////////////////////////////////////////////////////////////////

/** This class handles discoverable properties of type "QList<double>". A value of this type is
    externally represented as a single string containing a comma-separated list of decimal floating
    point numbers. Each number in the list can optionally be followed by a unit specification. */
class DoubleListPropertyHandler : public PropertyHandler
{
public:
    /** Constructs a property handler for the specified simulation item, without initializing any
        property attributes. The property's attributes must be added separately by calling
        addAttribute(). The property handler keeps a reference to the simulation item but doesn't
        assume ownership. The simulation item should not be deleted during the lifetime of the
        property handler. */
    explicit DoubleListPropertyHandler(SimulationItem* target);

    /** Sets the value of the handled property in the target item to the specified integer. */
    void setValue(QList<double> value);

    /** Returns the value of the handled property in the target item. */
    QList<double> value() const;

    /** Returns the minimum value for one of the items in the list held by the handled property. If
        no minimum value is specified in class info, the function returns negative infinity (can be
        tested with std::isinf()). */
    double minValue() const;

    /** Returns the maximum value for one of the items in the list held by the handled property. If
        no maximum value is specified in class info, the function returns positive infinity (can be
        tested with std::isinf()). */
    double maxValue() const;

    /** Returns the physical quantity name for items in the list held by the handled property, as
        described in the Units class, or the empty string if the items represent dimensionless
        quantities. */
    QString quantity() const;

    /** Accepts the specified visitor. This function is part of the "visitor" design pattern
        implementation used to handle properties of various types. */
    void acceptVisitor(PropertyHandlerVisitor* visitor);

public:
    /** Returns true if the specified string is non-empty and contains a comma separated list, with
        each item in the list representing a valid floating point number with an optional unit
        specification, according to the format described for the DoublePropertyHandler class.
        Otherwise returns false. */
    bool isValid(QString value) const;

    /** Returns the list of double values represented by the specified string, or the empty list if
        the string is empty. If any of the comma-separated items in the specified string are
        invalid, the corresponding item in the returned list is zero. See isValid() for more
        information. */
    QList<double> toDoubleList(QString value) const;

    /** Returns the double value represented by the specified string, or zero if the string is
        empty or contains an invalid representation. See isValid() for more information. */
    double toDouble(QString value) const;

    /** Returns a string representation of the specified list of double values, each item including
        an appropriate unit specification, and commas seperating the items. See isValid() for more
        information. */
    QString toString(QList<double> value) const;

    /** Returns a string representation of the specified double value, including an appropriate
        unit specification. See isValid() for more information. */
    QString toString(double value) const;
};

////////////////////////////////////////////////////////////////////

#endif // DOUBLELISTPROPERTYHANDLER_HPP
