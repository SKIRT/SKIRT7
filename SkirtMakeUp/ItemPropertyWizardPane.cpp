/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include "ItemPropertyWizardPane.hpp"

#include "ItemPropertyHandler.hpp"
#include "SimulationItemDiscovery.hpp"
#include <QVBoxLayout>
#include <QLabel>
#include <QRadioButton>
#include <QButtonGroup>
#include <QVariant>

using namespace SimulationItemDiscovery;

////////////////////////////////////////////////////////////////////

ItemPropertyWizardPane::ItemPropertyWizardPane(PropertyHandlerPtr handler, QObject* target)
    : PropertyWizardPane(handler, target)
{
    // create the layout so that we can add stuff one by one
    auto layout = new QVBoxLayout;

    // add the question
    layout->addWidget(new QLabel("Please select one of the following options for " + handler->title() + ":"));

    // add the radio buttons reflecting the possible choices, putting them into a button group as well
    auto buttonGroup = new QButtonGroup;
    auto myhandler = handlerCast<ItemPropertyHandler>();
    QByteArray initialType = myhandler->value() ? itemType(myhandler->value()) : "";
    for (auto choiceType : descendants(myhandler->baseType()))
    {
        auto choiceTitle = title(choiceType);
        if (!choiceTitle.isEmpty()) choiceTitle.replace(0, 1, choiceTitle[0].toUpper());
        auto choiceButton = new QRadioButton(choiceTitle);
        buttonGroup->addButton(choiceButton);
        layout->addWidget(choiceButton);

        // associate the item type corresponding to this button with the button object
        choiceButton->setProperty("choiceType", choiceType);

        // select the button corresponding to the initial choice
        if (choiceType==initialType)
        {
            choiceButton->setChecked(true);
            emit propertyValidChanged(true);
        }
    }

    // connect the button group to ourselves
    connect(buttonGroup, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(selectTypeFor(QAbstractButton*)));

    // finalize the layout and assign it to ourselves
    layout->addStretch();
    setLayout(layout);
}

////////////////////////////////////////////////////////////////////

void ItemPropertyWizardPane::selectTypeFor(QAbstractButton* button)
{
    auto myhandler = handlerCast<ItemPropertyHandler>();
    myhandler->setToNewItemOfType(button->property("choiceType").toByteArray());
    emit propertyValidChanged(true);
}

////////////////////////////////////////////////////////////////////
