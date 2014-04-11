/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef SIMULATIONITEMREGISTRY_HPP
#define SIMULATIONITEMREGISTRY_HPP

#include <QByteArray>
#include <QList>
#include <QMetaObject>

////////////////////////////////////////////////////////////////////

/** This namespace serves as a static registry for all discoverable simulation item classes. The
    application must explicitly initialize the registry by calling the add() function for each
    simulation item class that should be discoverable. This initialization must occur from within a
    single execution thread (since the internal data structures are not thread-safe) and before any
    of the discovery functionality is used (for obvious reasons). The other functions in this
    namespace are intended for use only by the discovery mechanism itself. */
namespace SimulationItemRegistry
{
    /** Adds the simulation item class specified as template argument to the registry. The \em
        concrete argument should be true (the default) for concrete classes with a public default
        constructor, and false for abstract classes that shouldn't be instantiated. All simulation
        item classes must be added from within a single execution thread and before any of the
        discovery functionality is used. */
    template<class itemClass> void add(bool concrete = true);

    /** Adds the simulation item class specified by a meta object to the registry. The \em
        concrete argument should be true for concrete classes with a public default constructor,
        and false for abstract classes that shouldn't be instantiated. This function is called from
        the add() template function and is not intended for public use. */
    void addPrivate(const QMetaObject* item, bool concrete);

    /** Returns a list of all registered concrete item types, i.e. class names for simulation item
        classes, in order of addition to the registry. Abstract classes are not included in the
        list. */
    QList<QByteArray> concreteItemTypes();

    /** Returns the meta object for the specified item type, i.e. simulation item class name, or
        null if the item type is not in the registry. The item type may be concrete or abstract. */
    const QMetaObject* metaObject(QByteArray itemType);
}

////////////////////////////////////////////////////////////////////

// template function implementation
template<class itemClass> void SimulationItemRegistry::add(bool concrete)
{
    addPrivate(&itemClass::staticMetaObject, concrete);
}

////////////////////////////////////////////////////////////////////

#endif // SIMULATIONITEMREGISTRY_HPP
