/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include "CreateRootWizardPane.hpp"

#include "SimulationItemDiscovery.hpp"
#include <QVBoxLayout>
#include <QLabel>
#include <QRadioButton>
#include <QButtonGroup>
#include <QVariant>

using namespace SimulationItemDiscovery;

////////////////////////////////////////////////////////////////////

CreateRootWizardPane::CreateRootWizardPane(QByteArray abstractType, QByteArray initialType, QObject* target)
{
    // create the layout so that we can add stuff one by one
    auto layout = new QVBoxLayout;

    // add the title and question
    layout->addWidget(new QLabel("Choosing the type of " + title(abstractType) + "."));
    layout->addWidget(new QLabel("Please select one of the following options:"));

    // add the radio buttons reflecting the possible choices, putting them into a button group as well
    auto buttonGroup = new QButtonGroup;
    for (auto choiceType : descendants(abstractType))
    {
        auto choiceTitle = title(choiceType);
        if (!choiceTitle.isEmpty()) choiceTitle.replace(0, 1, choiceTitle[0].toUpper());
        auto choiceButton = new QRadioButton(choiceTitle);
        buttonGroup->addButton(choiceButton);
        layout->addWidget(choiceButton);

        // associate the item type corresponding to this button with the button object
        choiceButton->setProperty("choiceType", choiceType);

        // select the button corresponding to the initial choice
        if (choiceType==initialType) choiceButton->setChecked(true);
    }

    // connect the button group to ourselves, and ourselves to the target
    connect(buttonGroup, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(selectTypeFor(QAbstractButton*)));
    connect(this, SIGNAL(rootTypeChanged(QByteArray)), target, SLOT(setRootType(QByteArray)));

    // finalize the layout and assign it to ourselves
    layout->addStretch();
    setLayout(layout);
}

////////////////////////////////////////////////////////////////////

void CreateRootWizardPane::selectTypeFor(QAbstractButton* button)
{
    emit rootTypeChanged(button->property("choiceType").toByteArray());
}

////////////////////////////////////////////////////////////////////
