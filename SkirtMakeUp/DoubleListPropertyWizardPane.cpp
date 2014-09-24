/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include "DoubleListPropertyWizardPane.hpp"

#include "DoubleListPropertyHandler.hpp"
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>

////////////////////////////////////////////////////////////////////

namespace
{
    // returns true if text is a valid double list, and all numbers are within range
    bool isValidAndInRange(DoubleListPropertyHandler* hdlr, QString text)
    {
        if (!hdlr->isValid(text)) return false;
        double mini = hdlr->minValue();
        double maxi = hdlr->maxValue();
        for (double number: hdlr->toDoubleList(text)) if (number < mini || number > maxi) return false;
        return true;
    }
}

////////////////////////////////////////////////////////////////////

DoubleListPropertyWizardPane::DoubleListPropertyWizardPane(PropertyHandlerPtr handler, QObject* target)
    : PropertyWizardPane(handler, target)
{
    auto hdlr = handlerCast<DoubleListPropertyHandler>();

    // create the layout so that we can add stuff one by one
    auto layout = new QVBoxLayout;

    // add the message
    auto message = "Enter " + hdlr->title();
    message += " [" + hdlr->toString(hdlr->minValue()) + "," + hdlr->toString(hdlr->maxValue()) + "]";
    message += ":";
    layout->addWidget(new QLabel(message));

    // add the edit field
    auto field = new QLineEdit;
    field->setText(hdlr->toString(hdlr->value()));
    layout->addWidget(field);

    // connect the field to ourselves
    connect(field, SIGNAL(textEdited(const QString&)), this, SLOT(updateValue(const QString&)));

    // finalize the layout and assign it to ourselves
    layout->addStretch();
    setLayout(layout);

    // if the property was never configured, put the default value or a blank in the text field
    if (!isPropertyConfigured())
    {
        field->setText("");
    }

    // ensure proper validity state
    emit propertyValidChanged(isValidAndInRange(hdlr, field->text()));
}

////////////////////////////////////////////////////////////////////

void DoubleListPropertyWizardPane::updateValue(const QString& text)
{
    auto hdlr = handlerCast<DoubleListPropertyHandler>();

    // verify that text is valid and all numbers are within range before setting value
    bool valid = isValidAndInRange(hdlr, text);
    if (valid) hdlr->setValue(hdlr->toDoubleList(text));
    setPropertyConfigured(valid);
    emit propertyValidChanged(valid);
}

////////////////////////////////////////////////////////////////////
