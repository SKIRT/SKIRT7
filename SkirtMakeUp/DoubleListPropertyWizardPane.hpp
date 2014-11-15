/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef DOUBLELISTPROPERTYWIZARDPANE_HPP
#define DOUBLELISTPROPERTYWIZARDPANE_HPP

#include "PropertyWizardPane.hpp"

////////////////////////////////////////////////////////////////////

/** A DoubleListPropertyWizardPane instance displays the user interface corresponding to a
    DoublePropertyHandler. When the user changes the edit field, the corresponding value is updated
    in the target property. */
class DoubleListPropertyWizardPane : public PropertyWizardPane
{
    Q_OBJECT

    // ============= Construction and Destruction =============

public:
    /** The default (and only) constructor creates and initializes the GUI for this pane. For a
        description of the arguments, see the PropertyWizardPane constructor. */
    explicit DoubleListPropertyWizardPane(PropertyHandlerPtr handler, QObject* target);

    // ==================== Event Handling ====================

public slots:
    /** This function stores the value corresponding to the specified text string into the target
        property. */
    void updateValue(const QString& text);
};

////////////////////////////////////////////////////////////////////

#endif // DOUBLELISTPROPERTYWIZARDPANE_HPP
