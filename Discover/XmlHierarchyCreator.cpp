/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include <QTextStream>
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
#include "XmlHierarchyCreator.hpp"

using namespace SimulationItemDiscovery;

////////////////////////////////////////////////////////////////////

XmlHierarchyCreator::XmlHierarchyCreator()
{
}

////////////////////////////////////////////////////////////////////

SimulationItem* XmlHierarchyCreator::createHierarchy(QString filename, QByteArray topItemType)
{
    // open the file
    _file.setFileName(filename);
    if (!_file.open(QIODevice::ReadOnly))
        throw FATALERROR("File couldn't be opened for reading XML: " + filename);

    // setup the XML reader
    _reader.clear();
    _reader.setDevice(&_file);

    // actually create the hierarchy
    SimulationItem* result = createHierarchyFromReader(topItemType);

    // handle errors raised while reading the XML
    if (!result) throw FATALERROR("Error in XML file '" + filename + "' at line "
                                  + QString::number(_reader.lineNumber()) + "\n"
                                  + _reader.errorString());
    return result;
}

////////////////////////////////////////////////////////////////////

SimulationItem* XmlHierarchyCreator::createHierarchy(QByteArray content, QByteArray topItemType)
{
    // setup the XML reader
    _reader.clear();
    _reader.addData(content);

    // actually create the hierarchy
    SimulationItem* result = createHierarchyFromReader(topItemType);

    // handle errors raised while reading the XML
    if (!result) throw FATALERROR("Error in XML data at line " + QString::number(_reader.lineNumber()) + "\n"
                                  + _reader.errorString());
    return result;
}

////////////////////////////////////////////////////////////////////

SimulationItem* XmlHierarchyCreator::createHierarchyFromReader(QByteArray topItemType)
{
    // the hierarchy to be created
    SimulationItem* result = 0;

    // read the root element and verify the top item type
    if (!_reader.atEnd() && _reader.readNextStartElement())
    {
        QByteArray topName = "skirt-simulation-hierarchy";
        QByteArray rootName = _reader.name().toUtf8();
        if (rootName == topName)
        {
            QByteArray rootItemType = _reader.attributes().value("type").toUtf8();
            if (inherits(rootItemType, topItemType))
            {
                // create a simulation item from the next element
                result = createItemFromNextElement(0, rootItemType);
            }
            else _reader.raiseError("Root item type '" + rootItemType + "' does not inherit '" + topItemType + "'");
        }
        else _reader.raiseError("Root element is '" + rootName + "' rather than '" + topName + "'");

        // process the end of the root element
        if (!_reader.atEnd() && _reader.readNextStartElement())
        {
            _reader.raiseError("Unexpected element '" + _reader.name().toString() + "'");
        }
    }

    // handle errors raised while reading the XML
    if (_reader.hasError())
    {
        delete result;
        return 0;
    }
    else return result;
}

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

