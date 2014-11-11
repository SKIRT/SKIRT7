/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef SIMULATIONITEMTOOLS_HPP
#define SIMULATIONITEMTOOLS_HPP

#include <QByteArray>
#include <QList>
#include <QSet>
class SimulationItem;

////////////////////////////////////////////////////////////////////

/** This namespace offers a set of utility functions for working with simulation items in the
    context of the wizard offered by SkirtMakeUp. Some functions in this namespace rely on
    functions in the SimulationItemDiscovery namespace and thus require that the simulation item
    registry has been appropriately initialized. Some functions in this namespace rely on the fact
    that the SimulationItem class inherits QObject, which offers e.g. the parent() and property()
    functionality. */
namespace SimulationItemTools
{
    /** This function sets the \em configured state for the specified property in the specified
        item to the specified boolean value. */
    void setPropertyConfigured(SimulationItem* item, QByteArray property, bool configured);

    /** This function sets the \em configured state of all properties in the specified simulation
        hierarchy to true. The function calls itself recursively to process the children of the
        specified root item. */
    void setHierarchyConfigured(SimulationItem* root);

    /** This function returns the \em configured state for the specified property in the specified
        item. If the \em configured state has never been set for this property and item, the
        function returns false. */
    bool isPropertyConfigured(SimulationItem* item, QByteArray property);

    /** This function sets the \em complete state for the specified item to true. */
    void setItemComplete(SimulationItem* item);

    /** This function sets the \em complete state for all items in the specified simulation
        hierarchy to true. The function calls itself recursively to process the children of the
        specified root item.*/
    void setHierarchyComplete(SimulationItem* root);

    /** This function clears the \em complete state for the specified item and for all its
        ascendants in the run-time hierarchy. */
    void setItemIncomplete(SimulationItem* item);

    /** This function returns the \em complete state for the specified item. If the \em complete
        state has never been set for this item, the function returns false. */
    bool isItemComplete(SimulationItem* item);

    /** This function stores the selected row index for the specified property in the specified
        item to the specified integer value. The function should be called only for item list
        properties, but the current implementation does not enforce this. */
    void storeSelectedRow(SimulationItem* item, QByteArray property, int row);

    /** This function returns the stored selected row index for the specified property in the specified
        item. If the selected row index has never been stored for this property and item, the
        function returns zero. */
    int retrieveSelectedRow(SimulationItem* item, QByteArray property);

    /** This function returns the root of the hierarchy in which the specified simulation item
        resides. */
    SimulationItem* getRoot(SimulationItem* item);

    /** This function adds the item types of all simulation items present in the specified
        hierarchy, and the item types of all their compile-time ascendants, to the specified set.
        The function calls itself recursively to process the children of the specified root item.
        */
    void addHierarchyTypeNames(SimulationItem* root, QSet<QByteArray>& keys);

    /** This function returns a list of item types, in order of addition to the simulation item
        registry, which inherit the specified item type and which are allowed according to
        conditional rules based on the presence of other item types in the hierarchy in which the
        specified item resides. The function first finds the root of the hierarchy in which the
        specified item resides, and then traverses the complete hierarchy to build a set containing
        the item types of all simulation items present in the hierarchy, and the item types of all
        their compile-time ascendants. Finally the function calls the
        SimulationItemDiscovery::allowedDescendants() function to produce the result. */
    QList<QByteArray> allowedDescendants(QByteArray parentType, SimulationItem* item);
}

////////////////////////////////////////////////////////////////////

#endif // SIMULATIONITEMTOOLS_HPP
