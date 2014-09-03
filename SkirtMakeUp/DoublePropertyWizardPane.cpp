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
}

////////////////////////////////////////////////////////////////////

void DoublePropertyWizardPane::updateValue(const QString& text)
{
    auto hdlr = handlerCast<DoublePropertyHandler>();

    // verify that value is valid and within range
    double value = hdlr->toDouble(text);
    if (hdlr->isValid(text) && value >= hdlr->minValue() && value <= hdlr->maxValue())
    {
        hdlr->setValue(value);
        emit propertyValidChanged(true);
    }
    else
    {
        emit propertyValidChanged(false);
    }
}

////////////////////////////////////////////////////////////////////
