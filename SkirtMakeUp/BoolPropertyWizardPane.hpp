/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef BOOLPROPERTYWIZARDPANE_HPP
#define BOOLPROPERTYWIZARDPANE_HPP

#include "PropertyWizardPane.hpp"

////////////////////////////////////////////////////////////////////

/** A BoolPropertyWizardPane instance displays the user interface corresponding to a
    BoolPropertyHandler. When the user makes a choice, the corresponding value is updated in the
    target property. */
class BoolPropertyWizardPane : public PropertyWizardPane
{
    Q_OBJECT

    // ============= Construction and Destruction =============

public:
    /** The default (and only) constructor creates and initializes the GUI for this pane. For a
        description of the arguments, see the PropertyWizardPane constructor. */
    explicit BoolPropertyWizardPane(PropertyHandlerPtr handler, QObject* target);

    // ==================== Event Handling ====================

public slots:
    /** This function stores the value corresponding to the specified button into the target
        property. */
    void updateValueFor(int buttonID);
};

////////////////////////////////////////////////////////////////////

#endif // BOOLPROPERTYWIZARDPANE_HPP
