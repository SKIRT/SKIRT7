/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef XMLHIERARCHYCREATOR_HPP
#define XMLHIERARCHYCREATOR_HPP

#include <QFile>
#include <QHash>
#include <QXmlStreamReader>
#include "PropertyHandler.hpp"
#include "PropertyHandlerVisitor.hpp"
class QTextStream;
class SimulationItem;

////////////////////////////////////////////////////////////////////

/** This class creates a simulation hierarchy from an XML file with the appropriate structure, such
    as produced by the XmlHierarchyWriter class. The implementation uses three private
    PropertyHandlerVisitor subclasses (ScalarPropertySetter, CompoundPropertySetter, and
    DefaultPropertySetter) to facilitate handling of the various types of properties. */
class XmlHierarchyCreator
{
public:
    /** Constructs an XML hierarchy creator without doing anything; call the
        createHierarchy() function to actually create a simulation hierarchy. */
    XmlHierarchyCreator();

    /** Creates a fresh simulation hierarchy from the information in the XML file with the
        specified name (which may include a relative or absolute path), and returns a pointer to
        the top-most simulation item in the hierarchy (handing over ownership for the complete
        hierarchy to the caller). The template argument specifies the class name of the
        top-level simulation item to be returned. If the hierarchy can't be created due to some
        error condition, the function throws a fatal error. */
    template<class itemClass> itemClass* createHierarchy(QString filename);

    /** Creates a fresh simulation hierarchy from the information in the specified byte array, and
        returns a pointer to the top-most simulation item in the hierarchy (handing over ownership
        for the complete hierarchy to the caller). The template argument specifies the class name
        of the top-level simulation item to be returned. If the hierarchy can't be created due to
        some error condition, the function throws a fatal error. */
    template<class itemClass> itemClass* createHierarchy(QByteArray content);

private:
    /** This is the private version of the function to create a simulation hierarchy from a file; it is
        called from the public template function with the same name and first argument. */
    SimulationItem* createHierarchy(QString filename, QByteArray topItemType);

    /** This is the private version of the function to create a simulation hierarchy from a byte array; it is
        called from the public template function with the same name and first argument. */
    SimulationItem* createHierarchy(QByteArray content, QByteArray topItemType);

    /** This private function creates a simulation hierarchy from the data that is already loaded
        in the XML reader stored as a data member. */
    SimulationItem* createHierarchyFromReader(QByteArray topItemType);

    /** Recursively creates a simulation item (and its children) from the XML stream. This is
        accomplished by sequentially processing any scalar and compound properties provided in the
        XML file for the simulation item, while also verifying that all properties declared for the
        item have been handled, setting the appropriate defaults if not. */
    SimulationItem* createItemFromNextElement(SimulationItem* parent, QByteArray baseItemType, bool optional=false);

    /** Handles the scalar properties for the simulation item, specified in the XML file as XML
        attributes. This is accomplished by asking the relevant properties to accept a
        ScalarPropertySetter instance as a visitor, which causes a call-back to the
        ScalarPropertySetter::visitPropertyHandler() function with the appropriate PropertyHandler
        subtype. */
    void setScalarPropertyForAttribute(SimulationItem* item, QHash<QByteArray,PropertyHandlerPtr> handlers,
                                       const QXmlStreamAttribute& attribute);

    /** Handles the compound properties for the simulation item, specified in the XML file as
        nested XML elements. This is accomplished by asking the relevant properties to accept a
        CompoundPropertySetter instance as a visitor, which causes a call-back to the
        CompoundPropertySetter::visitPropertyHandler() function with the appropriate
        PropertyHandler subtype. */
    void setCompoundPropertyForCurrentElement(SimulationItem* item, QHash<QByteArray, PropertyHandlerPtr> handlers);

    /** Handles default values for the simulation item properties that are not specified in the XML
        file. This is accomplished by asking the relevant properties to accept a
        DefaultPropertySetter instance as a visitor, which causes a call-back to the
        DefaultPropertySetter::visitPropertyHandler() function with the appropriate PropertyHandler
        subtype. */
    void setDefaultPropertyValueWithHandler(SimulationItem* item, PropertyHandlerPtr handler);

