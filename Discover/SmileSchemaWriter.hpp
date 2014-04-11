/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef SMILESCHEMAWRITER_HPP
#define SMILESCHEMAWRITER_HPP

#include <QFile>
#include <QMap>
#include <QXmlStreamWriter>
#include "PropertyHandlerVisitor.hpp"
class PropertyHandler;

////////////////////////////////////////////////////////////////////

/** This class serves to generate a smile schema file describing the classes registered to the
    simulation item registry (see the SimulationItemRegistry class). The description includes the
    class inheritance tree and all properties defined for the classes using Q_CLASSINFO macros. */
class SmileSchemaWriter : PropertyHandlerVisitor
{
public:
    /** Constructs a SmileSchemaWriter instance without doing anything; call the writeSmileSchema()
        function to actually write a smile schema file. */
    SmileSchemaWriter();

    /** Writes a smile schema describing the classes registered to the simulation item registry to
        a file named "skirt.smile" to the current directory. If an error occurs, the function
        throws a fatal error. */
    void writeSmileSchema();

private:
    /** Writes the (compound) XML element for a single registered class. */
    void writeTypeElement(const QMetaObject* meta);

    /** Writes the (compound) XML element for a single property of a registered class. */
    void writePropertyElement(const QMetaObject* meta, QByteArray property);

    /** Writes appropriate information on the specified property, as part of the visitor pattern. */
    void visitPropertyHandler(BoolPropertyHandler* handler);

    /** Writes appropriate information on the specified property, as part of the visitor pattern. */
    void visitPropertyHandler(IntPropertyHandler* handler);

    /** Writes appropriate information on the specified property, as part of the visitor pattern. */
    void visitPropertyHandler(DoublePropertyHandler* handler);

    /** Writes appropriate information on the specified property, as part of the visitor pattern. */
    void visitPropertyHandler(DoubleListPropertyHandler* handler);

    /** Writes appropriate information on the specified property, as part of the visitor pattern. */
    void visitPropertyHandler(StringPropertyHandler* handler);

    /** Writes appropriate information on the specified property, as part of the visitor pattern. */
    void visitPropertyHandler(EnumPropertyHandler* handler);

    /** Writes appropriate information on the specified property, as part of the visitor pattern. */
    void visitPropertyHandler(ItemPropertyHandler* handler);

    /** Writes appropriate information on the specified property, as part of the visitor pattern. */
    void visitPropertyHandler(ItemListPropertyHandler* handler);

    /** Writes attributes on the specified property that are common to all property types. */
    void writeCommonPropertyAttributes(PropertyHandler* handler);

    /** Writes appropriate information on the specified physical quantity. */
    void writeQuantityElement(QString quantity);

    /** Writes appropriate information on the specified unit system. */
    void writeUnitSystemElement(QByteArray unitSystem);

private:
    // XML writer
    QFile _file;
    QXmlStreamWriter _writer;

    // the physical quantities used by any of the properties in the exported schema
    // (we use a QMap to keep the keys sorted; the value is not used)
    QMap<QString, int> _quantities;
};

////////////////////////////////////////////////////////////////////

#endif // SMILESCHEMAWRITER_HPP
