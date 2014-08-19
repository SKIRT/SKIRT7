/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef CREATEROOTWIZARDPANE_HPP
#define CREATEROOTWIZARDPANE_HPP

#include <QWidget>
class QAbstractButton;

////////////////////////////////////////////////////////////////////

/** A CreateRootWizardPane instance displays the user interface for selecting the type of the root
    simulation item of a simulation item hierarchy. When the user makes a choice, the selected type
    is passed on to the target object. */
class CreateRootWizardPane : public QWidget
{
    Q_OBJECT

    // ============= Construction and Destruction =============

public:
    /** The default (and only) constructor creates and initializes the GUI for this pane. The first
        argument specifies the name of the abstract type of the root for the simulation hierarchy
        under construction. The second argument specifies the name of the current root type, which
        is used to determine the choice that will be selected when the pane is initially displayed.
        If the name is empty, none of the choices will be selected. The third argument specifies
        the object that will be notified of changes in the selection through invocation of the
        object's setRoot() slot. */
    explicit CreateRootWizardPane(QByteArray abstractType, QByteArray initialType, QObject* target);

    // ==================== Event Handling ====================

public slots:
    /** This function creates a simulation item of the type selected by the user, and passes it to
        the target object by invoking its setRoot() slot. */
    void selectTypeFor(QAbstractButton* button);

signals:
    /** This signal is emitted when the user selects a new root type. */
    void rootTypeChanged(QByteArray newRootType);
};

////////////////////////////////////////////////////////////////////

#endif // CREATEROOTWIZARDPANE_HPP