    /** This is a private class defined in XmlHierarchyCreator. It sets the value of a
        scalar property by acting as part of the visitor pattern initiated by the
        XmlHierarchyCreator::setScalarPropertyForAttribute() function. */
    class ScalarPropertySetter : public PropertyHandlerVisitor
    {
    public:
        /** Constructs an instance. */
        ScalarPropertySetter(QXmlStreamReader& reader, QString value) : _reader(reader), _value(value) {}
        /** Sets the value for the specified scalar property. */
        void visitPropertyHandler(BoolPropertyHandler* handler);
        /** Sets the value for the specified scalar property. */
        void visitPropertyHandler(IntPropertyHandler* handler);
        /** Sets the value for the specified scalar property. */
        void visitPropertyHandler(DoublePropertyHandler* handler);
        /** Sets the value for the specified scalar property. */
        void visitPropertyHandler(DoubleListPropertyHandler* handler);
        /** Sets the value for the specified scalar property. */
        void visitPropertyHandler(StringPropertyHandler* handler);
        /** Sets the value for the specified scalar property. */
        void visitPropertyHandler(EnumPropertyHandler* handler);
        /** Raises an error for the specified compound property. */
        void visitPropertyHandler(ItemPropertyHandler* handler);
        /** Raises an error for the specified compound property. */
        void visitPropertyHandler(ItemListPropertyHandler* handler);
    private:
        QXmlStreamReader& _reader;
        QString _value;
    };

    /** This is a private class defined in XmlHierarchyCreator. It sets the value of a
        compound property by acting as part of the visitor pattern initiated by the
        XmlHierarchyCreator::setCompoundPropertyForCurrentElement() function. */
    class CompoundPropertySetter : public PropertyHandlerVisitor
    {
    public:
        /** Constructs an instance. */
        CompoundPropertySetter(QXmlStreamReader& reader, XmlHierarchyCreator* boss) : _reader(reader), _boss(boss) {}
        /** Raises an error for the specified scalar property. */
        void visitPropertyHandler(BoolPropertyHandler* handler);
        /** Raises an error for the specified scalar property. */
        void visitPropertyHandler(IntPropertyHandler* handler);
        /** Raises an error for the specified scalar property. */
        void visitPropertyHandler(DoublePropertyHandler* handler);
        /** Raises an error for the specified scalar property. */
        void visitPropertyHandler(DoubleListPropertyHandler* handler);
        /** Raises an error for the specified scalar property. */
        void visitPropertyHandler(StringPropertyHandler* handler);
        /** Raises an error for the specified scalar property. */
        void visitPropertyHandler(EnumPropertyHandler* handler);
        /** Sets the value for the specified compound property. */
        void visitPropertyHandler(ItemPropertyHandler* handler);
        /** Sets the value for the specified compound property. */
        void visitPropertyHandler(ItemListPropertyHandler* handler);
    private:
        QXmlStreamReader& _reader;
        XmlHierarchyCreator* _boss;
    };

    /** This is a private class defined in XmlHierarchyCreator. It sets the default value of a
        property by acting as part of the visitor pattern initiated by the
        XmlHierarchyCreator::setDefaultPropertyValueWithHandler() function. */
    class DefaultPropertySetter : public PropertyHandlerVisitor
    {
    public:
        /** Constructs an instance. */
        DefaultPropertySetter(QXmlStreamReader&) {}
        /** Sets the default value for the specified property. */
        void visitPropertyHandler(BoolPropertyHandler* handler);
        /** Sets the default value for the specified property. */
        void visitPropertyHandler(IntPropertyHandler* handler);
        /** Sets the default value for the specified property. */
        void visitPropertyHandler(DoublePropertyHandler* handler);
        /** Does nothing since this type doesn't support default values. */
        void visitPropertyHandler(DoubleListPropertyHandler* handler);
        /** Sets the default value for the specified property. */
        void visitPropertyHandler(StringPropertyHandler* handler);
        /** Sets the default value for the specified property. */
        void visitPropertyHandler(EnumPropertyHandler* handler);
        /** Sets the default value for the specified property. */
        void visitPropertyHandler(ItemPropertyHandler* handler);
        /** Sets the default value for the specified property. */
        void visitPropertyHandler(ItemListPropertyHandler* handler);
    };

    // XML reader
    QFile _file;
    QXmlStreamReader _reader;
};

////////////////////////////////////////////////////////////////////

// member template implementations
template<class itemClass> itemClass* XmlHierarchyCreator::createHierarchy(QString filename)
{
    return dynamic_cast<itemClass*>(createHierarchy(filename, itemClass::staticMetaObject.className()));
}
template<class itemClass> itemClass* XmlHierarchyCreator::createHierarchy(QByteArray content)
{
    return dynamic_cast<itemClass*>(createHierarchy(content, itemClass::staticMetaObject.className()));
}

////////////////////////////////////////////////////////////////////

#endif // XMLHIERARCHYCREATOR_HPP
