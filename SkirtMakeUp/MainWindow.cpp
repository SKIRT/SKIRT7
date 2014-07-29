/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include "MainWindow.hpp"

#include <QApplication>
#include <QFileOpenEvent>
#include <QLabel>
#include <QSettings>
#include <QStatusBar>

////////////////////////////////////////////////////////////////////

MainWindow::MainWindow()
{
    // set a minimum window size
    setMinimumSize(680, 510);

    // set the window title
    setWindowTitle(QCoreApplication::applicationName());

    // create the status bar
    statusBar()->addPermanentWidget(new QLabel(QCoreApplication::applicationVersion()));
    statusBar()->showMessage("Ready to roll", 3000);

    // restore previous window position and size
    readSettings();
}

/////////////////////////////////////////////////////////////////////////

void MainWindow::readSettings()
{
    QSettings settings;
    QPoint pos = settings.value("mainpos", QPoint(200, 200)).toPoint();
    QSize size = settings.value("mainsize", QSize(400, 400)).toSize();
    resize(size);
    move(pos);
}

/////////////////////////////////////////////////////////////////////////

void MainWindow::writeSettings()
{
    QSettings settings;
    settings.setValue("mainpos", pos());
    settings.setValue("mainsize", size());
}

/////////////////////////////////////////////////////////////////////////

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (false)
    {
        event->ignore();
    }
    else
    {
        writeSettings();
        event->accept();
    }
}

////////////////////////////////////////////////////////////////////
