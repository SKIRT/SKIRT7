/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "PropertyWizardPane.hpp"

#include "SimulationItem.hpp"
#include "SimulationItemTools.hpp"

////////////////////////////////////////////////////////////////////

PropertyWizardPane::PropertyWizardPane(PropertyHandlerPtr handler, QObject* target)
{
    _handler = handler;
    connect(this, SIGNAL(propertyValidChanged(bool)), target, SLOT(setPropertyValid(bool)));
    connect(this, SIGNAL(propertyValueChanged()), target, SLOT(hierarchyWasChanged()));
}

////////////////////////////////////////////////////////////////////

void PropertyWizardPane::showEvent(QShowEvent* event)
{
    setFocus();
    focusNextChild();
    QWidget::showEvent(event);
}

////////////////////////////////////////////////////////////////////

void PropertyWizardPane::setPropertyConfigured(bool configured)
{
    SimulationItemTools::setPropertyConfigured(_handler->target(), _handler->name(), configured);
}

////////////////////////////////////////////////////////////////////

bool PropertyWizardPane::isPropertyConfigured()
{
    return SimulationItemTools::isPropertyConfigured(_handler->target(), _handler->name());
}

////////////////////////////////////////////////////////////////////
