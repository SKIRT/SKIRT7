/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include <QCoreApplication>
#include <QDateTime>
#include "BoolPropertyHandler.hpp"
#include "DoublePropertyHandler.hpp"
#include "DoubleListPropertyHandler.hpp"
#include "EnumPropertyHandler.hpp"
#include "FatalError.hpp"
#include "IntPropertyHandler.hpp"
#include "ItemListPropertyHandler.hpp"
#include "ItemPropertyHandler.hpp"
#include "SimulationItem.hpp"
#include "SimulationItemDiscovery.hpp"
#include "StringPropertyHandler.hpp"
#include "XmlHierarchyWriter.hpp"

using namespace SimulationItemDiscovery;

////////////////////////////////////////////////////////////////////

XmlHierarchyWriter::XmlHierarchyWriter()
{
}

////////////////////////////////////////////////////////////////////

void XmlHierarchyWriter::writeHierarchy(SimulationItem* item, QString filename)
{
    // open the file and setup the XML writer
    _file.setFileName(filename);
    if (!_file.open(QIODevice::WriteOnly | QIODevice::Text))
        throw FATALERROR("File couldn't be opened for writing XML: " + filename);
    _writer.setDevice(&_file);
    _writer.setAutoFormatting(true);

    // write document header and start root element
    _writer.writeStartDocument();
    _writer.writeComment("SKIRT radiative transfer simulations - © 2012-2014 Astronomical Observatory, Ghent University");
    _writer.writeStartElement("skirt-simulation-hierarchy");
    _writer.writeAttribute("type", itemBaseType(item));
    _writer.writeAttribute("format", "6.1");  // to be adjusted manually for incompatible changes
    _writer.writeAttribute("producer", QCoreApplication::applicationName() + " " + QCoreApplication::applicationVersion());
    _writer.writeAttribute("time", QDateTime::currentDateTime().toString("yyyy-MM-ddThh:mm:ss"));

    // recursively write all properties of the top-level item and its children
    writeProperties(item);

    // end root element and document
    _writer.writeEndElement();
    _writer.writeEndDocument();

    // close the file and check for errors
    _file.close();
    if (_writer.hasError())
        throw FATALERROR("An error occurred while writing XML: " + filename);
}

////////////////////////////////////////////////////////////////////

void XmlHierarchyWriter::writeProperties(SimulationItem* item)
{
    // start an element for the item
    _writer.writeStartElement(itemType(item));

    // handle all properties of the item
    foreach (PropertyHandlerPtr handler, createSortedPropertyHandlersList(item))
    {
        // distribute to write methods depending on property type (using visitor pattern)
        handler->acceptVisitor(this);
    }

    // end the element for the item
    _writer.writeEndElement();
}

////////////////////////////////////////////////////////////////////

void XmlHierarchyWriter::visitPropertyHandler(BoolPropertyHandler* handler)
{
    _writer.writeAttribute(handler->name(), handler->toString(handler->value()));
}

////////////////////////////////////////////////////////////////////

void XmlHierarchyWriter::visitPropertyHandler(IntPropertyHandler* handler)
{
    _writer.writeAttribute(handler->name(), handler->toString(handler->value()));
}

////////////////////////////////////////////////////////////////////

void XmlHierarchyWriter::visitPropertyHandler(DoublePropertyHandler* handler)
{
    _writer.writeAttribute(handler->name(), handler->toString(handler->value()));
}

////////////////////////////////////////////////////////////////////

void XmlHierarchyWriter::visitPropertyHandler(DoubleListPropertyHandler* handler)
{
    _writer.writeAttribute(handler->name(), handler->toString(handler->value()));
}

////////////////////////////////////////////////////////////////////

void XmlHierarchyWriter::visitPropertyHandler(StringPropertyHandler* handler)
{
    _writer.writeAttribute(handler->name(), handler->value());
}

////////////////////////////////////////////////////////////////////

void XmlHierarchyWriter::visitPropertyHandler(EnumPropertyHandler* handler)
{
    _writer.writeAttribute(handler->name(), handler->value());
}

////////////////////////////////////////////////////////////////////

void XmlHierarchyWriter::visitPropertyHandler(ItemPropertyHandler* handler)
{
    if (handler->value())
    {
        // start an element for the property
        _writer.writeStartElement(handler->name());
        _writer.writeAttribute("type", handler->baseType());

        // handle the item pointed to by the property
        writeProperties(handler->value());

        // end the element for the property
        _writer.writeEndElement();
    }
}

////////////////////////////////////////////////////////////////////

void XmlHierarchyWriter::visitPropertyHandler(ItemListPropertyHandler* handler)
{
    QList<SimulationItem*> items = handler->value();
    if (items.count())
    {
        // start an element for the property
        _writer.writeStartElement(handler->name());
        _writer.writeAttribute("type", handler->baseType());

        // handle the items pointed to by the property
        foreach(SimulationItem* item, items)
        {
            writeProperties(item);
        }

        // end the element for the property
        _writer.writeEndElement();
    }
}

////////////////////////////////////////////////////////////////////
