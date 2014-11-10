/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "ItemListPropertyWizardPane.hpp"

#include "ItemListPropertyHandler.hpp"
#include "ItemPropertyHandler.hpp"
#include "StringPropertyHandler.hpp"
#include "SimulationItem.hpp"
#include "SimulationItemDiscovery.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QVariant>

using namespace SimulationItemDiscovery;

////////////////////////////////////////////////////////////////////

namespace
{
    // return a description for the specified simulation item containing the item type
    // and the name and value of the first item or string property in the item
    QString descriptionForItem(SimulationItem* item)
    {
        for (auto handler : createPropertyHandlersList(item))
        {
            auto itemhandler = handler.dynamicCast<ItemPropertyHandler>();
            if (itemhandler && itemhandler->value())
            {
                return itemType(item) + " (" + itemhandler->name() + ": " + itemType(itemhandler->value()) + ")";
            }
            auto stringhandler = handler.dynamicCast<StringPropertyHandler>();
            if (stringhandler && !stringhandler->value().isEmpty())
            {
                return itemType(item) + " (" + stringhandler->name() + ": " + stringhandler->value() + ")";
            }
        }
        return itemType(item);
    }
}

////////////////////////////////////////////////////////////////////

ItemListPropertyWizardPane::ItemListPropertyWizardPane(PropertyHandlerPtr handler, QObject* target)
    : PropertyWizardPane(handler, target)
{
    auto hdlr = handlerCast<ItemListPropertyHandler>();

    // connect our edit-sub-item signal to the wizard
    connect(this, SIGNAL(advanceToEditSubItem(int)), target, SLOT(advanceToEditSubItem(int)));

    // ---- construct the overall layout ----

    // create the layout so that we can add stuff one by one
    auto layout = new QVBoxLayout;

    // add the question
    layout->addWidget(new QLabel("Compose " + hdlr->title() + " list:"));

    // add the widget holding the items that represent the contents of this property
    _listWidget = new QListWidget;
    _listWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    layout->addWidget(_listWidget);

    // connect the widget to our respective slot
    connect(_listWidget, SIGNAL(currentRowChanged(int)), this, SLOT(storeSelectedRow(int)));

    // add the push buttons for adding, editing and removing items
    _removeButton = new QPushButton("Remove");
    _editButton = new QPushButton("Edit");
    _addButton = new QPushButton("Add");
    auto buttonLayout = new QHBoxLayout;
    layout->addLayout(buttonLayout);
    buttonLayout->addWidget(_removeButton);
    buttonLayout->addWidget(_editButton);
    buttonLayout->addWidget(_addButton);

    // connect the buttons to our respective slots
    connect(_addButton, SIGNAL(clicked()), this, SLOT(addItem()));
    connect(_editButton, SIGNAL(clicked()), this, SLOT(editItem()));
    connect(_removeButton, SIGNAL(clicked()), this, SLOT(removeItem()));

    // finalize the layout and assign it to ourselves
    layout->addStretch();
    setLayout(layout);

    // ---- populate the list widget with its initial contents ----

    // iterate over all items in the item list
    int index = 0;
    for (auto item : hdlr->value())
    {
        index++;
        QString label = QString::number(index) + ": " + descriptionForItem(item);
        if (!item->property("item_complete").toBool()) label += "  \u2190 editing incomplete !";
        _listWidget->addItem(new QListWidgetItem(label));
    }

    // restore the selection
    int row = retrieveSelectedRow();
    if (row < _listWidget->count()) _listWidget->setCurrentRow(row);

    // enable buttons appropriately
    setButtonsEnabled();
}

////////////////////////////////////////////////////////////////////

void ItemListPropertyWizardPane::addItem()
{
    auto hdlr = handlerCast<ItemListPropertyHandler>();

    // add a new item of the default type to the property's list
    // *** this assumes that an ItemList property always has a default type ***
    auto defaultType = hdlr->defaultItemType();
    for (auto choiceType : descendants(hdlr->baseType()))
    {
        if (SimulationItemDiscovery::inherits(choiceType,defaultType))
        {
            defaultType = choiceType;
            break;
        }
    }
    bool success = hdlr->addNewItemOfType(defaultType);
    if (success) emit propertyValueChanged();

    // add a corresponding line to the list widget
    if (success)
    {
        int count = _listWidget->count();
        _listWidget->addItem(new QListWidgetItem(QString::number(count+1) + ": " + defaultType));
        _listWidget->setCurrentRow(count);
    }

    // start the item edit wizard for the current row
    editItem();
}

////////////////////////////////////////////////////////////////////

void ItemListPropertyWizardPane::editItem()
{
    emit advanceToEditSubItem(retrieveSelectedRow());
}

////////////////////////////////////////////////////////////////////

void ItemListPropertyWizardPane::removeItem()
{
    if (_listWidget->count() > 0)
    {
        int index = _listWidget->currentRow();
        delete _listWidget->takeItem(index);
        auto hdlr = handlerCast<ItemListPropertyHandler>();
        hdlr->removeValueAt(index);
        emit propertyValueChanged();
    }
    setButtonsEnabled();
}

////////////////////////////////////////////////////////////////////

void ItemListPropertyWizardPane::storeSelectedRow(int row)
{
    auto hdlr = handlerCast<ItemListPropertyHandler>();
    hdlr->target()->setProperty(hdlr->name()+"_row", row);
}

////////////////////////////////////////////////////////////////////

int ItemListPropertyWizardPane::retrieveSelectedRow()
{
    auto hdlr = handlerCast<ItemListPropertyHandler>();
    return hdlr->target()->property(hdlr->name()+"_row").toInt();
}

////////////////////////////////////////////////////////////////////

void ItemListPropertyWizardPane::setButtonsEnabled()
{
    // we assume that an item is always selected unless the list is empty
    bool hasItems = _listWidget->count() > 0;

    // enable/disable buttons
    _removeButton->setEnabled(hasItems);
    _editButton->setEnabled(hasItems);
    _addButton->setEnabled(true);

    // validate/invalidate property
    auto hdlr = handlerCast<ItemListPropertyHandler>();
    bool complete = true;
    for (auto item : hdlr->value()) if (!item->property("item_complete").toBool()) complete = false;
    emit propertyValidChanged(complete && (hasItems || hdlr->isOptional()));
}

////////////////////////////////////////////////////////////////////
