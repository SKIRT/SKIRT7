/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include "BoolPropertyHandler.hpp"
#include "ConsoleHierarchyCreator.hpp"
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

using namespace SimulationItemDiscovery;

////////////////////////////////////////////////////////////////////

ConsoleHierarchyCreator::ConsoleHierarchyCreator()
{
}

////////////////////////////////////////////////////////////////////

SimulationItem* ConsoleHierarchyCreator::createHierarchy(QByteArray topItemType)
{
    // make the user select the appropriate subclass of the top-level item type
    QList<QByteArray> choices = descendants(topItemType);
    int choice = promptForChoice(title(topItemType), titles(choices));

    // create the top-level item
    SimulationItem* topItem = createSimulationItem(choices[choice]);
    if (!topItem) throw FATALERROR("Can't create simulation item of type " + choices[choice]);
    foreach (QByteArray key, ascendants(choices[choice])) _keys << key;

    // recursively setup all properties of the top-level item and its children
    setupProperties(topItem);
    return topItem;
}

////////////////////////////////////////////////////////////////////

void ConsoleHierarchyCreator::setupProperties(SimulationItem* item)
{
    // setup all properties of the item
    foreach (PropertyHandlerPtr handler, createPropertyHandlersList(item))
    {
        // distribute to setup methods depending on property type (using visitor pattern)
        if (handler->isRelevant()) handler->acceptVisitor(this);
    }
}

////////////////////////////////////////////////////////////////////

void ConsoleHierarchyCreator::visitPropertyHandler(BoolPropertyHandler *handler)
{
    bool value = promptForBool("Do you want to " + handler->title() + "?",
                               handler->hasDefaultValue(), handler->defaultValue() );
    handler->setValue(value);
}

////////////////////////////////////////////////////////////////////

void ConsoleHierarchyCreator::visitPropertyHandler(IntPropertyHandler *handler)
{
    int value = promptForInt("Enter " + handler->title(), handler->minValue(), handler->maxValue(),
                             handler->hasDefaultValue(), handler->defaultValue() );
    handler->setValue(value);
}

////////////////////////////////////////////////////////////////////

void ConsoleHierarchyCreator::visitPropertyHandler(DoublePropertyHandler *handler)
{
    double value = promptForDouble("Enter " + handler->title(), handler->minValue(), handler->maxValue(),
                                   handler->hasDefaultValue(), handler->defaultValue(), handler );
    handler->setValue(value);
}

////////////////////////////////////////////////////////////////////

void ConsoleHierarchyCreator::visitPropertyHandler(DoubleListPropertyHandler *handler)
{
    QList<double> value = promptForDoubleList("Enter " + handler->title(), handler->minValue(), handler->maxValue(),
                                              handler );
    handler->setValue(value);
}

////////////////////////////////////////////////////////////////////

void ConsoleHierarchyCreator::visitPropertyHandler(StringPropertyHandler *handler)
{
    QString value = promptForString("Enter " + handler->title(), handler->hasDefaultValue(), handler->defaultValue());
    handler->setValue(value);
}

////////////////////////////////////////////////////////////////////

void ConsoleHierarchyCreator::visitPropertyHandler(EnumPropertyHandler *handler)
{
    QStringList keys = handler->values();
    int choice = promptForChoice(handler->title(), handler->titlesForValues(), handler->hasDefaultValue(),
                                 keys.indexOf(handler->defaultValue()));
    handler->setValue(keys[choice]);
}

////////////////////////////////////////////////////////////////////

void ConsoleHierarchyCreator::visitPropertyHandler(ItemPropertyHandler *handler)
{
    // make the user select the appropriate subclass for this property
    QList<QByteArray> choices = allowedDescendants(handler->baseType(), _keys);
    int choice = promptForChoice(handler->title(), titles(choices), handler->hasDefaultValue(),
                                 choices.indexOf(handler->defaultItemType()),
                                 handler->isOptional(), "or zero to select none");

    // don't set the property value if so requested
    if (choice < 0) return;

    // create the item and set the property
    bool success = handler->setToNewItemOfType(choices[choice]);
    if (!success) throw FATALERROR("Can't create simulation item of type " + choices[choice]);
    foreach (QByteArray key, ascendants(choices[choice])) _keys << key;

    // recursively handle the newly created simulation item
    setupProperties(handler->value());
}

////////////////////////////////////////////////////////////////////

void ConsoleHierarchyCreator::visitPropertyHandler(ItemListPropertyHandler *handler)
{
    // get a list of the subclasses for this property
    QList<QByteArray> choices = allowedDescendants(handler->baseType(), _keys);

    // loop to create list of new items
    for (int count = 1; ; count++)
    {
        // make the user select the appropriate subclass for this item
        int choice = promptForChoice("item #" + QString::number(count) + " in " + handler->title() + " list",
                                     titles(choices), handler->hasDefaultValue(),
                                     choices.indexOf(handler->defaultItemType()),
                                     count != 1  || handler->isOptional(), "or zero to terminate the list");

        // terminate the list if requested
        if (choice < 0) return;

        // create the item and set the property
        bool success = handler->addNewItemOfType(choices[choice]);
        if (!success) throw FATALERROR("Can't create simulation item of type " + choices[choice]);
        foreach (QByteArray key, ascendants(choices[choice])) _keys << key;

        // recursively handle the newly created simulation item
        setupProperties(handler->value().last());
    }
}

////////////////////////////////////////////////////////////////////

