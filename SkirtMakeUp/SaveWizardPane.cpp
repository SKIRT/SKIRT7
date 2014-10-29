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

SaveWizardPane::SaveWizardPane(SimulationItem* root, QString filepath, QObject* target)
{
    // remember the constructor arguments
    _root = root;
    _filepath = filepath;

    // connect ourselves to the target
    connect(this, SIGNAL(hierarchyWasSaved(QString)), target, SLOT(hierarchyWasSaved(QString)));

    // create the layout so that we can add stuff one by one
    auto layout = new QVBoxLayout;

    // add the caption, depending on the type of hierarchy
    bool skirt = SimulationItemDiscovery::inherits(itemType(_root), "MonteCarloSimulation");
    QString filetype = skirt ? "SKIRT parameter file" : "FitScheme file";
    layout->addWidget(new QLabel("Press the appropriate button to save the " + filetype + ":"));

    // add the push buttons for saving
    _saveButton = new QPushButton("Save");
    _saveAsButton = new QPushButton("Save As...");
    auto buttonLayout = new QHBoxLayout;
    layout->addLayout(buttonLayout);
    buttonLayout->addWidget(_saveButton, 1);
    buttonLayout->addWidget(_saveAsButton, 1);
    buttonLayout->addStretch(2);

    // connect the buttons to our respective slots
    connect(_saveButton, SIGNAL(clicked()), this, SLOT(save()));
    connect(_saveAsButton, SIGNAL(clicked()), this, SLOT(saveAs()));

    // add the exit caption
    layout->addWidget(new QLabel("Close the window to exit the wizard."));

    // finalize the layout and assign it to ourselves
    layout->addStretch();
    setLayout(layout);
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
    if (!filepath.isEmpty()) saveToFile(filepath);
}

void SaveWizardPane::saveToFile(QString filepath)
{
    // save the hierarchy in the specified file
    XmlHierarchyWriter writer;
    writer.writeHierarchy(_root, filepath);
    _filepath = filepath;

    // notify the target
    emit hierarchyWasSaved(filepath);
}

////////////////////////////////////////////////////////////////////
