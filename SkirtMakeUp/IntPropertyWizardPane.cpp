/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include "IntPropertyWizardPane.hpp"

#include "IntPropertyHandler.hpp"
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>

////////////////////////////////////////////////////////////////////

namespace
{
    // returns true if text is a valid integer, and the value is within range
    bool isValidAndInRange(IntPropertyHandler* hdlr, QString text)
    {
        if (!hdlr->isValid(text)) return false;
        int value = hdlr->toInt(text);
        if (value < hdlr->minValue() || value > hdlr->maxValue()) return false;
        return true;
    }
}

////////////////////////////////////////////////////////////////////

IntPropertyWizardPane::IntPropertyWizardPane(PropertyHandlerPtr handler, QObject* target)
    : PropertyWizardPane(handler, target)
{
    auto hdlr = handlerCast<IntPropertyHandler>();

    // create the layout so that we can add stuff one by one
    auto layout = new QVBoxLayout;

    // add the message
    auto message = "Enter " + hdlr->title();
    message += " [" + hdlr->toString(hdlr->minValue()) + "," + hdlr->toString(hdlr->maxValue()) + "]";
    if (hdlr->hasDefaultValue()) message += " (" + hdlr->toString(hdlr->defaultValue()) + ")";
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
        if (hdlr->hasDefaultValue())
        {
            field->setText(hdlr->toString(hdlr->defaultValue()));
            hdlr->setValue(hdlr->defaultValue());    // also update the property value
        }
        else
        {
            field->setText("");
        }
    }

    // ensure proper validity state
    emit propertyValidChanged(isValidAndInRange(hdlr, field->text()));
}

////////////////////////////////////////////////////////////////////

void IntPropertyWizardPane::updateValue(const QString& text)
{
    auto hdlr = handlerCast<IntPropertyHandler>();

    // verify that value is valid and within range before setting it
    bool valid = isValidAndInRange(hdlr, text);
    if (valid) hdlr->setValue(hdlr->toInt(text));
    setPropertyConfigured(valid);
    emit propertyValidChanged(valid);
}

////////////////////////////////////////////////////////////////////