SimulationItem *XmlHierarchyCreator::createItemFromNextElement(SimulationItem* parent,
                                                               QByteArray baseItemType, bool optional)
{
    SimulationItem* result = 0;

    // read the next element and verify that the element type inherits from the base item type
    if (!_reader.atEnd() && _reader.readNextStartElement())
    {
        QByteArray itemType = _reader.name().toUtf8();
        if (inherits(itemType, baseItemType))
        {
            result = createSimulationItem(itemType);
            if (result)
            {
                // temporarily hook the new item into the simulation hierarchy so that the double property handler
                // can find the Units instance attached to the hierarchy -- this is somewhat of a hack...
                result->setParent(parent);

                // completing the properties for this simulation item poses a dual challenge:
                //  - we need to process scalar and compound properties in the order provided in the XML file
                //  - we need to ensure that all properties declared for the item are handled
                // to achieve this, we proceed as follows:
                //  - retrieve a dictionary of handlers for all declared properties
                //  - process the properties in the XML file
                //  - the handlers keep track of whether a value has been successfully set
                //  - process the "virgin" handlers to set default values or complain if there is no default

                // get a dictionary of handlers for all declared properties
                QHash<QByteArray, PropertyHandlerPtr> handlers = createPropertyHandlersDict(result);

                // process scalar properties (derived from XML attributes)
                foreach (QXmlStreamAttribute attribute, _reader.attributes())
                {
                    setScalarPropertyForAttribute(result, handlers, attribute);
                }
                // process compound properties (derived from XML child elements)
                while (!_reader.atEnd() && _reader.readNextStartElement())
                {
                    setCompoundPropertyForCurrentElement(result, handlers);
                }
                // honor default property values
                if (!_reader.atEnd()) foreach (PropertyHandlerPtr handler, handlers)
                {
                    if (!handler->hasChanged()) setDefaultPropertyValueWithHandler(result, handler);
                }

                // undo the temporary hook into the simulation hierarchy (see above)
                result->setParent(0);
            }
            else  _reader.raiseError("Simulation item of type '" + itemType + "' couldn't be created");
        }
        else _reader.raiseError("Simulation item type '" + itemType + "' does not inherit '" + baseItemType + "'");
    }
    else if (!optional) _reader.raiseError("Expected element for item type inheriting '" + baseItemType + "'");

    return result;
}

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

void XmlHierarchyCreator::setScalarPropertyForAttribute(SimulationItem* item,
                                                        QHash<QByteArray,PropertyHandlerPtr> handlers,
                                                        const QXmlStreamAttribute &attribute)
{
    QByteArray name = attribute.name().toUtf8();

    if (handlers.contains(name))
    {
        // ignore empty attribute values
        QString value = attribute.value().toString().trimmed();
        if (value.isEmpty()) return;

        // remove enclosing square brackets and corresponding label (used by FitSKIRT to tag attribute values)
        if (value.startsWith('[') && value.endsWith(']') && value.contains(':'))
        {
            value.chop(1);
            value = value.right(value.size()-value.indexOf(':')-1);
        }

        // set the value in the property
        ScalarPropertySetter setter(_reader, value);
        handlers.value(name)->acceptVisitor(&setter);
    }
    else _reader.raiseError("Simulation item of type '" + itemType(item) + "' has no property named '" + name + "'");
}

////////////////////////////////////////////////////////////////////

void XmlHierarchyCreator::ScalarPropertySetter::visitPropertyHandler(BoolPropertyHandler* handler)
{
    if (handler->isValid(_value))
    {
        handler->setValue(handler->toBool(_value));
    }
    else _reader.raiseError("Value '" + _value + "' for property '" + handler->name()
                            + "' can't be converted to bool");
}

////////////////////////////////////////////////////////////////////

void XmlHierarchyCreator::ScalarPropertySetter::visitPropertyHandler(IntPropertyHandler* handler)
{
    if (handler->isValid(_value))
    {
        int value = handler->toInt(_value);
        // silently clip the value to the specified range
        if (value < handler->minValue()) value = handler->minValue();
        if (value > handler->maxValue()) value = handler->maxValue();
        handler->setValue(value);
    }
    else _reader.raiseError("Value '" + _value + "' for property '" + handler->name()
                            + "' can't be converted to integer");
}

////////////////////////////////////////////////////////////////////

void XmlHierarchyCreator::ScalarPropertySetter::visitPropertyHandler(DoublePropertyHandler* handler)
{
    if (handler->isValid(_value))
    {
        double value = handler->toDouble(_value);
        // silently clip the value to the specified range
        if (value < handler->minValue()) value = handler->minValue();
        if (value > handler->maxValue()) value = handler->maxValue();
        handler->setValue(value);
    }
    else _reader.raiseError("Value '" + _value + "' for property '" + handler->name()
                            + "' can't be converted to double");
}

////////////////////////////////////////////////////////////////////

