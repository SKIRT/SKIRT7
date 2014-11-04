/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "BasicChoiceWizardPane.hpp"

#include <QVBoxLayout>
#include <QLabel>
#include <QRadioButton>
#include <QButtonGroup>
#include <QMessageBox>
#include <QApplication>

////////////////////////////////////////////////////////////////////

BasicChoiceWizardPane::BasicChoiceWizardPane(int initialChoice, bool dirty, QObject* target)
{
    // remember constructor arguments
    _choice = initialChoice;
    _dirty = dirty;

    // create the title
    auto title = new QLabel("Welcome to the SKIRT MakeUp wizard!");

    // create the group of radio buttons that allow the user to make the basic choice
    auto question = new QLabel("What would you like to do? Select one of the following options:");
    auto choice1 = new QRadioButton("Create and configure a new SKIRT parameter file");
    auto choice2 = new QRadioButton("Create and configure a new FitScheme file");
    auto choice3 = new QRadioButton("Open and edit an existing SKIRT parameter file");
    auto choice4 = new QRadioButton("Open and edit an existing FitScheme file");
    _buttonGroup = new QButtonGroup;
    _buttonGroup->addButton(choice1, 1);
    _buttonGroup->addButton(choice2, 2);
    _buttonGroup->addButton(choice3, 3);
    _buttonGroup->addButton(choice4, 4);

    // select the initial choice
    auto selected = _buttonGroup->button(initialChoice);
    if (selected) selected->setChecked(true);

    // connect the button group to ourselves, and ourselves to the target
    connect(_buttonGroup, SIGNAL(buttonClicked(int)), this, SLOT(setBasicChoice(int)));
    connect(this, SIGNAL(basicChoiceWasChanged(int)), target, SLOT(setBasicChoice(int)));

    // put everything in a layout
    auto choiceLayout = new QVBoxLayout;
    choiceLayout->addWidget(title);
    choiceLayout->addWidget(question);
    choiceLayout->addWidget(choice1);
    choiceLayout->addWidget(choice2);
    choiceLayout->addWidget(choice3);
    choiceLayout->addWidget(choice4);
    choiceLayout->addStretch();

    // assign the layout to ourselves
    setLayout(choiceLayout);
}

////////////////////////////////////////////////////////////////////

void BasicChoiceWizardPane::setBasicChoice(int choice)
{
    if (choice != _choice)
    {
        bool update = true;

        // if the current hierarchy is dirty, give the user a chance to cancel the change
        if (_dirty)
        {
            auto ret = QMessageBox::warning(dynamic_cast<QWidget*>(parent()), qApp->applicationName(),
                                            "Do you want to discard your unsaved changes?",
                                            QMessageBox::Discard | QMessageBox::Cancel, QMessageBox::Cancel);
            update = (ret == QMessageBox::Discard);
        }

        // if allowed, update the choice
        if (update)
        {
            _choice = choice;
            _dirty = false;
            emit basicChoiceWasChanged(choice);
        }
        // otherwise, revert the selected radio button
        else
        {
            _buttonGroup->button(_choice)->setChecked(true);
        }
    }
}

////////////////////////////////////////////////////////////////////
