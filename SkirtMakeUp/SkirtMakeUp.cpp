/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "SkirtMakeUp.hpp"

#include "FatalError.hpp"
#include "MainWindow.hpp"
#include "RegisterFitSchemeItems.hpp"
#include "RegisterSimulationItems.hpp"
#include "SignalHandler.hpp"
#include <QApplication>
#include <iostream>

#include "git_version.h"

////////////////////////////////////////////////////////////////////

int main(int argc, char** argv)
{
    // construct application object
    QApplication app(argc, argv);
    app.setApplicationName("SkirtMakeUp");
    app.setApplicationVersion("v7 (git " + QString(git_version).simplified() +
                              " built on " + QString(__DATE__).simplified() + " at "  __TIME__ ")");

    // install C signal handlers (which throw an exception if all goes well)
    SignalHandler::InstallSignalHandlers();

    // initialize the class registry used for discovering simulation items
    RegisterSimulationItems::registerAll();
    RegisterFitSchemeItems::registerAll();

    // show the main (and only) window
    MainWindow wizard;
    wizard.show();

    // execute the event loop
    try
    {
        return app.exec();
    }
    catch (FatalError& error)
    {
        foreach (QString line, error.message()) std::cerr << line.toLocal8Bit().constData() << std::endl;
        return EXIT_FAILURE;
    }
}

////////////////////////////////////////////////////////////////////
