/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "MainWindow.hpp"

#include "WizardEngine.hpp"
#include <QApplication>
#include <QFileInfo>
#include <QFileOpenEvent>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QSettings>
#include <QStatusBar>
#include <QVBoxLayout>

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
    auto buttonGroupLayout = new QHBoxLayout;
    buttonGroupLayout->addWidget(retreatButton);
    buttonGroupLayout->addWidget(advanceButton);
    auto buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch(2);
    buttonLayout->addLayout(buttonGroupLayout, 1);

    // create the pane that will hold the wizard UI
    _wizardPane = new QWidget;
    _wizardLayout = new QHBoxLayout;
    _wizardLayout->addWidget(_wizardPane);
    auto wizardArea = new QFrame;
    wizardArea->setFrameStyle(QFrame::StyledPanel);
    wizardArea->setLayout(_wizardLayout);

    // create the central area
    auto centralLayout = new QVBoxLayout;
    auto centralArea = new QWidget;
    centralLayout->addWidget(wizardArea);
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
    connect(_wizard, SIGNAL(titleChanged()), this, SLOT(updateTitle()));
    connect(_wizard, SIGNAL(dirtyChanged()), this, SLOT(updateDirtyState()));
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

void MainWindow::updateTitle()
{
    QString filepath = _wizard->filepath();
    QString file = filepath.isEmpty() ? QString("Untitled") : QFileInfo(filepath).fileName();
    QString app = QCoreApplication::applicationName();
    setWindowTitle(file + "[*] - " + app);
}

////////////////////////////////////////////////////////////////////

void MainWindow::updateDirtyState()
{
    setWindowModified(_wizard->isDirty());
}

////////////////////////////////////////////////////////////////////

void MainWindow::keyPressEvent(QKeyEvent* event)
{
    switch (event->key())
    {
    case Qt::Key_Left:
    case Qt::Key_Up:
    case Qt::Key_PageUp:
        _wizard->retreat();
        break;
    case Qt::Key_Right:
    case Qt::Key_Down:
    case Qt::Key_PageDown:
        _wizard->advance();
        break;
    default:
        QMainWindow::keyPressEvent(event);
    }
}

////////////////////////////////////////////////////////////////////

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (_wizard->isDirty())
    {
        auto ret = QMessageBox::warning(this, QCoreApplication::applicationName(),
                                        "Do you want to discard your unsaved changes?",
                                        QMessageBox::Discard | QMessageBox::Cancel, QMessageBox::Cancel);
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