void XmlHierarchyCreator::ScalarPropertySetter::visitPropertyHandler(DoubleListPropertyHandler* handler)
{
    if (handler->isValid(_value))
    {
        QList<double> value = handler->toDoubleList(_value);
        // silently clip the values to the specified range
        for (int i = 0; i < value.size(); i++)
        {
            if (value[i] < handler->minValue()) value[i] = handler->minValue();
            if (value[i] > handler->maxValue()) value[i] = handler->maxValue();
        }
        handler->setValue(value);
    }
    else _reader.raiseError("Value '" + _value + "' for property '" + handler->name()
                            + "' can't be converted to list of doubles");
}

////////////////////////////////////////////////////////////////////

void XmlHierarchyCreator::ScalarPropertySetter::visitPropertyHandler(StringPropertyHandler* handler)
{
    handler->setValue(_value);
}

////////////////////////////////////////////////////////////////////

void XmlHierarchyCreator::ScalarPropertySetter::visitPropertyHandler(EnumPropertyHandler* handler)
{
    if (handler->isValid(_value))
    {
        handler->setValue(_value);
    }
    else _reader.raiseError("Value '" + _value + "' for property '" + handler->name()
                            + "' is an invalid enumeration key");
}

////////////////////////////////////////////////////////////////////

void XmlHierarchyCreator::ScalarPropertySetter::visitPropertyHandler(ItemPropertyHandler* handler)
{
    // unexpected property type
    _reader.raiseError("Property '" + handler->name() + "' has a compund data type and is given as an xml attribute");
}

////////////////////////////////////////////////////////////////////

void XmlHierarchyCreator::ScalarPropertySetter::visitPropertyHandler(ItemListPropertyHandler* handler)
{
    // unexpected property type
    _reader.raiseError("Property '" + handler->name() + "' has a compund data type and is given as an xml attribute");
}

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

void XmlHierarchyCreator::setCompoundPropertyForCurrentElement(SimulationItem* item,
                                                               QHash<QByteArray,
                                                               PropertyHandlerPtr> handlers)
{
    QByteArray name = _reader.name().toUtf8();

    if (handlers.contains(name))
    {
        CompoundPropertySetter setter(_reader, this);
        handlers.value(name)->acceptVisitor(&setter);
    }
    else _reader.raiseError("Simulation item of type '" + itemType(item) + "' has no property named '" + name + "'");
}

////////////////////////////////////////////////////////////////////

void XmlHierarchyCreator::CompoundPropertySetter::visitPropertyHandler(BoolPropertyHandler* handler)
{
    // unexpected property type
    _reader.raiseError("Property '" + handler->name() + "' has a scalar data type and is given as an xml element");
}

////////////////////////////////////////////////////////////////////

void XmlHierarchyCreator::CompoundPropertySetter::visitPropertyHandler(IntPropertyHandler* handler)
{
    // unexpected property type
    _reader.raiseError("Property '" + handler->name() + "' has a scalar data type and is given as an xml element");
}

////////////////////////////////////////////////////////////////////

void XmlHierarchyCreator::CompoundPropertySetter::visitPropertyHandler(DoublePropertyHandler* handler)
{
    // unexpected property type
    _reader.raiseError("Property '" + handler->name() + "' has a scalar data type and is given as an xml element");
}

////////////////////////////////////////////////////////////////////

void XmlHierarchyCreator::CompoundPropertySetter::visitPropertyHandler(DoubleListPropertyHandler* handler)
{
    // unexpected property type
    _reader.raiseError("Property '" + handler->name() + "' has a scalar data type and is given as an xml element");
}

////////////////////////////////////////////////////////////////////

void XmlHierarchyCreator::CompoundPropertySetter::visitPropertyHandler(StringPropertyHandler* handler)
{
    // unexpected property type
    _reader.raiseError("Property '" + handler->name() + "' has a scalar data type and is given as an xml element");
}

////////////////////////////////////////////////////////////////////

void XmlHierarchyCreator::CompoundPropertySetter::visitPropertyHandler(EnumPropertyHandler* handler)
{
    // unexpected property type
    _reader.raiseError("Property '" + handler->name() + "' has a scalar data type and is given as an xml element");
}

