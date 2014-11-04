/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef WIZARDENGINE_HPP
#define WIZARDENGINE_HPP

#include <QObject>
class SimulationItem;

////////////////////////////////////////////////////////////////////

/** The WizardEngine class implements the back end of the wizard through which users can create ski
    files. The MainWindow creates a single WizardEngine instance. This WizardEngine object manages
    the SimulationItem hierarchy under construction, keeps track of the wizard's state (i.e. which
    question should currently be displayed), allowing it to advance and retreat, and generates the
    user interface pane corresponding to the current state. */
class WizardEngine : public QObject
{
    Q_OBJECT

    // ============= Construction and Destruction =============

public:
    /** The default (and only) constructor places the WizardEngine in its initial state,
        corresponding to the question "what would you like to do". */
    WizardEngine(QObject* parent);

    /** The destructor deallocates the SimulationItem hierarchy under construction. */
    ~WizardEngine();

    // ================== State Handling ====================

public:
    /** This function returns true if the wizard can currently advance; false otherwise. */
    bool canAdvance();

    /** This function returns true if the wizard can currently retreat; false otherwise. */
    bool canRetreat();

    /** This function returns true if the wizard currently holds unsaved information; false
        otherwise. */
    bool isDirty();

    /** This function returns the path of the file to which the current hierarchy has already been
        saved (although it may have changed since then), or the empty string if it has never been
        saved. */
    QString filepath();

public slots:
    /** This function advances the wizard to the next state. It should only be called if
        canAdvance() returns true. */
    void advance();

    /** This function retreats the wizard to the previous state. It should only be called if
        canRetreat() returns true. */
    void retreat();

    /** This function advances the wizard to a state that starts editing the specified item in the
        current item list property. */
    void advanceToEditSubItem(int subItemIndex);

signals:
    /** This signal is emitted when the return value of the canAdvance() function may have changed.
        The argument specifies the new value. */
    void canAdvanceChangedTo(bool canAdvance);

    /** This signal is emitted when the return value of the canRetreat() function may have changed.
        The argument specifies the new value. */
    void canRetreatChangedTo(bool canRetreat);

    /** This signal is emitted when the state of the wizard has changed. */
    void stateChanged();

    /** This signal is emitted when the filename in which the current hierarchy was saved has
        changed. */
    void titleChanged();

    /** This signal is emitted when the dirty state of the current hierarchy has changed. */
    void dirtyChanged();

public:
    /** This function emits the stateChanged(), canAdvanceChangedTo() and canRetreatChangedTo()
        signals. */
    void emitStateChanged();

    // ================== State Updating ====================

public slots:
    /** This function updates the basic choice to the specified value. */
    void setBasicChoice(int newChoice);

    /** If the current root does not have the specified type (or if there is no current root), this
        function deletes the current simulation hierarchy (if present), and replaces it by a newly
        created root simulation item of the specified type. If the current root already has the
        specified type, this function does nothing. */
    void setRootType(QByteArray newRootType);

    /** This function deletes the current simulation hierarchy (if present), and replaces it by the
        new simulation hierarchy specified through it root item. The function adopts ownership for
        specified hierarchy. This function further clears the dirty flag and remembers the filepath
        from which the hierarchy was loaded. */
    void hierarchyWasLoaded(SimulationItem* root, QString filepath);

    /** This function updates the flag that indicates whether the value of the property currently
        being handled is valid. */
    void setPropertyValid(bool valid);

    /** This function sets the dirty flag. */
    void hierarchyWasChanged();

    /** This function clears the dirty flag and remembers the filepath in which the hierarchy was
        saved. */
    void hierarchyWasSaved(QString filepath);

    // ================== GUI Generation ====================

public:
    /** This function creates a fresh QWidget object corresponding to the current wizard state,
        returns a pointer to it, and transfers ownership to the caller. The QWidget has no parent,
        but is otherwise fully equipped to handle the keyboard and mouse events for any UI elements
        it contains. For example, the UI elements are equipped so that they can properly update the
        corresponding portion of SimulationItem hierarchy under construction. */
    QWidget* createPane();

    // ================== Data Members ====================

private:
    // ---- data members representing the state of the wizard ----

    // the top-level state; always valid
    enum State { BasicChoice, CreateRoot, OpenHierarchy, ConstructHierarchy, SaveHierarchy };
    State _state = BasicChoice;

    // the basic choice; always valid
    enum Choice { Unknown, NewSki, NewFski, OpenSki, OpenFski };
    Choice _choice = NewSki;

    // the simulation item hierarchy under construction; pointer has ownership;
    // always valid but remains null until CreateRoot has been completed at least once
    SimulationItem* _root = 0;

    // the simulation item currently being handled; pointer is a reference without ownership;
    // valid only during ConstructHierarchy
    SimulationItem* _current = 0;

    // the zero-based index of the property currently being handled (in the current simulation item);
    // valid only during ConstructHierarchy
    int _propertyIndex = -1;

    // the zero-based index of the currently selected sub-item of the current item list property,
    // or -1 when editing the item list property itself;
    // valid only if the current property is an item list
    int _subItemIndex = -1;

    // true if the value of the property being handled is valid, false otherwise
    // valid only during ConstructHierarchy
    bool _propertyValid = false;

    // ---- data members related to the state of the wizard ----

    // true if the current hierarchy holds unsaved information, false otherwise; always valid
    bool _dirty = false;

    // the path of the file to which the current hierarchy has already been saved (although it may have changed),
    // or the empty string if it has never been saved; always valid
    QString _filepath;
};

////////////////////////////////////////////////////////////////////

#endif // WIZARDENGINE_HPP
