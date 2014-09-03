/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef PROPERTYHANDLERVISITOR_HPP
#define PROPERTYHANDLERVISITOR_HPP

class BoolPropertyHandler;
class DoublePropertyHandler;
class DoubleListPropertyHandler;
class EnumPropertyHandler;
class IntPropertyHandler;
class ItemListPropertyHandler;
class ItemPropertyHandler;
class StringPropertyHandler;

////////////////////////////////////////////////////////////////////

/** This is the abstract base class for a visitor of property handlers. It is part of the "visitor"
    design pattern implementation used to handle properties of various types. */
class PropertyHandlerVisitor
{
protected:
    /** Constructs a property handler visitor. */
    PropertyHandlerVisitor();

public:
    /** Destructs the property handler visitor. This virtual destructor is declared here because
        PropertyHandlerVisitor is the top-level class in the hierarchy of property handler visitors. */
    virtual ~PropertyHandlerVisitor();

    /** Visits the specified property handler using the acceptVisitor() function specialization
        corresponding to the specified type. This function must be implemented in each subclass. */
    virtual void visitPropertyHandler(BoolPropertyHandler* handler) = 0;

    /** Visits the specified property handler using the acceptVisitor() function specialization
        corresponding to the specified type. This function must be implemented in each subclass. */
    virtual void visitPropertyHandler(IntPropertyHandler* handler) = 0;

    /** Visits the specified property handler using the acceptVisitor() function specialization
        corresponding to the specified type. This function must be implemented in each subclass. */
    virtual void visitPropertyHandler(DoublePropertyHandler* handler) = 0;

    /** Visits the specified property handler using the acceptVisitor() function specialization
        corresponding to the specified type. This function must be implemented in each subclass. */
    virtual void visitPropertyHandler(DoubleListPropertyHandler* handler) = 0;

    /** Visits the specified property handler using the acceptVisitor() function specialization
        corresponding to the specified type. This function must be implemented in each subclass. */
    virtual void visitPropertyHandler(StringPropertyHandler* handler) = 0;

    /** Visits the specified property handler using the acceptVisitor() function specialization
        corresponding to the specified type. This function must be implemented in each subclass. */
    virtual void visitPropertyHandler(EnumPropertyHandler* handler) = 0;

    /** Visits the specified property handler using the acceptVisitor() function specialization
        corresponding to the specified type. This function must be implemented in each subclass. */
    virtual void visitPropertyHandler(ItemPropertyHandler* handler) = 0;

    /** Visits the specified property handler using the acceptVisitor() function specialization
        corresponding to the specified type. This function must be implemented in each subclass. */
    virtual void visitPropertyHandler(ItemListPropertyHandler* handler) = 0;
};

////////////////////////////////////////////////////////////////////

#endif // PROPERTYHANDLERVISITOR_HPP
