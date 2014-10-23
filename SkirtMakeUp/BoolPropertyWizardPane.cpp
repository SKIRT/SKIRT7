/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "BoolPropertyWizardPane.hpp"

#include "BoolPropertyHandler.hpp"
#include <QVBoxLayout>
#include <QLabel>
#include <QRadioButton>
#include <QButtonGroup>

////////////////////////////////////////////////////////////////////

BoolPropertyWizardPane::BoolPropertyWizardPane(PropertyHandlerPtr handler, QObject* target)
    : PropertyWizardPane(handler, target)
{
    auto hdlr = handlerCast<BoolPropertyHandler>();

    // create the layout so that we can add stuff one by one
    auto layout = new QVBoxLayout;

    // add the question
    layout->addWidget(new QLabel("Do you want to " + hdlr->title() + "?"));

    // make a button group to contain the radio buttons reflecting the possible choices
    auto buttonGroup = new QButtonGroup;

    // add the "No" choice
    {
        bool isDefault = hdlr->hasDefaultValue() && hdlr->defaultValue()==false;
        bool isCurrent = hdlr->value()==false;

        // add the choice button the group and to the layout
        auto choiceButton = new QRadioButton(QString("No") + (isDefault ? "  [default]" : ""));
        buttonGroup->addButton(choiceButton, 0);
        layout->addWidget(choiceButton);

        // if the property has never been configured by the user,
        // and this button corresponds to the default value,
        // store this value into the property
        if (!isPropertyConfigured() && isDefault)
        {
            hdlr->setValue(false);
            // adjust the "current" flag to trigger the next "if"
            isCurrent = true;
        }

        // if this button corresponds to the current value, select it
        if (isCurrent)
        {
            choiceButton->setChecked(true);
            emit propertyValidChanged(true);
        }
    }

    // add the "Yes" choice
    {
        bool isDefault = hdlr->hasDefaultValue() && hdlr->defaultValue()==true;
        bool isCurrent = hdlr->value()==true;

        // add the choice button the group and to the layout
        auto choiceButton = new QRadioButton(QString("Yes") + (isDefault ? "  [default]" : ""));
        buttonGroup->addButton(choiceButton, 1);
        layout->addWidget(choiceButton);

        // if the property has never been configured by the user,
        // and this button corresponds to the default value,
        // store this value into the property
        if (!isPropertyConfigured() && isDefault)
        {
            hdlr->setValue(true);
            // adjust the "current" flag to trigger the next "if"
            isCurrent = true;
        }

        // if this button corresponds to the current value, select it
        if (isCurrent)
        {
            choiceButton->setChecked(true);
            emit propertyValidChanged(true);
        }
    }

    // connect the button group to ourselves
    connect(buttonGroup, SIGNAL(buttonClicked(int)), this, SLOT(updateValueFor(int)));

    // finalize the layout and assign it to ourselves
    layout->addStretch();
    setLayout(layout);
}

////////////////////////////////////////////////////////////////////

void BoolPropertyWizardPane::updateValueFor(int buttonID)
{
    auto hdlr = handlerCast<BoolPropertyHandler>();
    hdlr->setValue(buttonID ? true : false);
    setPropertyConfigured();
    emit propertyValidChanged(true);
}

////////////////////////////////////////////////////////////////////
