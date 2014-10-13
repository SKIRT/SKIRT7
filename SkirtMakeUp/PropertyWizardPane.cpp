/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include "PropertyWizardPane.hpp"

#include "SimulationItem.hpp"
#include <QVariant>

////////////////////////////////////////////////////////////////////

PropertyWizardPane::PropertyWizardPane(PropertyHandlerPtr handler, QObject* target)
{
    _handler = handler;
    connect(this, SIGNAL(propertyValidChanged(bool)), target, SLOT(setPropertyValid(bool)));
}

////////////////////////////////////////////////////////////////////

void PropertyWizardPane::showEvent(QShowEvent* event)
{
    focusNextChild();
    QWidget::showEvent(event);
}

////////////////////////////////////////////////////////////////////

void PropertyWizardPane::setPropertyConfigured(bool configured)
{
    _handler->target()->setProperty(_handler->name()+"_configured", configured);
}

////////////////////////////////////////////////////////////////////

bool PropertyWizardPane::isPropertyConfigured()
{
    return _handler->target()->property(_handler->name()+"_configured").toBool();
}

////////////////////////////////////////////////////////////////////
