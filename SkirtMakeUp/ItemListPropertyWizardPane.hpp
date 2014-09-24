/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef ITEMLISTPROPERTYWIZARDPANE_HPP
#define ITEMLISTPROPERTYWIZARDPANE_HPP

#include "PropertyWizardPane.hpp"
class QAbstractButton;

////////////////////////////////////////////////////////////////////

/** An ItemListPropertyWizardPane instance displays the user interface corresponding to an
    ItemListPropertyHandler. When the user makes a choice, a new simulation item of the selected
    type is created and added to the target property. */
class ItemListPropertyWizardPane : public PropertyWizardPane
{
    Q_OBJECT

    // ============= Construction and Destruction =============

public:
    /** The default (and only) constructor creates and initializes the GUI for this pane. For a
        description of the arguments, see the PropertyWizardPane constructor. */
    explicit ItemListPropertyWizardPane(PropertyHandlerPtr handler, QObject* target);

    // ==================== Event Handling ====================

};

////////////////////////////////////////////////////////////////////

#endif // ITEMLISTPROPERTYWIZARDPANE_HPP
