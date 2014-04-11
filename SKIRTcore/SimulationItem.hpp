/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef SIMULATIONITEM_HPP
#define SIMULATIONITEM_HPP

#include <typeinfo>
#include <QObject>

////////////////////////////////////////////////////////////////////

/** This is the base class for all classes in the simulation hierarchy. It inherits from QObject so
    that we can use the Qt facilities for object hierarchies and for class introspection.
    Simulation items form a compile-time class hierarchy through inheritance (with the
    SimulationItem class at the top), and a run-time object hierarchy using the QObject
    parent/children mechanism (usually with an instance of a MonteCarloSimulation subclass at the
    top). */
class SimulationItem : public QObject
{
    Q_OBJECT

    //============= Construction - Setup - Destruction =============

protected:
    /** The default constructor; it is protected since this is an abstract class. */
    SimulationItem();

public:
    /** This function performs setup for this simulation item and for all its descendents.
        Specifically it invokes setupSelfBefore() on itself, then setup() on all its children, and
        finally setupSelfAfter() on itself. As a rule, the constructor of a SimulationItem subclass
        sets any attributes to null values rather than to meaningful defaults (to avoid the extra
        construction time). Thus all attributes in the simulation hierarchy must have been
        explicitly set by the caller before invoking setup(). If setup() has already been invoked
        for the same item, this function does nothing. Do not override this function in subclasses;
        implement setupSelfBefore() and/or setupSelfAfter() instead. */
    void setup();

protected:
    /** This function should be overridden in a subclass to verify validity and completeness of the
        attribute values set for the class instance, and to perform any setup that must happen \em
        before the children of the class instance have been setup. This implementation here in the
        SimulationItem class does nothing. Any implementation overriding this function should start
        by calling the same function in its immediate base class. */
    virtual void setupSelfBefore();

    /** This function should be overridden in a subclass to perform any setup that must happen \em
        after the children of the class instance have been setup. This implementation here in the
        SimulationItem class does nothing. Any implementation overriding this function should start
        by calling the same function in its immediate base class. */
    virtual void setupSelfAfter();

    //======================== Other Functions =======================

public:
    /** This template function looks for a simulation item of a specific type in the hierarchy
        containing the receiving object, and returns a pointer to that item. The function guarantees
        that setup has been invoked on the item before it is returned. If no appropriate item is
        found, a FatalError is thrown. The class specified as template parameter must inherit from
        SimulationItem. The function searches for an appropriate item among all ancestors of the
        receiving item (including the receiving item itself) and their immediate children. In other
        words, it recursively runs upwards along the ancestors and goes just one level down for
        each ancestor. The function returns the first appropriate item found; if multiple items of
        the same type exist in the hierarchy, there is no telling which one of these will be
        returned. If no appropriate item is found, the function never returns so there is no need to
        check for null pointers. */
    template<class T> T* find() const;

private:
    /** This is the private implementation used by the find() template function. It takes a class
        name string argument rather than a data type template argument. */
    QObject* find(const char* className) const;

public:
    /** This template function looks for an interface of a specific type offered by the receiving
        simulation item, or by one of its self-designated delegates. The interface type is
        specified as the template argument. If an interface of the requested type is found, the
        function returns a pointer to it, or rather to the simulation item that implements the
        interface after it has been dynamically cast to the requested interface type. If no
        interface of the requested type is found, the function returns a null pointer. Thus the
        caller must check the returned pointer before dereferencing it.

        By default only the receiving simulation item is considered as a candidate for implementing
        the interface, but a simulation item subclass can provide a longer list of candidates to
        which the interface implementation can be delegated by overriding the implementation of the
        interfaceCandidates() function. */
    template<class T> T* interface();

protected:
    /** This virtual function is for use only by the interface() template function. It returns a
        list of simulation items that should be considered in the search for an item that
        implements the requested interface. The first item in the list that actually implements the
        interface will be returned by the interface() function. The implementation in this abstract
        class returns a list containing just the receiving item. A simulation item subclass can
        override this function to provide a longer list of candidates. */
    virtual QList<SimulationItem*> interfaceCandidates(const std::type_info& interfaceTypeInfo);

    //======================== Data Members ========================

protected:
    /** This enum includes a constant for each run-time state of a simulation instance, in
        chronological order. */
    enum State { Created = 0, SetupInProgress, SetupDone };

    // the run-time state of this instance
    State _state;
};

////////////////////////////////////////////////////////////////////

template<class T> T* SimulationItem::find() const
{
    T* item = dynamic_cast<T*>(find(T::staticMetaObject.className()));
    item->setup();  // ensure the item has been setup
    return item;
}

////////////////////////////////////////////////////////////////////

template<class T> T* SimulationItem::interface()
{
    foreach (SimulationItem* candidate, interfaceCandidates(typeid(T)))
    {
        T* interface = dynamic_cast<T*>(candidate);
        if (interface) return interface;
    }
    return 0;
}

////////////////////////////////////////////////////////////////////

#endif // SIMULATIONITEM_HPP
