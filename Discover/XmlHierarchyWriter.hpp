/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef XMLHIERARCHYWRITER_HPP
#define XMLHIERARCHYWRITER_HPP

#include <QFile>
#include <QXmlStreamWriter>
#include "PropertyHandlerVisitor.hpp"
class SimulationItem;

////////////////////////////////////////////////////////////////////

/** This class writes the structure and properties of a simulation hierarchy to an XML file. The
    XML file contains sufficient information to reconstruct a fresh copy of the simulation
    hierarchy. The class inherits from PropertyHandlerVisitor to facilitate handling of the
    various types of properties. */
class XmlHierarchyWriter : public PropertyHandlerVisitor
{
public:
    /** Constructs an XmlHierarchyWriter instance without doing anything; call the
        writeHierarchy() function to actually write a simulation hierarchy to a file. */
    XmlHierarchyWriter();

    /** Writes the structure and properties of the specified simulation hierarchy to an XML file
        with the specified name (which may include a relative or absolute path). If an error
        occurs, the function throws a fatal error. */
    void writeHierarchy(SimulationItem* item, QString filename);

    /** Writes appropriate XML information on the specified property, as part of the visitor
        pattern initiated by the writeProperties() function. */
    void visitPropertyHandler(BoolPropertyHandler* handler);

    /** Writes appropriate XML information on the specified property, as part of the visitor
        pattern initiated by the writeProperties() function. */
    void visitPropertyHandler(IntPropertyHandler* handler);

    /** Writes appropriate XML information on the specified property, as part of the visitor
        pattern initiated by the writeProperties() function. */
    void visitPropertyHandler(DoublePropertyHandler* handler);

    /** Writes appropriate XML information on the specified property, as part of the visitor
        pattern initiated by the writeProperties() function. */
    void visitPropertyHandler(DoubleListPropertyHandler* handler);

    /** Writes appropriate XML information on the specified property, as part of the visitor
        pattern initiated by the writeProperties() function. */
    void visitPropertyHandler(StringPropertyHandler* handler);

    /** Writes appropriate XML information on the specified property, as part of the visitor
        pattern initiated by the writeProperties() function. */
    void visitPropertyHandler(EnumPropertyHandler* handler);

    /** Writes appropriate XML information on the specified property, as part of the visitor
        pattern initiated by the writeProperties() function. */
    void visitPropertyHandler(ItemPropertyHandler* handler);

    /** Writes appropriate XML information on the specified property, as part of the visitor
        pattern initiated by the writeProperties() function. */
    void visitPropertyHandler(ItemListPropertyHandler* handler);

private:
    /** Recursively writes the properties of the specified item and its children. This is
        accomplished by asking each of the properties to accept "this" PropertyHandlerVisitor
        instance as a visitor, which causes a call-back to the visitPropertyHandler() function with
        the appropriate PropertyHandler subtype. */
    void writeProperties(SimulationItem* item);

    // XML writer
    QFile _file;
    QXmlStreamWriter _writer;
};

////////////////////////////////////////////////////////////////////

#endif // XMLHIERARCHYWRITER_HPP
