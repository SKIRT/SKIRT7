/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef WIZARDENGINE_HPP
#define WIZARDENGINE_HPP

#include <QObject>
class SimulationItem;
class QWidget;

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

public slots:
    /** This function advances the wizard to the next state. It should only be called if
        canAdvance() returns true. */
    void advance();

    /** This function retreats the wizard to the previous state. It should only be called if
        canRetreat() returns true. */
    void retreat();

signals:
    /** This signal is emitted when the return value of the canAdvance() function may have changed.
        The argument specifies the new value. */
    void canAdvanceChangedTo(bool canAdvance);

    /** This signal is emitted when the return value of the canRetreat() function may have changed.
        The argument specifies the new value. */
    void canRetreatChangedTo(bool canRetreat);

    /** This signal is emitted when the state of the wizard has changed. */
    void stateChanged();

public:
    /** This function emits the stateChanged(), canAdvanceChangedTo() and canRetreatChangedTo()
        signals. */
    void emitStateChanged();

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
    enum State { BasicChoice, CreateRoot, ConstructHierarchy, SaveHierarchy };
    State _state;
    SimulationItem* _root;
};

////////////////////////////////////////////////////////////////////

#endif // WIZARDENGINE_HPP
