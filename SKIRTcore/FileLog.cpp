/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "FatalError.hpp"
#include "FileLog.hpp"
#include "FilePaths.hpp"
#include "PeerToPeerCommunicator.hpp"

////////////////////////////////////////////////////////////////////

FileLog::FileLog()
    : _limit(0)
{
}

////////////////////////////////////////////////////////////////////

FileLog::~FileLog()
{
    if (_file.isOpen())
    {
        _out.flush();
        _file.close();
    }
}

////////////////////////////////////////////////////////////////////

void FileLog::setLimit(double value)
{
    _limit = value;
}

////////////////////////////////////////////////////////////////////

void FileLog::setupSelfBefore()
{
    // Call the setup of the base class first, to ensure the string identifying the process is set.
    Log::setupSelfBefore();

    PeerToPeerCommunicator* comm = find<PeerToPeerCommunicator>();

    // If not in verbose mode, the log file for a process that is not the root needn't be
    // created at this point; only when an error or a warning is encountered from this process.
    if (!comm->isRoot() && !verbose()) return;

    // Open the log output file
    open();
}

////////////////////////////////////////////////////////////////////

void FileLog::open()
{
    PeerToPeerCommunicator* comm = find<PeerToPeerCommunicator>();

    QString filepath;
    if (comm->isRoot())
    {
        filepath = find<FilePaths>()->output("log.txt");
    }
    else
    {
        filepath = find<FilePaths>()->output( "log" + processName() + ".txt");
    }
    _file.setFileName(filepath);
    if (!_file.open(QIODevice::WriteOnly | QIODevice::Text))
        throw FATALERROR("Could not open the log file " + filepath);

    _out.setDevice(&_file);
    _out.setCodec("UTF-8");
}

////////////////////////////////////////////////////////////////////

void FileLog::output(QString message, Log::Level level)
{
    if (!_file.isOpen() && (level == Warning || level == Error))
    {
        open();
    }

    if (_file.isOpen())
    {
        std::unique_lock<std::mutex> lock(_mutex);
        _out << message << endl;
    }
}

////////////////////////////////////////////////////////////////////

void FileLog::memory(size_t oldsize, size_t newsize, void *ptr)
{
    // Calculate the change in memory (in GB)
    double delta = 0;
    if (oldsize < newsize) delta = (newsize - oldsize) * 8 * 1e-9;
    else if (oldsize > newsize) delta = (oldsize - newsize) * 8 * 1e-9;

    // Log the amount of gained or released memory to the console, if larger than a certain threshold
    if (delta >= _limit)
    {
        // Convert the pointer to a unique string
        QString address = QString("0x%1").arg((quintptr)ptr, QT_POINTER_SIZE * 2, 16, QChar('0'));

        if (oldsize < newsize) info("+" + QString::number(delta) + " GB for " + address);
        else if (oldsize > newsize) info("-" + QString::number(delta) + " GB for " + address);
    }
}

////////////////////////////////////////////////////////////////////
