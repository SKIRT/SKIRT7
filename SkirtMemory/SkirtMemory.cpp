/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "ProcessManager.hpp"
#include <QCoreApplication>
#include "SkirtMemoryCommandLineHandler.hpp"
#include "RegisterSimulationItems.hpp"
#include "SignalHandler.hpp"
#include "SkirtMemory.hpp"
#include <clocale>

#include "git_version.h"

//////////////////////////////////////////////////////////////////////

int main(int argc, char** argv)
{
    // force standard locale so that sprintf (used e.g. in cfitsio) always produces the same result
    setlocale(LC_ALL, "C");

    // initialize remote communication capability, if present
    ProcessManager::initialize(&argc, &argv);

    // construct application object for argument parsing and such,
    // but don't run the event loop because we don't need it
    QCoreApplication app(argc, argv);
    app.setApplicationName("SKIRT");
    app.setApplicationVersion("v7 (git " + QString(git_version).simplified() +
                              " built on " + QString(__DATE__).simplified() + " at "  __TIME__ ")");

    // install C signal handlers (which throw an exception if all goes well)
    SignalHandler::InstallSignalHandlers();

    // initialize the class registry used for discovering simulation items
    RegisterSimulationItems::registerAll();

    // get and handle the command line arguments
    SkirtMemoryCommandLineHandler handler(app.arguments());
    int status = handler.perform();

    // finalize remote communication capability, if present
    ProcessManager::finalize();

    return status;
}

//////////////////////////////////////////////////////////////////////
