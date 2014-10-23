/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef ITEMPROPERTYHANDLER_HPP
#define ITEMPROPERTYHANDLER_HPP

#include "PropertyHandler.hpp"

////////////////////////////////////////////////////////////////////

/** This class handles discoverable properties of type "pointer to simulation item subclass". */
class ItemPropertyHandler : public PropertyHandler
{
public:
    /** Constructs a property handler for the specified simulation item, without initializing any
        property attributes. The property's attributes must be added separately by calling
        addAttribute(). The property handler keeps a reference to the simulation item but doesn't
        assume ownership. The simulation item should not be deleted during the lifetime of the
        property handler. */
    explicit ItemPropertyHandler(SimulationItem* target);

    /** Returns the type of the simulation item being pointed to by the handled property, i.e. the
        return value of type() without the trailing asterisk. */
    QByteArray baseType() const;

    /** Sets the value of the handled property in the target item so that it points to the
        specified simulation item. The target item assumes ownership of the specified instance. The
        function returns false if the property couldn't be set (e.g. because the specified value
        has an inappropriate type). */
    bool setValue(SimulationItem* value);

    /** Constructs a new instance of the specified simulation item type and sets the value of the
        handled property in the target item so that it points to this new instance. The target item
        assumes ownership of the new instance. The function returns false if the property couldn't
        be set (e.g. because the specified item type is inappropriate). */
    bool setToNewItemOfType(QByteArray itemType);

    /** Sets the value of the handled property in the target item to a null pointer, removing any
        previously owned simulation item. */
    void setToNull();

    /** Returns the value of the handled property in the target item. */
    SimulationItem* value() const;

    /** Returns true if the handled property is optional (i.e. its value may a null pointer), or
        false if not. */
    bool isOptional() const;

    /** Returns true if the handled property has a valid default value, or false if not. */
    bool hasDefaultValue() const;

    /** Returns the default item type for the handled property, or empty if unavailable. */
    QByteArray defaultItemType() const;

    /** Returns true if the value of the handled property is not null, and false if it is null. */
    bool isTrueInCondition() const;

    /** Returns a fixed value used to sort properties on type. */
    int sortIndex() const;

    /** Accepts the specified visitor. This function is part of the "visitor" design pattern
        implementation used to handle properties of various types. */
    void acceptVisitor(PropertyHandlerVisitor* visitor);

private:
    /** Sets the value of the handled property in the target item to the specified pointer. */
    bool setPlainValue(QObject* value);
};

////////////////////////////////////////////////////////////////////

#endif // ITEMPROPERTYHANDLER_HPP
