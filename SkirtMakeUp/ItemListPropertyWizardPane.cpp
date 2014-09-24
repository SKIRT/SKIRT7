/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include "ItemListPropertyWizardPane.hpp"

#include "ItemListPropertyHandler.hpp"
#include "SimulationItemDiscovery.hpp"
#include <QVBoxLayout>
#include <QLabel>
#include <QRadioButton>
#include <QButtonGroup>
#include <QVariant>

using namespace SimulationItemDiscovery;

////////////////////////////////////////////////////////////////////

ItemListPropertyWizardPane::ItemListPropertyWizardPane(PropertyHandlerPtr handler, QObject* target)
    : PropertyWizardPane(handler, target)
{
    auto hdlr = handlerCast<ItemListPropertyHandler>();

    // create the layout so that we can add stuff one by one
    auto layout = new QVBoxLayout;

    // add the question
    layout->addWidget(new QLabel("Dummy handler for " + hdlr->title() + ":"));

    // dummy implementation --- TODO
    emit propertyValidChanged(true);

    // finalize the layout and assign it to ourselves
    layout->addStretch();
    setLayout(layout);
}

////////////////////////////////////////////////////////////////////
