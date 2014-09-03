/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include "ItemPropertyWizardPane.hpp"

#include "ItemPropertyHandler.hpp"
#include "SimulationItem.hpp"
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
    auto hdlr = handlerCast<ItemPropertyHandler>();

    // create the layout so that we can add stuff one by one
    auto layout = new QVBoxLayout;

    // add the question
    layout->addWidget(new QLabel("Select one of the following options for " + handler->title() + ":"));

    // determine the current and default item types
    QByteArray currentType = hdlr->value() ? itemType(hdlr->value()) : "";
    QByteArray defaultType = hdlr->hasDefaultValue() ? hdlr->defaultItemType() : "";

    // if there is only one choice, make it the forced item type
    auto choiceList = descendants(hdlr->baseType());
    QByteArray forcedType = (choiceList.size()==1 && !hdlr->isOptional()) ? choiceList[0] : "";

    // make a button group to contain the radio buttons reflecting the possible choices
    auto buttonGroup = new QButtonGroup;

    // if the property is optional, add the "None" choice
    if (hdlr->isOptional())
    {
        auto choiceButton = new QRadioButton("None");
        buttonGroup->addButton(choiceButton);
        layout->addWidget(choiceButton);

        // if the property currently holds no item, select this button
        if (currentType.isEmpty())
        {
            choiceButton->setChecked(true);
            emit propertyValidChanged(true);
        }
    }

    // add the regular choices
    for (auto choiceType : choiceList)
    {
        auto choiceTitle = title(choiceType);
        if (!choiceTitle.isEmpty()) choiceTitle.replace(0, 1, choiceTitle[0].toUpper());
        if (choiceType==defaultType) choiceTitle += "  [default]";
        auto choiceButton = new QRadioButton(choiceTitle);
        buttonGroup->addButton(choiceButton);
        layout->addWidget(choiceButton);

        // associate the item type corresponding to this button with the button object
        choiceButton->setProperty("choiceType", choiceType);

        // if the property has never been configured by the user,
        // and this button corresponds to the default or forced type,
        // store a newly created item into the property
        if (!hdlr->target()->property(hdlr->name()+"_configured").toBool()
            && (choiceType==defaultType || choiceType==forcedType))
        {
            hdlr->setToNewItemOfType(choiceType);
            // adjust the current type to trigger the next "if"
            currentType = choiceType;
        }

        // if this button corresponds to the current type, select it
        if (choiceType==currentType)
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
    auto hdlr = handlerCast<ItemPropertyHandler>();

    // update the value
    if (button->property("choiceType").isValid())
    {
        auto newType = button->property("choiceType").toByteArray();
        if (!hdlr->value() || itemType(hdlr->value()) != newType) hdlr->setToNewItemOfType(newType);
    }
    else
    {
        if (hdlr->value()) hdlr->setToNull();
    }

    // make the target item remember that this property was configured by the user
    hdlr->target()->setProperty(hdlr->name()+"_configured", true);

    // signal the change
    emit propertyValidChanged(true);
}

////////////////////////////////////////////////////////////////////
