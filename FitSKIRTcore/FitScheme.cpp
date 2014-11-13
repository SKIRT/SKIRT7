/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "FitScheme.hpp"

#include "Console.hpp"
#include "FatalError.hpp"
#include "FilePaths.hpp"
#include "MasterSlaveCommunicator.hpp"
#include "Optimization.hpp"
#include "SIUnits.hpp"
#include "TimeLogger.hpp"

////////////////////////////////////////////////////////////////////

FitScheme::FitScheme()
{
    _paths = new FilePaths();
    _paths->setParent(this);
    _log = new Console();
    _log->setParent(this);
    _comm = new MasterSlaveCommunicator();
    _comm->setParent(this);
    _units = new SIUnits();
    _units->setParent(this);
    _parallelSimulations = 1;
    _parallelThreads = 0;
}

////////////////////////////////////////////////////////////////////

void FitScheme::setup()
{
    if (_state > Created) return;

    // ensure the log has been setup before using it
    _log->setup();

    TimeLogger logger(_log, "setup");
    SimulationItem::setup();
}

////////////////////////////////////////////////////////////////////

void FitScheme::run()
{
    // verify setup
    if (_state < SetupDone) throw FATALERROR("Fit scheme has not been setup before being run");

    _comm->acquireSlaves();
    if(_comm->isMaster())
    {
        TimeLogger logger(_log, "fitting");
        runSelf();
    }
    _comm->releaseSlaves();
}

////////////////////////////////////////////////////////////////////

void FitScheme::setupAndRun()
{
    _log->setup(); // ensure the log is properly setup before first use

    QString processInfo = _comm->isMultiProc() ? " with " + QString::number(_comm->getSize()) + " processes" : "";
    TimeLogger logger(_log, "fit scheme " + _paths->outputPrefix() + processInfo);

    setup();
    run();
}

////////////////////////////////////////////////////////////////////

FilePaths* FitScheme::filePaths() const
{
    return _paths;
}

////////////////////////////////////////////////////////////////////

Log* FitScheme::log() const
{
    return _log;
}

////////////////////////////////////////////////////////////////////

void FitScheme::setParallelSimulationCount(int value)
{
    _parallelSimulations = value;
}

////////////////////////////////////////////////////////////////////

int FitScheme::parallelSimulationCount() const
{
    return _parallelSimulations;
}

////////////////////////////////////////////////////////////////////

void FitScheme::setParallelThreadCount(int value)
{
    _parallelThreads = value;
}

////////////////////////////////////////////////////////////////////

int FitScheme::parallelThreadCount() const
{
    return _parallelThreads;
}

////////////////////////////////////////////////////////////////////

void FitScheme::setUnits(Units *value)
{
    if (_units) delete _units;
    _units = value;
    if (_units) _units->setParent(this);
}

////////////////////////////////////////////////////////////////////

Units *FitScheme::units() const
{
    return _units;
}

////////////////////////////////////////////////////////////////////
