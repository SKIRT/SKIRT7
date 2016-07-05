/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include <cmath>
#include <QCoreApplication>
#include <QDateTime>
#include <QMetaClassInfo>
#include "BoolPropertyHandler.hpp"
#include "DoubleListPropertyHandler.hpp"
#include "DoublePropertyHandler.hpp"
#include "EnumPropertyHandler.hpp"
#include "FatalError.hpp"
#include "IntPropertyHandler.hpp"
#include "ItemListPropertyHandler.hpp"
#include "ItemPropertyHandler.hpp"
#include "OligoMonteCarloSimulation.hpp"
#include "SimulationItem.hpp"
#include "SimulationItemDiscovery.hpp"
#include "SimulationItemRegistry.hpp"
#include "SmileSchemaWriter.hpp"
#include "StringPropertyHandler.hpp"
#include "Units.hpp"

////////////////////////////////////////////////////////////////////

SmileSchemaWriter::SmileSchemaWriter()
{
}

////////////////////////////////////////////////////////////////////

void SmileSchemaWriter::writeSmileSchema()
{
    // get the list of all concrete classes, in order of addition to the registry
    QList<QByteArray> concreteTypes = SimulationItemRegistry::concreteItemTypes();

    // build a dictionary containing all classes, including the concrete classes
    // and all direct and indirect superclasses used by any of the concrete classes;
    // the keys in a QMap are ordered alphabetically
    QMap<QByteArray, const QMetaObject*> allTypes;
    foreach (QByteArray type, concreteTypes)
    {
        const QMetaObject* current = SimulationItemRegistry::metaObject(type);
        while (current)
        {
            allTypes.insert(current->className(), current);
            current = current->superClass();
        }
    }
    allTypes.remove("QObject");

    // open the file and setup the XML writer
    _file.setFileName("skirt.smile");
    if (!_file.open(QIODevice::WriteOnly | QIODevice::Text))
        throw FATALERROR("File couldn't be opened for writing XML: skirt.smile");
    _writer.setDevice(&_file);
    _writer.setAutoFormatting(true);

    // write document header and start root element
    _writer.writeStartDocument();
    _writer.writeComment("SKIRT radiative transfer simulations - © 2012-2014 Astronomical Observatory, Ghent University");
    _writer.writeStartElement("smile-schema");
    _writer.writeAttribute("type", "Schema");
    _writer.writeAttribute("format", "1.1");  // to be adjusted for incompatible changes to the schema language format
    _writer.writeAttribute("producer", QCoreApplication::applicationName() + " " + QCoreApplication::applicationVersion());
    _writer.writeAttribute("time", QDateTime::currentDateTime().toString("yyyy-MM-ddThh:mm:ss"));

    // start the "Schema" element
    _writer.writeStartElement("Schema");
    _writer.writeAttribute("name", "SKIRT");
    _writer.writeAttribute("title", "SKIRT parameter file");
    _writer.writeAttribute("version", "1.0");  // to be adjusted for incompatible changes to the schema definition
    _writer.writeAttribute("extension", "ski");
    _writer.writeAttribute("root", "skirt-simulation-hierarchy");
    _writer.writeAttribute("type", "MonteCarloSimulation");
    _writer.writeAttribute("format", "6.1");   // to be adjusted for incompatible changes to the SKIRT parameter format

    // clear the set of physical quantities; it is maintained in visitPropertyHandler(Double[List]PropertyHandler)
    _quantities.clear();

    // write a full "Type" element (including complete definitions) for each class, in alphabetical order
    _writer.writeStartElement("allTypes");
    _writer.writeAttribute("type", "Type");
    foreach (const QMetaObject* meta, allTypes.values())
    {
        writeTypeElement(meta);
    }
    _writer.writeEndElement();

    // write a brief "Type" element (just the name) for each concrete class, in order of addition to the registry
    _writer.writeStartElement("concreteTypes");
    _writer.writeAttribute("type", "Type");
    foreach (QByteArray type, concreteTypes)
    {
        _writer.writeStartElement("Type");
        _writer.writeAttribute("name", type);
        _writer.writeEndElement();
     }
    _writer.writeEndElement();

    // write a "Quantity" element for each physical quantity used by any of the properties in the exported schema
    _quantities.remove("");
    _writer.writeStartElement("quantities");
    _writer.writeAttribute("type", "Quantity");
    foreach (QString quantity, _quantities.keys())
    {
        writeQuantityElement(quantity);
    }
    _writer.writeEndElement();

    // write a "UnitSystem" element for each declared unit system
    _writer.writeStartElement("unitSystems");
    _writer.writeAttribute("type", "UnitSystem");
    foreach (QByteArray unitSystem, SimulationItemDiscovery::descendants("Units"))
    {
        writeUnitSystemElement(unitSystem);
    }
    _writer.writeEndElement();

    // end the "Schema" element
    _writer.writeEndElement();

    // end root element and document
    _writer.writeEndElement();
    _writer.writeEndDocument();

    // close the file and check for errors
    _file.close();
    if (_writer.hasError())
        throw FATALERROR("An error occurred while writing XML: skirt.smile");
}

