/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include "MainWindow.hpp"

#include "WizardEngine.hpp"
#include <QApplication>
#include <QFileOpenEvent>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QSettings>
#include <QStatusBar>

////////////////////////////////////////////////////////////////////

MainWindow::MainWindow()
{
    // setup the window, restoring previous position and size
    setWindowTitle(QCoreApplication::applicationName());
    setMinimumSize(680, 510);
    readSettings();

    // create the status bar
    statusBar()->addPermanentWidget(new QLabel(QCoreApplication::applicationVersion()));
    statusBar()->showMessage("Ready to roll", 3000);

    // create the pane that holds the buttons to drive the wizard
    auto advanceButton = new QPushButton("Continue");
    auto retreatButton = new QPushButton("Back");
    auto buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(retreatButton);
    buttonLayout->addWidget(advanceButton);

    // create the pane that will hold the wizard UI
    _wizardPane = new QLabel("Start");
    _wizardLayout = new QHBoxLayout;
    _wizardLayout->addWidget(_wizardPane);

    // create the central area
    auto centralLayout = new QVBoxLayout;
    auto centralArea = new QWidget;
    centralLayout->addLayout(_wizardLayout);
    centralLayout->addLayout(buttonLayout);
    centralArea->setLayout(centralLayout);
    setCentralWidget(centralArea);

    // create the wizard engine and connect it into our UI
    _wizard = new WizardEngine(this);
    connect(advanceButton, SIGNAL(clicked()), _wizard, SLOT(advance()));
    connect(retreatButton, SIGNAL(clicked()), _wizard, SLOT(retreat()));
    connect(_wizard, SIGNAL(canAdvanceChangedTo(bool)), advanceButton, SLOT(setEnabled(bool)));
    connect(_wizard, SIGNAL(canRetreatChangedTo(bool)), retreatButton, SLOT(setEnabled(bool)));
    connect(_wizard, SIGNAL(stateChanged()), this, SLOT(replaceWizardPane()));
    _wizard->emitStateChanged();
}

////////////////////////////////////////////////////////////////////

void MainWindow::readSettings()
{
    QSettings settings;
    QPoint pos = settings.value("mainpos", QPoint(200, 200)).toPoint();
    QSize size = settings.value("mainsize", QSize(400, 400)).toSize();
    resize(size);
    move(pos);
}

////////////////////////////////////////////////////////////////////

void MainWindow::writeSettings()
{
    QSettings settings;
    settings.setValue("mainpos", pos());
    settings.setValue("mainsize", size());
}

////////////////////////////////////////////////////////////////////

void MainWindow::replaceWizardPane()
{
    _wizardLayout->removeWidget(_wizardPane);
    delete _wizardPane;
    _wizardPane = _wizard->createPane();
    _wizardLayout->addWidget(_wizardPane);
}

////////////////////////////////////////////////////////////////////

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (_wizard->isDirty())
    {
        auto ret = QMessageBox::warning(this, QCoreApplication::applicationName(),
                                        "Do you want to discard your unsaved changes?",
                                        QMessageBox::Discard | QMessageBox::Cancel);
        if (ret == QMessageBox::Cancel)
        {
            event->ignore();
            return;
        }
    }

    writeSettings();
    event->accept();
}

////////////////////////////////////////////////////////////////////