////////////////////////////////////////////////////////////////////

void XmlHierarchyCreator::CompoundPropertySetter::visitPropertyHandler(ItemPropertyHandler* handler)
{
    QByteArray baseType = _reader.attributes().value("type").toUtf8();
    if (baseType == handler->baseType())
    {
        handler->setValue(_boss->createItemFromNextElement(handler->target(), baseType));

        // process the end of the property element
        if (!_reader.atEnd() && _reader.readNextStartElement())
        {
            _reader.raiseError("Unexpected element '" + _reader.name().toString() + "'");
        }
    }
    else _reader.raiseError("Type '" + baseType + "' does not match base type '" + handler->baseType()
                            + "' for property '" + handler->name() + "'");
}

////////////////////////////////////////////////////////////////////

void XmlHierarchyCreator::CompoundPropertySetter::visitPropertyHandler(ItemListPropertyHandler* handler)
{
    QByteArray baseType = _reader.attributes().value("type").toUtf8();
    if (baseType == handler->baseType())
    {
        while (true)
        {
            // because we set optional=true, the end of the property element is processed
            // by createItemFromNextElement without generating an error
            SimulationItem* newItem = _boss->createItemFromNextElement(handler->target(), baseType, true);
            if (newItem) handler->addValue(newItem);
            else break;
        }
    }
    else _reader.raiseError("Type '" + baseType + "' does not match base type '" + handler->baseType()
                            + "' for property '" + handler->name() + "'");
}

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

void XmlHierarchyCreator::setDefaultPropertyValueWithHandler(SimulationItem* item, PropertyHandlerPtr handler)
{
    if (!handler->isOptional())
    {
        if (handler->hasDefaultValue())
        {
            DefaultPropertySetter setter(_reader);
            handler->acceptVisitor(&setter);
        }
        else _reader.raiseError("Value for property '" + handler->name() + "' in simulation item of type '"
                                + itemType(item) + "' is not specified and has no default value");
    }
}

////////////////////////////////////////////////////////////////////

void XmlHierarchyCreator::DefaultPropertySetter::visitPropertyHandler(BoolPropertyHandler* handler)
{
    handler->setValue(handler->defaultValue());
}

////////////////////////////////////////////////////////////////////

void XmlHierarchyCreator::DefaultPropertySetter::visitPropertyHandler(IntPropertyHandler* handler)
{
    handler->setValue(handler->defaultValue());
}

////////////////////////////////////////////////////////////////////

void XmlHierarchyCreator::DefaultPropertySetter::visitPropertyHandler(DoublePropertyHandler* handler)
{
    handler->setValue(handler->defaultValue());
}

////////////////////////////////////////////////////////////////////

void XmlHierarchyCreator::DefaultPropertySetter::visitPropertyHandler(DoubleListPropertyHandler* /*handler*/)
{
    // double list does not support default values
}

////////////////////////////////////////////////////////////////////

void XmlHierarchyCreator::DefaultPropertySetter::visitPropertyHandler(StringPropertyHandler* handler)
{
    handler->setValue(handler->defaultValue());
}

////////////////////////////////////////////////////////////////////

void XmlHierarchyCreator::DefaultPropertySetter::visitPropertyHandler(EnumPropertyHandler* handler)
{
    handler->setValue(handler->defaultValue());
}

////////////////////////////////////////////////////////////////////

void XmlHierarchyCreator::DefaultPropertySetter::visitPropertyHandler(ItemPropertyHandler* handler)
{
    handler->setValue(createSimulationItem(handler->defaultItemType()));
}

////////////////////////////////////////////////////////////////////

void XmlHierarchyCreator::DefaultPropertySetter::visitPropertyHandler(ItemListPropertyHandler* handler)
{
    handler->addValue(createSimulationItem(handler->defaultItemType()));
}

////////////////////////////////////////////////////////////////////