////////////////////////////////////////////////////////////////////

void SmileSchemaWriter::writeTypeElement(const QMetaObject* meta)
{
    // start the "Type" element and write its name attribute
    _writer.writeStartElement("Type");
    _writer.writeAttribute("name", meta->className());

    // write the "base" attribute (empty string for top-level class)
    QByteArray base = meta->superClass()->className();
    if (base == "QObject") base = "";
    _writer.writeAttribute("base", base);

    // write the "title" attribute, if a title is found for this class
    for (int index = meta->classInfoOffset(); index < meta->classInfoCount(); index++)
    {
        QMetaClassInfo info = meta->classInfo(index);
        if (strcmp(info.name(), "Title") == 0)
        {
            _writer.writeAttribute("title", info.value());
            break;
        }
        if (strcmp(info.name(), "Property") == 0) break;
    }

    // write the "allowedIf" attribute, if one or more conditions are found for this class
    QByteArray compound;
    for (int index = meta->classInfoOffset(); index < meta->classInfoCount(); index++)
    {
        QMetaClassInfo info = meta->classInfo(index);
        if (strcmp(info.name(), "AllowedIf") == 0)
        {
            QByteArray condition = info.value();
            condition.replace(',','|');
            if (compound.isEmpty()) compound = condition;
            else if (compound.startsWith('(')) compound = compound + "&(" + condition + ")";
            else compound = "(" + compound + ")&(" + condition + ")";
        }
        if (strcmp(info.name(), "Property") == 0) break;
    }
    if (!compound.isEmpty()) _writer.writeAttribute("allowedIf", compound);

    // write a "Property" element for each property declared for this class, in order of declaration
    bool haveProperty = false;
    for (int index = meta->classInfoOffset(); index < meta->classInfoCount(); index++)
    {
        QMetaClassInfo info = meta->classInfo(index);
        if (strcmp(info.name(), "Property") == 0)
        {
            if (!haveProperty)
            {
                _writer.writeStartElement("properties");
                _writer.writeAttribute("type", "Property");
                haveProperty = true;
            }
            writePropertyElement(meta, info.value());
        }
    }
    if (haveProperty) _writer.writeEndElement();

    // end the "Type" element
    _writer.writeEndElement();
}

////////////////////////////////////////////////////////////////////

