/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "Console.hpp"
#include "FatalError.hpp"
#include "FilePaths.hpp"
#include "ParallelFactory.hpp"
#include "PeerToPeerCommunicator.hpp"
#include "Random.hpp"
#include "Simulation.hpp"
#include "SIUnits.hpp"
#include "TimeLogger.hpp"

////////////////////////////////////////////////////////////////////

Simulation::Simulation()
{
    _paths = new FilePaths();
    _paths->setParent(this);
    _log = new Console();
    _log->setParent(this);
    _parfac = new ParallelFactory();
    _parfac->setParent(this);
    _comm = new PeerToPeerCommunicator();
    _comm->setParent(this);
    _random = new Random();
    _random->setParent(this);
    _units = new SIUnits();
    _units->setParent(this);
}

////////////////////////////////////////////////////////////////////

void Simulation::setup()
{
    if (_state > Created) return;

    // ensure the log has been setup before using it
    _log->setup();

    TimeLogger logger(_log, "setup");
    SimulationItem::setup();
}

////////////////////////////////////////////////////////////////////

void Simulation::run()
{
    // verify setup
    if (_state < SetupDone) throw FATALERROR("Simulation has not been setup before being run");

    if (_comm->isMultiProc()) _random->randomize();

    TimeLogger logger(_log, "the simulation run");
    runSelf();
}

////////////////////////////////////////////////////////////////////

void Simulation::setupAndRun()
{
    _log->setup(); // ensure the log is properly setup before first use

    QString processInfo = _comm->isMultiProc() ? " with " + QString::number(_comm->size()) + " processes" : "";
    TimeLogger logger(_log, "simulation " + _paths->outputPrefix() + processInfo);

    // Set the number of threads to 1 for the setup in multiprocessing mode
    int Nthreads = _parfac->maxThreadCount();
    if (_comm->isMultiProc()) _parfac->setMaxThreadCount(1);

    setup();

    // Set the number of threads back to its original value
    if (_comm->isMultiProc()) _parfac->setMaxThreadCount(Nthreads);

    run();
}

////////////////////////////////////////////////////////////////////

FilePaths* Simulation::filePaths() const
{
    return _paths;
}

////////////////////////////////////////////////////////////////////

Log* Simulation::log() const
{
    return _log;
}

////////////////////////////////////////////////////////////////////

ParallelFactory* Simulation::parallelFactory() const
{
    return _parfac;
}

////////////////////////////////////////////////////////////////////

PeerToPeerCommunicator* Simulation::communicator() const
{
    return _comm;
}

////////////////////////////////////////////////////////////////////

void Simulation::setRandom(Random* value)
{
    if (_random) delete _random;
    _random = value;
    if (_random) _random->setParent(this);
}

////////////////////////////////////////////////////////////////////

Random* Simulation::random() const
{
    return _random;
}

////////////////////////////////////////////////////////////////////

void Simulation::setUnits(Units *value)
{
    if (_units) delete _units;
    _units = value;
    if (_units) _units->setParent(this);
}

////////////////////////////////////////////////////////////////////

Units *Simulation::units() const
{
    return _units;
}

////////////////////////////////////////////////////////////////////
