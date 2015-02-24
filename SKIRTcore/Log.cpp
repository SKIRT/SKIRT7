/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "FatalError.hpp"
#include "QDateTime"
#include "Log.hpp"
#include "ProcessCommunicator.hpp"
#include "ProcessManager.hpp"

////////////////////////////////////////////////////////////////////

Log::Log()
    : _lowestLevel(Info), _link(0), _verbose(false)
{
}

////////////////////////////////////////////////////////////////////

void Log::setupSelfBefore()
{
    ProcessCommunicator* comm;

    try
    {
        // get a pointer to the ProcessCommunicator without performing setup
        // to avoid catching (and hiding) fatal errors during such setup
        comm = find<ProcessCommunicator>(false);
    }
    catch (FatalError)
    {
        return;
    }

    // Do the find operation again, now to perform the setup of the
    // PeerToPeerCommunicator so that the correct rank is initialized
    comm = find<ProcessCommunicator>();

    if (comm->isMultiProc()) setRank(comm->rank());
}

////////////////////////////////////////////////////////////////////

void Log::setRank(int rank)
{
    if (_link) _link->setRank(rank);
    _procNameShort = QString("P%1").arg(rank, 3, 10, QChar('0'));
    _procNameLong = "[" + _procNameShort + "] ";
}

////////////////////////////////////////////////////////////////////

void Log::setLowestLevel(Log::Level level)
{
    _lowestLevel = level;
}

////////////////////////////////////////////////////////////////////

Log::Level Log::lowestLevel() const
{
    return _lowestLevel;
}

////////////////////////////////////////////////////////////////////

void Log::setLinkedLog(Log* log)
{
    if (_link) delete _link;
    _link = log;
    if (_link) _link->setParent(this);
}

////////////////////////////////////////////////////////////////////

Log* Log::linkedLog() const
{
    return _link;
}

////////////////////////////////////////////////////////////////////

void Log::setVerbose(bool value)
{
    _verbose = value;
    if (_link) _link->setVerbose(value);
}

////////////////////////////////////////////////////////////////////

bool Log::verbose() const
{
    return _verbose;
}

////////////////////////////////////////////////////////////////////

void Log::info(QString message)
{
    if (_link) _link->info(message);

    if (verbose())
    {
        if (Info >= _lowestLevel) output(timestamp() + "   " + _procNameLong + message, Info);
    }
    else if (ProcessManager::isRoot())
    {
        if (Info >= _lowestLevel) output(timestamp() + "   " + message, Info);
    }
}

////////////////////////////////////////////////////////////////////

void Log::warning(QString message)
{
    if (_link) _link->warning(message);
    if (Warning >= _lowestLevel) output(timestamp() + " ! " + _procNameLong + message, Warning);
}

////////////////////////////////////////////////////////////////////

void Log::success(QString message)
{
    if (_link) _link->success(message);

    if (verbose())
    {
        if (Success >= _lowestLevel) output(timestamp() + " - " + _procNameLong + message, Success);
    }
    else if (ProcessManager::isRoot())
    {
        if (Success >= _lowestLevel) output(timestamp() + " - " + message, Success);
    }
}

////////////////////////////////////////////////////////////////////

void Log::error(QString message)
{
    if (_link) _link->error(message);
    if (Error >= _lowestLevel) output(timestamp() + " * " + _procNameLong + "*** Error: " + message, Error);
}

////////////////////////////////////////////////////////////////////

QString Log::timestamp()
{
    return QDateTime::currentDateTime().toString("dd/MM/yyyy hh:mm:ss.zzz");
}

////////////////////////////////////////////////////////////////////

QString Log::processName()
{
    return _procNameShort;
}

////////////////////////////////////////////////////////////////////
