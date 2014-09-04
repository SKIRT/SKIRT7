/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include "DoublePropertyWizardPane.hpp"

#include "DoublePropertyHandler.hpp"
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>

////////////////////////////////////////////////////////////////////

DoublePropertyWizardPane::DoublePropertyWizardPane(PropertyHandlerPtr handler, QObject* target)
    : PropertyWizardPane(handler, target)
{
    auto hdlr = handlerCast<DoublePropertyHandler>();

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
    emitPropertyValidChanged(field->text());
}

////////////////////////////////////////////////////////////////////

void DoublePropertyWizardPane::updateValue(const QString& text)
{
    auto hdlr = handlerCast<DoublePropertyHandler>();

    // verify that value is valid and within range before setting it
    double value = hdlr->toDouble(text);
    if (hdlr->isValid(text) && value >= hdlr->minValue() && value <= hdlr->maxValue())
    {
        hdlr->setValue(value);
        setPropertyConfigured();
    }
    emitPropertyValidChanged(text);
}


////////////////////////////////////////////////////////////////////

void DoublePropertyWizardPane::emitPropertyValidChanged(const QString& text)
{
    auto hdlr = handlerCast<DoublePropertyHandler>();

    double value = hdlr->toDouble(text);
    bool valid = hdlr->isValid(text) && value >= hdlr->minValue() && value <= hdlr->maxValue();
    emit propertyValidChanged(valid);
}

////////////////////////////////////////////////////////////////////
