/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "SubItemPropertyWizardPane.hpp"

#include "ItemListPropertyHandler.hpp"
#include "SimulationItemDiscovery.hpp"
#include "SimulationItem.hpp"
#include <QVBoxLayout>
#include <QLabel>
#include <QRadioButton>
#include <QButtonGroup>
#include <QVariant>

using namespace SimulationItemDiscovery;

////////////////////////////////////////////////////////////////////

SubItemPropertyWizardPane::SubItemPropertyWizardPane(PropertyHandlerPtr handler, QObject* target)
    : PropertyWizardPane(handler, target)
{
    auto hdlr = handlerCast<ItemListPropertyHandler>();

    // create the layout so that we can add stuff one by one
    auto layout = new QVBoxLayout;

    // add the question
    layout->addWidget(new QLabel("Select one of the following options for item #"
                                 + QString::number(selectedIndex()+1) + " in " + hdlr->title() + " list:"));

    // determine the current and default item types
    QByteArray currentType = itemType(hdlr->value()[selectedIndex()]);
    QByteArray defaultType = hdlr->hasDefaultValue() ? hdlr->defaultItemType() : "";

    // make a button group to contain the radio buttons reflecting the possible choices
    auto buttonGroup = new QButtonGroup;

    // add the choices
    for (auto choiceType : descendants(hdlr->baseType()))
    {
        auto choiceTitle = title(choiceType);
        if (!choiceTitle.isEmpty()) choiceTitle.replace(0, 1, choiceTitle[0].toUpper());
        if (SimulationItemDiscovery::inherits(choiceType,defaultType)) choiceTitle += "  [default]";
        auto choiceButton = new QRadioButton(choiceTitle);
        buttonGroup->addButton(choiceButton);
        layout->addWidget(choiceButton);

        // associate the item type corresponding to this button with the button object
        choiceButton->setProperty("choiceType", choiceType);

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

int SubItemPropertyWizardPane::selectedIndex()
{
    auto hdlr = handlerCast<ItemListPropertyHandler>();
    return hdlr->target()->property(hdlr->name()+"_row").toInt();
}

////////////////////////////////////////////////////////////////////

void SubItemPropertyWizardPane::selectTypeFor(QAbstractButton* button)
{
    auto hdlr = handlerCast<ItemListPropertyHandler>();

    // update the value if needed
    auto newType = button->property("choiceType").toByteArray();
    int index = selectedIndex();
    if (itemType(hdlr->value()[index]) != newType)
    {
        hdlr->removeValueAt(index);
        hdlr->insertNewItemOfType(index, newType);
        emit propertyValueChanged();
    }

    // signal the change
    emit propertyValidChanged(true);
}

////////////////////////////////////////////////////////////////////
