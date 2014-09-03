/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include "BasicChoiceWizardPane.hpp"

#include <QVBoxLayout>
#include <QLabel>
#include <QRadioButton>
#include <QButtonGroup>

////////////////////////////////////////////////////////////////////

BasicChoiceWizardPane::BasicChoiceWizardPane(int initialChoice, QObject* target)
{
    // create the title
    auto title = new QLabel("Welcome to the SKIRT MakeUp wizard!");

    // create the group of radio buttons that allow the user to make the basic choice
    auto question = new QLabel("What would you like to do? Select one of the following options:");
    auto choice1 = new QRadioButton("Create and configure a new SKIRT parameter file");
    auto choice2 = new QRadioButton("Create and configure a new FitScheme file");
    auto buttonGroup = new QButtonGroup;
    buttonGroup->addButton(choice1, 1);
    buttonGroup->addButton(choice2, 2);

    // select the initial choice
    auto selected = buttonGroup->button(initialChoice);
    if (selected) selected->setChecked(true);

    // connect the button group to the target
    connect(buttonGroup, SIGNAL(buttonClicked(int)), target, SLOT(setBasicChoice(int)));

    // put everything in a layout
    auto choiceLayout = new QVBoxLayout;
    choiceLayout->addWidget(title);
    choiceLayout->addWidget(question);
    choiceLayout->addWidget(choice1);
    choiceLayout->addWidget(choice2);
    choiceLayout->addStretch();

    // assign the layout to ourselves
    setLayout(choiceLayout);
}

////////////////////////////////////////////////////////////////////
