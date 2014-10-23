/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef BASICCHOICEWIZARDPANE_HPP
#define BASICCHOICEWIZARDPANE_HPP

#include <QWidget>

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
        negative, zero or out-of-range value, none of the choices will be selected. The second
        argument specifies the object that will be notified of changes in the selection through
        invocation of the object's setBasicChoice() slot. */
    explicit BasicChoiceWizardPane(int initialChoice, QObject* target);
};

////////////////////////////////////////////////////////////////////

#endif // BASICCHOICEWIZARDPANE_HPP
