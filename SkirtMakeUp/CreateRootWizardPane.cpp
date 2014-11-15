/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

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
    // connect ourselves to the target
    connect(this, SIGNAL(rootTypeChanged(QByteArray)), target, SLOT(setRootType(QByteArray)));

    // if there is currently no root, create one using the first option in the list
    if (initialType.isEmpty())
    {
        initialType = descendants(abstractType)[0];
        emit rootTypeChanged(initialType);
    }

    // create the layout so that we can add stuff one by one
    auto layout = new QVBoxLayout;

    // add the question
    layout->addWidget(new QLabel("Select one of the following options for the type of " + title(abstractType) + ":"));

    // add the radio buttons reflecting the possible choices, putting them into a button group as well
    auto buttonGroup = new QButtonGroup;
    for (auto choiceType : descendants(abstractType))
    {
        auto choiceTitle = title(choiceType);
        if (!choiceTitle.isEmpty()) choiceTitle.replace(0, 1, choiceTitle[0].toUpper());
        auto choiceButton = new QRadioButton(choiceTitle);
        choiceButton->setFocusPolicy(Qt::NoFocus);
        buttonGroup->addButton(choiceButton);
        layout->addWidget(choiceButton);

        // associate the item type corresponding to this button with the button object
        choiceButton->setProperty("choiceType", choiceType);
        choiceButton->setToolTip(choiceType);

        // select the button corresponding to the initial choice
        if (choiceType==initialType) choiceButton->setChecked(true);
    }

    // connect the button group to ourselves
    connect(buttonGroup, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(selectTypeFor(QAbstractButton*)));

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
