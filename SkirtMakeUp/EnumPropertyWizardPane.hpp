/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef ENUMPROPERTYWIZARDPANE_HPP
#define ENUMPROPERTYWIZARDPANE_HPP

#include "PropertyWizardPane.hpp"
class QAbstractButton;

////////////////////////////////////////////////////////////////////

/** An EnumPropertyWizardPane instance displays the user interface corresponding to an
    EnumPropertyHandler. When the user makes a choice, the corresponding value is updated in the
    target property. */
class EnumPropertyWizardPane : public PropertyWizardPane
{
    Q_OBJECT

    // ============= Construction and Destruction =============

public:
    /** The default (and only) constructor creates and initializes the GUI for this pane. For a
        description of the arguments, see the PropertyWizardPane constructor. */
    explicit EnumPropertyWizardPane(PropertyHandlerPtr handler, QObject* target);

    // ==================== Event Handling ====================

public slots:
    /** This function stores the value corresponding to the specified button into the target
        property. */
    void updateValueFor(QAbstractButton* button);
};

////////////////////////////////////////////////////////////////////

#endif // ENUMPROPERTYWIZARDPANE_HPP
