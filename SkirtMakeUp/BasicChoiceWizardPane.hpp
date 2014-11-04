/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef BASICCHOICEWIZARDPANE_HPP
#define BASICCHOICEWIZARDPANE_HPP

#include <QWidget>
class QButtonGroup;

////////////////////////////////////////////////////////////////////

/** A BasicChoiceWizardPane instance displays the user interface for the question "what do you want
    to do" at the start of the wizard guidance process, and it passes the selection on to the
    target object. */
class BasicChoiceWizardPane : public QWidget
{
    Q_OBJECT

    // ============= Construction and Destruction =============

public:
    /** The default (and only) constructor creates and initializes the GUI for this pane. The first
        argument specifies the choice that will be selected when the pane is initially displayed. A
        value of one means the first choice, two means the second choice, and so on. With a
        negative, zero or out-of-range value, none of the choices will be selected. The second argument
     indicates the  dirty state of the current simulation item hierarchy. The last
        argument specifies the object that will be notified of changes in the selection through
        invocation of the object's setBasicChoice() slot. */
    explicit BasicChoiceWizardPane(int initialChoice, bool dirty, QObject* target);

    // ==================== Event Handling ====================

public slots:
    /** This function updates the basic choice in reaction to a user click on one of the radio
        buttons. If the current hierarchy is dirty, and the new choice differs from the previous
        one, the function asks for confirmation from the user before actually updating the choice.
        If the new choice is the same as the previous one, the function does nothing. */
    void setBasicChoice(int choice);

signals:
    /** This signal is emitted after the basic choice was changed. */
    void basicChoiceWasChanged(int choice);

    // ==================== Data members ======================

private:
    int _choice;
    bool _dirty;
    QButtonGroup* _buttonGroup;
};

////////////////////////////////////////////////////////////////////

#endif // BASICCHOICEWIZARDPANE_HPP
