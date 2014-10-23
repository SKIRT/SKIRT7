/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef ITEMLISTPROPERTYHANDLER_HPP
#define ITEMLISTPROPERTYHANDLER_HPP

#include "PropertyHandler.hpp"
#include <QList>

////////////////////////////////////////////////////////////////////

/** This class handles discoverable properties of type "QList of pointer to simulation item
    subclass". */
class ItemListPropertyHandler : public PropertyHandler
{
public:
    /** Constructs a property handler for the specified simulation item, without initializing any
        property attributes. The property's attributes must be added separately by calling
        addAttribute(). The property handler keeps a reference to the simulation item but doesn't
        assume ownership. The simulation item should not be deleted during the lifetime of the
        property handler. */
    explicit ItemListPropertyHandler(SimulationItem* target);

    /** Returns the type of the simulation item being pointed to by the handled property, i.e. the
        part of type() inside the triangle brackets and without the trailing asterisk. */
    QByteArray baseType() const;

    /** Returns the pointer type in the list held by the handled property, i.e. the part of type()
        inside the triangle brackets including the trailing asterisk. */
    QByteArray ptrType() const;

    /** Adds the specified simulation item to the list held by the handled property in the target
        item. The target item assumes ownership of the specified instance. The function returns
        false if the item couldn't be added (e.g. because it has an inappropriate type). */
    bool addValue(SimulationItem* value);

    /** Constructs a new instance of the specified simulation item type and adds it to the list
        held by the handled property in the target item. The target item assumes ownership of the
        new instance. The function returns false if a new item couldn't be added (e.g. because the
        specified item type is inappropriate). */
    bool addNewItemOfType(QByteArray itemType);

    /** Inserts the specified simulation item at the specified index into the list held by the
        handled property in the target item. The target item assumes ownership of the specified
        instance. The function returns false if the item couldn't be added (e.g. because it has an
        inappropriate type). */
    bool insertValue(int index, SimulationItem* value);

    /** Constructs a new instance of the specified simulation item type and inserts it at the
        specified index into the list held by the handled property in the target item. The target
        item assumes ownership of the new instance. The function returns false if a new item
        couldn't be added (e.g. because the specified item type is inappropriate). */
    bool insertNewItemOfType(int index, QByteArray itemType);

    /** Removes the simulation item with the specified zero-based index from the list held by the
        handled property in the target item. The removed simulation item is deleted. The function
        returns false if the item couldn't be removed (e.g. because the index is out of range). */
    bool removeValueAt(int index);

    /** Returns the value of the handled property in the target item. */
    QList<SimulationItem*> value() const;

    /** Returns true if the handled property is optional (i.e. its value may be an empty list), or
        false if not. */
    bool isOptional() const;

    /** Returns true if the handled property has a valid default value, or false if not. */
    bool hasDefaultValue() const;

    /** Returns the default item type for the handled property, or empty if unavailable. */
    QByteArray defaultItemType() const;

    /** Returns true if the list held by the handled property is not empty, and false if it is empty. */
    bool isTrueInCondition() const;

    /** Returns a fixed value used to sort properties on type. */
    int sortIndex() const;

    /** Accepts the specified visitor. This function is part of the "visitor" design pattern
        implementation used to handle properties of various types. */
    void acceptVisitor(PropertyHandlerVisitor* visitor);

private:
    /** Inserts the specified pointer at the specified index into the list held by the handled
        property in the target item. The target item assumes ownership of the new instance. */
    bool insertPlainValue(int index, QObject* value);
};

////////////////////////////////////////////////////////////////////

#endif // ITEMLISTPROPERTYHANDLER_HPP
