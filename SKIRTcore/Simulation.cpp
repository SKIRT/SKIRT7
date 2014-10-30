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
    _communicator = new PeerToPeerCommunicator();
    _communicator->setParent(this);
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

    TimeLogger logger(_log, "the simulation run");
    runSelf();
}

////////////////////////////////////////////////////////////////////

void Simulation::setupAndRun()
{
    _log->setup(); // ensure the log is properly setup before first use

    QString processInfo = _communicator->isMultiProc() ? " with " + QString::number(_communicator->getSize()) + " processes" : "";

    TimeLogger logger(_log, "simulation " + _paths->outputPrefix() + processInfo);
    setup();

    if (_communicator->isMultiProc()) _random->randomize();

    run();
}

////////////////////////////////////////////////////////////////////

void Simulation::setFilePaths(FilePaths* value)
{
    if (_paths) delete _paths;
    _paths = value;
    if (_paths) _paths->setParent(this);
}

////////////////////////////////////////////////////////////////////

FilePaths* Simulation::filePaths() const
{
    return _paths;
}

////////////////////////////////////////////////////////////////////

void Simulation::setLog(Log* value)
{
    if (_log) delete _log;
    _log = value;
    if (_log) _log->setParent(this);
}

////////////////////////////////////////////////////////////////////

Log* Simulation::log() const
{
    return _log;
}

////////////////////////////////////////////////////////////////////

void Simulation::setParallelFactory(ParallelFactory* value)
{
    if (_parfac) delete _parfac;
    _parfac = value;
    if (_parfac) _parfac->setParent(this);
}

////////////////////////////////////////////////////////////////////

ParallelFactory* Simulation::parallelFactory() const
{
    return _parfac;
}

////////////////////////////////////////////////////////////////////

PeerToPeerCommunicator* Simulation::getCommunicator() const
{
    return _communicator;
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
