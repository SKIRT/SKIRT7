/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef PROPERTYHANDLER_HPP
#define PROPERTYHANDLER_HPP

#include <QByteArray>
#include <QHash>
#include <QSharedPointer>
class PropertyHandlerVisitor;
class SimulationItem;

////////////////////////////////////////////////////////////////////

/** This is the abstract base class for handling a discoverable property in a simulation item. */
class PropertyHandler
{
protected:
    /** Constructs a property handler for the specified simulation item, without initializing any
        property attributes. The property's attributes must be added separately by calling
        addAttribute(). The property handler keeps a reference to the simulation item but doesn't
        assume ownership. The simulation item should not be deleted during the lifetime of the
        property handler. */
    explicit PropertyHandler(SimulationItem* target);

public:
    /** Destructs the property handler. This virtual destructor is declared here because
        PropertyHandler is the top-level class in the hierarchy of property handlers. */
    virtual ~PropertyHandler();

    /** Adds a property attribute with the specified key and value, as declared in class info.
        These attributes must be added before the property handler is put to use. */
    void addAttribute(QByteArray key, QByteArray value);

    /** Returns the simulation item for which a property is being handled. */
    SimulationItem* target() const;

    /** Returns the name of the handled property. */
    QByteArray name() const;

    /** Returns the type for the handled property. */
    QByteArray type() const;

    /** Returns the title (used for display to a user) for the handled property. */
    QString title() const;

    /** Returns true if the handled property is silent, i.e. an interactive Q&A session should
        not ask for a value corresponding to this property. A silent property must have an
        appropriate default value. */
    bool isSilent() const;

    /** Returns true if the handled property is optional (i.e. its value may remain unset), or
        false if not. The default implementation in this abstract class returns false; this
        function must be overridden by property handler subclasses that support optional values. */
    virtual bool isOptional() const;

    /** Returns true if the handled property has a valid default value, or false if not. The
        default implementation in this abstract class returns false; this function must be
        overridden by property handler subclasses that may offer a default value. */
    virtual bool hasDefaultValue() const;

    /** Returns true if the value of the target item has been modified by this handler; otherwise
        returns false. */
    bool hasChanged() const;

    /** Returns true if the value of the handled property would test as true in a condition, and
        false if it would test as false or if the property type can't be used in a condition. For
        example a boolean property would simply return its value; an integer property would return
        true if its value is nonzero. This function is used by isRelevant() to test a simple
        condition on the value of a property in the current simulation hierarchy in a way that
        doesn't depend on the property type. The default implementation provided in this abstract
        class always returns false; this function must be overridden by property handler subclasses
        that support condition testing. */
    virtual bool isTrueInCondition() const;

    /** Returns true if the handled property is relevant for the current simulation hierarchy, and
        false otherwise. A property that does not contain the "RelevantIf" attribute is always
        relevant. A property that contains the "RelevantIf" attribute is relevant if the property
        named in the attribute value can be found for the item being handled, is relevant in its
        own right (as determined by a recursive call to isRelevant()), and has a value that would
        test as true in a condition (as determined by isTrueInCondition()). */
    bool isRelevant() const;

    /** Returns the name of the property specified in the "RelevantIf" attribute value for the
        handled property, or the empty byte array if the handled property does not contain the
        "RelevantIf" attribute. */
    QByteArray isRelevantPropertyName() const;

    /** Returns a fixed value used to sort properties on type. */
    virtual int sortIndex() const;

    /** Accepts the specified visitor. This function is part of the "visitor" design pattern
        implementation used to handle properties of various types. */
    virtual void acceptVisitor(PropertyHandlerVisitor* visitor);

    /** A shorthand type definition for a PropertyHandler pointer with auto-delete. */
    typedef QSharedPointer<PropertyHandler> PropertyHandlerPtr;

protected:
    /** Returns the name of the setter for the handled property. */
    QByteArray setter() const;

    /** Returns the name of the adder for the handled property. */
    QByteArray adder() const;

    /** Returns the name of the getter for the handled property. */
    QByteArray getter() const;

protected:
    // the simulation item being handled
    SimulationItem* _target;

    // becomes true if the value of the target item has been modified by this handler
    bool _changed;

    // the key/value pairs loaded from classinfo
    QHash<QByteArray,QByteArray> _attributes;
};

////////////////////////////////////////////////////////////////////

// copy this typedef in the global namespace for ease of use
typedef PropertyHandler::PropertyHandlerPtr PropertyHandlerPtr;

////////////////////////////////////////////////////////////////////

#endif // PROPERTYHANDLER_HPP