void SmileSchemaWriter::writePropertyElement(const QMetaObject *meta, QByteArray property)
{
    // create a temporary item to serve as target for a property handler
    QList<QByteArray> descendants = SimulationItemDiscovery::descendants(meta->className());
    if (descendants.empty()) throw FATALERROR("Could not find a concrete subtype for " + QString(meta->className()));
    SimulationItem* item = SimulationItemDiscovery::createSimulationItem(descendants[0]);
    if (!item) throw FATALERROR("Could not create a simulation item of type " + QString(descendants[0]));

    // hook the item into a temporary fake simulation item hierarchy to provide an SI units system
    QSharedPointer<Simulation> simulation(new OligoMonteCarloSimulation());
    item->setParent(simulation.data());

    // create a temporary property handler
    PropertyHandlerPtr handler = SimulationItemDiscovery::createPropertyHandler(item, property);
    if (!handler) throw FATALERROR("Could not handle property" + QString(property) +
                                   " for item of type " + QString(meta->className()));

    // distribute to write methods depending on property type (using visitor pattern)
    handler->acceptVisitor(this);
}

////////////////////////////////////////////////////////////////////

void SmileSchemaWriter::visitPropertyHandler(BoolPropertyHandler* handler)
{
    _writer.writeStartElement("BoolProperty");
    writeCommonPropertyAttributes(handler);
    if (handler->hasDefaultValue()) _writer.writeAttribute("default", handler->toString(handler->defaultValue()));
    _writer.writeEndElement();
}

////////////////////////////////////////////////////////////////////

void SmileSchemaWriter::visitPropertyHandler(IntPropertyHandler* handler)
{
    _writer.writeStartElement("IntProperty");
    writeCommonPropertyAttributes(handler);
    const int MAXINT = 2*1000*1000*1000;  // the value returned if no min/max value was specified
    if (handler->minValue() > -MAXINT) _writer.writeAttribute("min", handler->toString(handler->minValue()));
    if (handler->maxValue() <  MAXINT) _writer.writeAttribute("max", handler->toString(handler->maxValue()));
    if (handler->hasDefaultValue()) _writer.writeAttribute("default", handler->toString(handler->defaultValue()));
    _writer.writeEndElement();
}

////////////////////////////////////////////////////////////////////

void SmileSchemaWriter::visitPropertyHandler(DoublePropertyHandler* handler)
{
    _writer.writeStartElement("DoubleProperty");
    writeCommonPropertyAttributes(handler);
    _quantities.insert(handler->quantity(), 0);
    _writer.writeAttribute("quantity", handler->quantity());
    if (!std::isinf(handler->minValue())) _writer.writeAttribute("min", handler->toString(handler->minValue()));
    if (!std::isinf(handler->maxValue())) _writer.writeAttribute("max", handler->toString(handler->maxValue()));
    if (handler->hasDefaultValue()) _writer.writeAttribute("default", handler->toString(handler->defaultValue()));
    _writer.writeEndElement();
}

////////////////////////////////////////////////////////////////////

void SmileSchemaWriter::visitPropertyHandler(DoubleListPropertyHandler* handler)
{
    _writer.writeStartElement("DoubleListProperty");
    writeCommonPropertyAttributes(handler);
    _quantities.insert(handler->quantity(), 0);
    _writer.writeAttribute("quantity", handler->quantity());
    if (!std::isinf(handler->minValue())) _writer.writeAttribute("min", handler->toString(handler->minValue()));
    if (!std::isinf(handler->maxValue())) _writer.writeAttribute("max", handler->toString(handler->maxValue()));
    _writer.writeEndElement();
}

////////////////////////////////////////////////////////////////////

void SmileSchemaWriter::visitPropertyHandler(StringPropertyHandler* handler)
{
    _writer.writeStartElement("StringProperty");
    writeCommonPropertyAttributes(handler);
    if (handler->hasDefaultValue()) _writer.writeAttribute("default", handler->defaultValue());
    _writer.writeEndElement();
}

////////////////////////////////////////////////////////////////////