bool ConsoleHierarchyCreator::promptForBool(QString message, bool hasDef, bool def)
{
    // add hints on possible and default values to the message
    message += " [yes/no]";
    if (hasDef) message += def ? " (yes)" : " (no)";

    while (true)
    {
        // get the input string
        QString input = _console.promptForInput(message);

        // if provided, use the default value instead of an empty string
        if (input.isEmpty() && hasDef) return def;

        // if successful conversion, return result
        if (BoolPropertyHandler::isValid(input)) return BoolPropertyHandler::toBool(input);

        // reject conversion errors or empty strings without default
        _console.error("Enter 'yes' or 'no'");
    }
}

////////////////////////////////////////////////////////////////////

int ConsoleHierarchyCreator::promptForInt(QString message, int min, int max, bool hasDef, int def)
{
    // verify that default is in range
    if (hasDef && (def<min || def>max)) throw FATALERROR("Default value out of range");

    // add hints on min, max and default values to the message
    message += " [" + IntPropertyHandler::toString(min) + "," + IntPropertyHandler::toString(max) + "]";
    if (hasDef) message += " (" + IntPropertyHandler::toString(def) + ")";

    while (true)
    {
        // get the input string and attempt conversion
        QString input = _console.promptForInput(message);
        int result = IntPropertyHandler::toInt(input);

        // if provided, use the default value instead of an empty string
        if (input.isEmpty() && hasDef) return def;

        // reject conversion errors or empty strings without default
        if (!IntPropertyHandler::isValid(input))
        {
            _console.error("Enter a valid integer number");
        }

        // reject out-of-range values
        else if (result<min)
        {
            _console.error("Enter a number larger than or equal to " + IntPropertyHandler::toString(min));
        }
        else if (result>max)
        {
            _console.error("Enter a number smaller than or equal to " + IntPropertyHandler::toString(max));
        }
        // return successful result
        else
        {
            return result;
        }
    }
}

////////////////////////////////////////////////////////////////////

double ConsoleHierarchyCreator::promptForDouble(QString message, double min, double max, bool hasDef, double def,
                                                DoublePropertyHandler* handler)
{
    // verify that default is in range
    if (hasDef && (def<min || def>max)) throw FATALERROR("Default value out of range");

    // add hints on min, max and default values to the message
    message += " [" + handler->toString(min) + "," + handler->toString(max) + "]";
    if (hasDef) message += " (" + handler->toString(def) + ")";

    while (true)
    {
        // get the input string and attempt conversion
        QString input = _console.promptForInput(message);
        double result = handler->toDouble(input);

        // if provided, use the default value instead of an empty string
        if (input.isEmpty() && hasDef) return def;

        // reject conversion errors or empty strings without default
        if (!handler->isValid(input))
        {
            _console.error("Enter a valid floating point number, optionally followed by a space and a unit string");
        }

        // reject out-of-range values
        else if (result<min)
        {
            _console.error("Enter a number larger than or equal to " + handler->toString(min));
        }
        else if (result>max)
        {
            _console.error("Enter a number smaller than or equal to " + handler->toString(max));
        }
        // return successful result
        else
        {
            return result;
        }
    }
}

////////////////////////////////////////////////////////////////////

QList<double> ConsoleHierarchyCreator::promptForDoubleList(QString message, double min, double max,
                                                           DoubleListPropertyHandler* handler)
{
    // add hint on min and max values to the message
    QString hint = " [" + handler->toString(min) + "," + handler->toString(max) + "]";
    message += hint;

    while (true)
    {
        // get the input string
        QString input = _console.promptForInput(message);

        // reject conversion errors or empty strings
        if (!handler->isValid(input))
        {
            _console.error("Enter a comma-separated list of floating point numbers, "
                           "each optionally followed by a space and a unit string");
        }

        // reject out-of-range values
        else
        {
            // convert the list of numbers
            QList<double> result = handler->toDoubleList(input);

            // verify that all numbers are within range
            bool inrange = true;
            foreach (double number, result) if (number < min || number > max) inrange = false;
            if (!inrange)
            {
                _console.error("Enter numbers in range" + hint);
            }

            // return successful result
            else
            {
                return result;
            }
        }
    }
}

////////////////////////////////////////////////////////////////////

QString ConsoleHierarchyCreator::promptForString(QString message, bool hasDef, QString def)
{
    // add hint on default value to the message
    if (hasDef) message += " (" + def + ")";

    while (true)
    {
        // get the input string
        QString input = _console.promptForInput(message);

        // accept non-empty string
        if (!input.isEmpty()) return input;

        // if provided, use the default value instead of an empty string
        if (hasDef) return def;

        // reject empty strings
        _console.error("Enter a nonempty string");
    }
}

////////////////////////////////////////////////////////////////////

int ConsoleHierarchyCreator::promptForChoice(QString message, QStringList choices, bool hasDef, int defIndex,
                                             bool allowNoChoice, QString noChoiceMessage)
{
    if (choices.empty()) throw FATALERROR("There are no choices to prompt for");
    if (defIndex < 0) hasDef = false;

    _console.info("Possible choices for " + message + ":");
    for (int index = 0; index < choices.length(); index++)
    {
        QString choice = choices[index];
        if (!choice.isEmpty()) choice.replace(0, 1, choice[0].toUpper());
        _console.info(QString(index<9 ? "   ":"  ") + QString::number(index+1) + ". " + choice);
    }

    if (choices.size()==1 && !allowNoChoice)
    {
        _console.info("Automatically selected the only choice: 1");
        return 0;
    }
    return promptForInt("Enter one of these numbers" + (allowNoChoice ? " " + noChoiceMessage : ""),
                        (allowNoChoice ? 0 : 1), choices.length(),
                        true, (hasDef ? defIndex+1 : (allowNoChoice ? 0 : 1)) ) - 1;
}

////////////////////////////////////////////////////////////////////
