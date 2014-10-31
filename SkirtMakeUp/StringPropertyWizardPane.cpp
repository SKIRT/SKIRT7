/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "StringPropertyWizardPane.hpp"

#include "StringPropertyHandler.hpp"
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>

////////////////////////////////////////////////////////////////////

StringPropertyWizardPane::StringPropertyWizardPane(PropertyHandlerPtr handler, QObject* target)
    : PropertyWizardPane(handler, target)
{
    auto hdlr = handlerCast<StringPropertyHandler>();

    // create the layout so that we can add stuff one by one
    auto layout = new QVBoxLayout;

    // add the message
    auto message = "Enter " + hdlr->title();
    if (hdlr->hasDefaultValue()) message += " (" + hdlr->defaultValue() + ")";
    message += ":";
    layout->addWidget(new QLabel(message));

    // add the edit field
    auto field = new QLineEdit;
    field->setText(hdlr->value());
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
            field->setText(hdlr->defaultValue());
            hdlr->setValue(hdlr->defaultValue());    // also update the property value
        }
        else
        {
            field->setText("");
        }
    }

    // ensure proper validity state
    emit propertyValidChanged(!field->text().isEmpty());
}

////////////////////////////////////////////////////////////////////

void StringPropertyWizardPane::updateValue(const QString& text)
{
    auto hdlr = handlerCast<StringPropertyHandler>();

    // verify that value is non-empty before setting it
    QString value = text.simplified();
    bool valid = !value.isEmpty();
    if (valid && value!=hdlr->value())
    {
        hdlr->setValue(value);
        emit propertyValueChanged();
    }
    setPropertyConfigured(valid);
    emit propertyValidChanged(valid);
}

////////////////////////////////////////////////////////////////////
