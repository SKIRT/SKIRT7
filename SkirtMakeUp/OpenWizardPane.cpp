/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "OpenWizardPane.hpp"

#include "SimulationItemDiscovery.hpp"
#include "Simulation.hpp"
#include "FatalError.hpp"
#include "FitScheme.hpp"
#include "XmlHierarchyCreator.hpp"
#include <QApplication>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QFileDialog>
#include <QStandardPaths>
#include <QMessageBox>

using namespace SimulationItemDiscovery;

////////////////////////////////////////////////////////////////////

OpenWizardPane::OpenWizardPane(bool skirt, QString filepath, bool dirty, QObject* target)
    : _skirt(skirt), _filepath(filepath), _dirty(dirty)
{
    // connect ourselves to the target
    connect(this, SIGNAL(hierarchyWasLoaded(SimulationItem*, QString)),
            target, SLOT(hierarchyWasLoaded(SimulationItem*, QString)));

    // create the layout so that we can add stuff one by one
    auto layout = new QVBoxLayout;

    // add the caption
    QString filetype = _skirt ? "SKIRT configuration" : "FitScheme";
    layout->addWidget(new QLabel("Press this button to load a " + filetype + " from file:"));

    // add the button
    _openButton = new QPushButton("Open...");
    auto buttonLayout = new QHBoxLayout;
    layout->addLayout(buttonLayout);
    buttonLayout->addWidget(_openButton, 1);

    // add the filepath label
    _filepathLabel = new QLabel();
    _filepathLabel->setWordWrap(true);
    _filepathLabel->setText(_filepath);
    buttonLayout->addWidget(_filepathLabel, 4);

    // connect the button
    connect(_openButton, SIGNAL(clicked()), this, SLOT(open()));

    // finalize the layout and assign it to ourselves
    layout->addStretch();
    setLayout(layout);
}

////////////////////////////////////////////////////////////////////

namespace
{
    // this function sets all properties in a simulation hierarchy to the "user-configured-this-property" state
    // so that property wizard panes won't replace the value by a fresh default (this is somewhat of a hack...);
    // the function calls itself recursively to process the children of the specified root item
    void setAllPropertiesConfigured(SimulationItem* root)
    {
        // process all immediate properties of the specified root item
        for (auto property : properties(root))
        {
            root->setProperty(property+"_configured", true);
        }

        // process all children of the specified root item
        for (auto child : root->children())
        {
            SimulationItem* item = dynamic_cast<SimulationItem*>(child);
            if (item) setAllPropertiesConfigured(item);
        }
    }
}

////////////////////////////////////////////////////////////////////

void OpenWizardPane::open()
{
    // if the current hierarchy is dirty, give the user a chance to opt out
    if (_dirty)
    {
        auto ret = QMessageBox::warning(this, qApp->applicationName(),
                                        "Do you want to discard your unsaved changes?",
                                        QMessageBox::Discard | QMessageBox::Cancel, QMessageBox::Cancel);
        if (ret == QMessageBox::Cancel) return;
    }

    // get a file path from the user
    QString directory = !_filepath.isEmpty() ? _filepath
                                             : QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    QString filetype = _skirt ? "SKIRT configuration" : "FitScheme";
    QString caption = qApp->applicationName() + " - Open " + filetype + " file";
    QString filter = _skirt ? "ski files (*.ski)" : "fski files (*.fski)";
    QString filepath = QFileDialog::getOpenFileName(this, caption, directory, filter);

    // if the user did not cancel, load the file
    if (!filepath.isEmpty())
    {
        // attempt to load the hierarchy from the specified file
        SimulationItem* root = 0;
        QStringList messageLines;
        try
        {
            XmlHierarchyCreator creator;
            if (_skirt) root = creator.createHierarchy<Simulation>(filepath);
            else        root = creator.createHierarchy<FitScheme>(filepath);
        }
        catch (FatalError error)
        {
            root = 0;
            messageLines = error.message();
        }

        // if successful, process the result
        if (root)
        {
            _filepath = filepath;
            _dirty = false;

            // set all properties to the "user-configured-this-property" state
            // so that property wizard panes won't replace the value by a fresh default
            setAllPropertiesConfigured(root);

            // notify the target, handing over ownership for the new hierarchy
            emit hierarchyWasLoaded(root, _filepath);

            // update our UI
            _filepathLabel->setText(_filepath);
        }

        // otherwise alert the user
        else
        {
            QString message = "An error occurred while opening or loading the file:";
            if (messageLines.size() > 0) message += "\n" + messageLines[0];
            if (messageLines.size() > 1) message += "\n" + messageLines[1];
            QMessageBox::critical(this, qApp->applicationName(), message, QMessageBox::Ok);
        }
    }
}

////////////////////////////////////////////////////////////////////
