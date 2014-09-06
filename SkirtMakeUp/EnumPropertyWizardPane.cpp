/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include "EnumPropertyWizardPane.hpp"

#include "EnumPropertyHandler.hpp"
#include "SimulationItemDiscovery.hpp"
#include <QVBoxLayout>
#include <QLabel>
#include <QRadioButton>
#include <QButtonGroup>
#include <QVariant>

using namespace SimulationItemDiscovery;

////////////////////////////////////////////////////////////////////

EnumPropertyWizardPane::EnumPropertyWizardPane(PropertyHandlerPtr handler, QObject* target)
    : PropertyWizardPane(handler, target)
{
    auto hdlr = handlerCast<EnumPropertyHandler>();

    // create the layout so that we can add stuff one by one
    auto layout = new QVBoxLayout;

    // add the question
    layout->addWidget(new QLabel("Select one of the following options for " + handler->title() + ":"));

    // determine the current and default values
    QString currentKey = hdlr->value();
    QString defaultKey = hdlr->hasDefaultValue() ? hdlr->defaultValue() : "";

    // make a button group to contain the radio buttons reflecting the possible choices
    auto buttonGroup = new QButtonGroup;

    // add the choices
    auto choiceKeys = hdlr->values();
    auto choiceTitles = hdlr->titlesForValues();
    for (int index=0; index!=choiceKeys.size(); ++index)
    {
        auto choiceKey = choiceKeys[index];
        auto choiceTitle = choiceTitles[index];
        if (!choiceTitle.isEmpty()) choiceTitle.replace(0, 1, choiceTitle[0].toUpper());
        if (choiceKey==defaultKey) choiceTitle += "  [default]";
        auto choiceButton = new QRadioButton(choiceTitle);
        buttonGroup->addButton(choiceButton);
        layout->addWidget(choiceButton);

        // associate the item type corresponding to this button with the button object
        choiceButton->setProperty("choiceKey", choiceKey);

        // if the property has never been configured by the user,
        // and this button corresponds to the default value,
        // store this value into the property
        if (!isPropertyConfigured() && choiceKey==defaultKey)
        {
            hdlr->setValue(choiceKey);
            // adjust the current choice to trigger the next "if"
            currentKey = choiceKey;
        }

        // if this button corresponds to the current value, select it
        if (choiceKey==currentKey)
        {
            choiceButton->setChecked(true);
            emit propertyValidChanged(true);
        }
    }

    // connect the button group to ourselves
    connect(buttonGroup, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(updateValueFor(QAbstractButton*)));

    // finalize the layout and assign it to ourselves
    layout->addStretch();
    setLayout(layout);
}

////////////////////////////////////////////////////////////////////

void EnumPropertyWizardPane::updateValueFor(QAbstractButton* button)
{
    auto hdlr = handlerCast<EnumPropertyHandler>();
    hdlr->setValue(button->property("choiceKey").toString());
    setPropertyConfigured();
    emit propertyValidChanged(true);
}

////////////////////////////////////////////////////////////////////