void SmileSchemaWriter::visitPropertyHandler(EnumPropertyHandler* handler)
{
    _writer.writeStartElement("EnumProperty");
    writeCommonPropertyAttributes(handler);
    if (handler->hasDefaultValue()) _writer.writeAttribute("default", handler->defaultValue());
    if (!handler->trueIf().isEmpty()) _writer.writeAttribute("trueIf", handler->trueIf());

    // write an element for each enumeration value
    _writer.writeStartElement("enumValues");
    _writer.writeAttribute("type", "EnumValue");
    QStringList values = handler->values();
    QStringList titles = handler->titlesForValues();
    for (int i = 0; i < values.size(); i++)
    {
        _writer.writeStartElement("EnumValue");
        _writer.writeAttribute("name", values[i]);
        _writer.writeAttribute("title", titles[i]);
        _writer.writeEndElement();
    }
    _writer.writeEndElement();

    _writer.writeEndElement();
}

////////////////////////////////////////////////////////////////////

void SmileSchemaWriter::visitPropertyHandler(ItemPropertyHandler* handler)
{
    _writer.writeStartElement("ItemProperty");
    writeCommonPropertyAttributes(handler);
    _writer.writeAttribute("base", handler->baseType());
    if (handler->hasDefaultValue()) _writer.writeAttribute("default", handler->defaultItemType());
    _writer.writeEndElement();
}

////////////////////////////////////////////////////////////////////

void SmileSchemaWriter::visitPropertyHandler(ItemListPropertyHandler* handler)
{
    _writer.writeStartElement("ItemListProperty");
    writeCommonPropertyAttributes(handler);
    _writer.writeAttribute("base", handler->baseType());
    if (handler->hasDefaultValue()) _writer.writeAttribute("default", handler->defaultItemType());
    _writer.writeEndElement();
}

////////////////////////////////////////////////////////////////////

void SmileSchemaWriter::writeCommonPropertyAttributes(PropertyHandler* handler)
{
    _writer.writeAttribute("name", handler->name());
    _writer.writeAttribute("title", handler->title());
    if (handler->isSilent()) _writer.writeAttribute("silent", "true");
    if (handler->isOptional()) _writer.writeAttribute("optional", "true");
    QByteArray relevant = handler->isRelevantPropertyName();
    if (!relevant.isEmpty()) _writer.writeAttribute("relevantIf", relevant);
}

////////////////////////////////////////////////////////////////////

void SmileSchemaWriter::writeQuantityElement(QString quantity)
{
    // start the "Quantity" element and write its name attribute
    _writer.writeStartElement("Quantity");
    _writer.writeAttribute("name", quantity);

    // write an element for each unit supported by this quantity
    _writer.writeStartElement("units");
    _writer.writeAttribute("type", "Unit");
    foreach (QString unit, Units::units(quantity))
    {
        _writer.writeStartElement("Unit");
        _writer.writeAttribute("name", unit);
        _writer.writeAttribute("factor", QString::number(Units::in(quantity, unit), 'g', 15));
        _writer.writeEndElement();
    }
    _writer.writeEndElement();

    // end the "Quantity" element
    _writer.writeEndElement();
}

////////////////////////////////////////////////////////////////////

void SmileSchemaWriter::writeUnitSystemElement(QByteArray unitSystem)
{
    // start the "UnitSystem" element and write its name attribute
    _writer.writeStartElement("UnitSystem");
    _writer.writeAttribute("name", unitSystem);

    // create a temporary unit system item of the appropriate type
    QSharedPointer<Units> units(SimulationItemDiscovery::createSimulationItem(unitSystem)->find<Units>());
    if (!units) throw FATALERROR("Could not create a simulation item of type " + QString(unitSystem));

    // write an element for each quantity, listing the default unit for the quantity in this unit system
    _writer.writeStartElement("defaultUnits");
    _writer.writeAttribute("type", "DefaultUnit");
    foreach (QString quantity, _quantities.keys())
    {
        _writer.writeStartElement("DefaultUnit");
        _writer.writeAttribute("quantity", quantity);
        _writer.writeAttribute("unit", units->unit(quantity));
        _writer.writeEndElement();
    }
    _writer.writeEndElement();

    // end the "UnitSystem" element
    _writer.writeEndElement();
}

////////////////////////////////////////////////////////////////////
