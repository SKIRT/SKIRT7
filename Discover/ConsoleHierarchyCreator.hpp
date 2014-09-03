/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef CONSOLEHIERARCHYCREATOR_HPP
#define CONSOLEHIERARCHYCREATOR_HPP

#include <QByteArray>
#include <QMetaObject>
#include <QObject>
#include <QSet>
#include <QStringList>
#include "Console.hpp"
#include "PropertyHandlerVisitor.hpp"
class SimulationItem;

////////////////////////////////////////////////////////////////////

/** This class creates a simulation hierarchy through user interaction via the console. It inherits
    from PropertyHandlerVisitor to facilitate handling of the various types of properties. */
class ConsoleHierarchyCreator : public PropertyHandlerVisitor
{
public:
    /** Constructs a ConsoleHierarchyCreator instance without doing anything; call the
        createHierarchy() function to actually create a simulation hierarchy. */
    ConsoleHierarchyCreator();

    /** Creates a fresh simulation hierarchy by asking questions (and receiving answers) via the
        console, and returns a pointer to the top-most simulation item in the hierarchy (handing
        over ownership for the complete hierarchy to the caller). The template argument specifies
        the class name of the top-level simulation item to be returned. If the hierarchy can't be
        created due to some error condition the function throws a fatal error. */
    template<class itemClass> itemClass* createHierarchy();

private:
    /** This is the private version of the function to create a simulation hierarchy; it is
        called from the public template function with the same name. */
    SimulationItem* createHierarchy(QByteArray topItemType);

    /** Recursively sets up the properties of the specified simulation item and its children. This
        is accomplished by asking each of the properties to accept "this" PropertyHandlerVisitor
        instance as a visitor, which causes a call-back to the visitPropertyHandler() function with
        the appropriate PropertyHandler subtype. */
    void setupProperties(SimulationItem* item);

public:
    /** Sets up a property of the specified type, as part of the visitor pattern initiated by the
        setupProperties() function. */
    void visitPropertyHandler(BoolPropertyHandler* handler);

    /** Sets up a property of the specified type, as part of the visitor pattern initiated by the
        setupProperties() function. */
    void visitPropertyHandler(IntPropertyHandler* handler);

    /** Sets up a property of the specified type, as part of the visitor pattern initiated by the
        setupProperties() function. */
    void visitPropertyHandler(DoublePropertyHandler* handler);

    /** Sets up a property of the specified type, as part of the visitor pattern initiated by the
        setupProperties() function. */
    void visitPropertyHandler(DoubleListPropertyHandler* handler);

    /** Sets up a property of the specified type, as part of the visitor pattern initiated by the
        setupProperties() function. */
    void visitPropertyHandler(StringPropertyHandler* handler);

    /** Sets up a property of the specified type, as part of the visitor pattern initiated by the
        setupProperties() function. */
    void visitPropertyHandler(EnumPropertyHandler* handler);

    /** Sets up a property of the specified type, as part of the visitor pattern initiated by the
        setupProperties() function. */
    void visitPropertyHandler(ItemPropertyHandler* handler);

    /** Sets up a property of the specified type, as part of the visitor pattern initiated by the
        setupProperties() function. */
    void visitPropertyHandler(ItemListPropertyHandler* handler);

private:
    /** Prompts the console for a yes/no reply within the specified default value, and returns the
        user's response. */
    bool promptForBool(QString message, bool hasDef, bool def);

    /** Prompts the console for an integer number within the specified range and with the specified
        default value, and returns the user's response. */
    int promptForInt(QString message, int min, int max, bool hasDef, int def);

    /** Prompts the console for a floating point number within the specified range and with the
        specified default value, and returns the user's response. */
    double promptForDouble(QString message, double min, double max, bool hasDef, double def,
                           DoublePropertyHandler* handler);

    /** Prompts the console for a non-empty list of floating point numbers, each within the
        specified range, and returns the user's response. */
    QList<double> promptForDoubleList(QString message, double min, double max, DoubleListPropertyHandler* handler);

    /** Prompts the console for a non-empty string with the specified default value, and returns
        the user's response. */
    QString promptForString(QString message, bool hasDef, QString def);

    /** Prompts the console for a choice from the specified list, with the specified default, and
        returns the user's response. The function returns a zero-based index into the \em choices
        list. If \em allowNoChoice is true, the function returns -1 to indicate that no choice was
        made. */
    int promptForChoice(QString message, QStringList choices, bool hasDef=false, int defIndex=-1,
                        bool allowNoChoice=false, QString noChoiceMessage=QString());

private:
    // Unicode-aware console i/o
    Console _console;

    // set of keywords used for conditional inclusion of class types
    QSet<QByteArray> _keys;
};

////////////////////////////////////////////////////////////////////

// member template implementation
template<class itemClass> itemClass* ConsoleHierarchyCreator::createHierarchy()
{
    return dynamic_cast<itemClass*>(createHierarchy(itemClass::staticMetaObject.className()));
}

////////////////////////////////////////////////////////////////////

#endif // CONSOLEHIERARCHYCREATOR_HPP
