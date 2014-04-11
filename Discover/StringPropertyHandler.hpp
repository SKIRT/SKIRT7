/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef STRINGPROPERTYHANDLER_HPP
#define STRINGPROPERTYHANDLER_HPP

#include "PropertyHandler.hpp"

////////////////////////////////////////////////////////////////////

/** This class handles discoverable properties of type "QString". */
class StringPropertyHandler : public PropertyHandler
{
public:
    /** Constructs a property handler for the specified simulation item, without initializing any
        property attributes. The property's attributes must be added separately by calling
        addAttribute(). The property handler keeps a reference to the simulation item but doesn't
        assume ownership. The simulation item should not be deleted during the lifetime of the
        property handler. */
    explicit StringPropertyHandler(SimulationItem* target);

    /** Sets the value of the handled property in the target item to the specified string. */
    void setValue(QString value);

    /** Returns the value of the handled property in the target item. */
    QString value() const;

    /** Returns true if the handled property has a valid default value, or false if not. */
    bool hasDefaultValue() const;

    /** Returns the default value for the handled property, or the empty string if unavailable. */
    QString defaultValue() const;

    /** Accepts the specified visitor. This function is part of the "visitor" design pattern
        implementation used to handle properties of various types. */
    void acceptVisitor(PropertyHandlerVisitor* visitor);
};

////////////////////////////////////////////////////////////////////

#endif // STRINGPROPERTYHANDLER_HPP
