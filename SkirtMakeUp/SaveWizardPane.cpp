/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "SaveWizardPane.hpp"

#include "SimulationItemDiscovery.hpp"
#include "XmlHierarchyWriter.hpp"
#include <QApplication>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QFileDialog>
#include <QStandardPaths>

using namespace SimulationItemDiscovery;

////////////////////////////////////////////////////////////////////

SaveWizardPane::SaveWizardPane(SimulationItem* root, QString filepath, bool dirty, QObject* target)
{
    // remember the constructor arguments
    _root = root;
    _filepath = filepath;
    _dirty = dirty;

    // connect ourselves to the target
    connect(this, SIGNAL(hierarchyWasSaved(QString)), target, SLOT(hierarchyWasSaved(QString)));

    // discover the type of hierarchy
    bool skirt = SimulationItemDiscovery::inherits(itemType(_root), "MonteCarloSimulation");
    QString filetype = skirt ? "SKIRT configuration" : "FitScheme";

    // create the layout so that we can add stuff one by one
    auto layout = new QVBoxLayout;

    // ---- save ----
    {
        // add the caption
        layout->addWidget(new QLabel("Press this button to save the " + filetype + " into the same file:"));

        // add the button
        _saveButton = new QPushButton("Save");
        auto buttonLayout = new QHBoxLayout;
        layout->addLayout(buttonLayout);
        buttonLayout->addWidget(_saveButton, 1);

        // add the filepath label
        _filepathLabel = new QLabel();
        _filepathLabel->setWordWrap(true);
        buttonLayout->addWidget(_filepathLabel, 4);

        // connect the button
        connect(_saveButton, SIGNAL(clicked()), this, SLOT(save()));
    }

    // ---- save as ----
    {
        // add the caption
        layout->addWidget(new QLabel("Press this button to save the " + filetype + " into a new file:"));

        // add the button
        _saveAsButton = new QPushButton("Save As...");
        auto buttonLayout = new QHBoxLayout;
        layout->addLayout(buttonLayout);
        buttonLayout->addWidget(_saveAsButton, 1);
        buttonLayout->addStretch(4);

        // connect the button
        connect(_saveAsButton, SIGNAL(clicked()), this, SLOT(saveAs()));
    }

    // ---- quit ----
    {
        // add the caption
        layout->addWidget(new QLabel("Press this button or close the window to exit the wizard:"));

        // add the button
        _quitButton = new QPushButton("Quit");
        auto buttonLayout = new QHBoxLayout;
        layout->addLayout(buttonLayout);
        buttonLayout->addWidget(_quitButton, 1);
        buttonLayout->addStretch(4);

        // connect the button
        connect(_quitButton, SIGNAL(clicked()), this, SLOT(quit()));
    }

    // --------

    // finalize the layout and assign it to ourselves
    layout->addStretch();
    setLayout(layout);

    // enable/disable save button and fill the filepath label
    updateSaveInfo();
}

////////////////////////////////////////////////////////////////////

void SaveWizardPane::save()
{
    // if the previous path is known, save again
    if (!_filepath.isEmpty()) saveToFile(_filepath);
}

////////////////////////////////////////////////////////////////////

void SaveWizardPane::saveAs()
{
    // discover the type of hierarchy
    bool skirt = SimulationItemDiscovery::inherits(itemType(_root), "MonteCarloSimulation");

    // get a file path from the user
    QString directory = !_filepath.isEmpty() ? _filepath
                                             : QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    QString caption = qApp->applicationName() + " - Save "
                      + QString(skirt ? "SKIRT parameter file" : "FitScheme file");
    QString filter = skirt ? "ski files (*.ski)" : "fski files (*.fski)";
    QString filepath = QFileDialog::getSaveFileName(this, caption, directory, filter);

    // if the user did not cancel, save the file
    if (!filepath.isEmpty())
    {
        // add ski/fski extension if needed
        QString extension = skirt ? ".ski" : ".fski";
        if (!filepath.endsWith(extension)) filepath += extension;

        saveToFile(filepath);
    }
}

////////////////////////////////////////////////////////////////////

void SaveWizardPane::quit()
{
    qApp->closeAllWindows();
}

////////////////////////////////////////////////////////////////////

void SaveWizardPane::saveToFile(QString filepath)
{
    // save the hierarchy in the specified file
    XmlHierarchyWriter writer;
    writer.writeHierarchy(_root, filepath);
    _filepath = filepath;
    _dirty = false;

    // notify the target
    emit hierarchyWasSaved(filepath);

    // update our UI
    updateSaveInfo();
}

////////////////////////////////////////////////////////////////////

void SaveWizardPane::updateSaveInfo()
{
    _saveButton->setEnabled(!_filepath.isEmpty() && _dirty);
    _filepathLabel->setText(_filepath);
}

////////////////////////////////////////////////////////////////////
