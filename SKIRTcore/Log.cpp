/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "QDateTime"
#include "Log.hpp"
#include "PeerToPeerCommunicator.hpp"
#include "ProcessManager.hpp"

////////////////////////////////////////////////////////////////////

Log::Log()
    : _lowestLevel(Info), _link(0), _procName(QString(""))
{
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

void Log::setProcessName(int rank)
{
    if (_link) _link->setProcessName(rank);
    _procName = "[Process " + QString::number(rank) + "] ";
}

////////////////////////////////////////////////////////////////////

void Log::info(QString message)
{
#ifndef DEBUG
    if (!ProcessManager::isRoot()) return;
#endif

    if (_link) _link->info(message);

#ifdef DEBUG
    if (Info >= _lowestLevel) output(timestamp() + "   " + _procName + message, Info);
#else
    if (Info >= _lowestLevel) output(timestamp() + "   " + message, Info);
#endif
}

////////////////////////////////////////////////////////////////////

void Log::warning(QString message)
{
    if (_link) _link->warning(message);
    if (Warning >= _lowestLevel) output(timestamp() + " ! " + _procName + message, Warning);
}

////////////////////////////////////////////////////////////////////

void Log::success(QString message)
{
#ifndef DEBUG
    if (!ProcessManager::isRoot()) return;
#endif

    if (_link) _link->success(message);

#ifdef DEBUG
    if (Success >= _lowestLevel) output(timestamp() + " - " + _procName + message, Success);
#else
    if (Success >= _lowestLevel) output(timestamp() + " - " + message, Success);
#endif
}

////////////////////////////////////////////////////////////////////

void Log::error(QString message)
{
    if (_link) _link->error(message);
    if (Error >= _lowestLevel) output(timestamp() + " * *** Error: " + _procName + message, Error);
}

////////////////////////////////////////////////////////////////////

QString Log::timestamp()
{
    return QDateTime::currentDateTime().toString("dd/MM/yyyy hh:mm:ss.zzz");
}

////////////////////////////////////////////////////////////////////
