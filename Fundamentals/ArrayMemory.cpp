/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifdef BUILDING_MEMORY
#include <QDateTime>
#include <QFileInfo>
#include "ArrayMemory.hpp"
#include "ProcessManager.hpp"
#endif

////////////////////////////////////////////////////////////////////

#ifdef BUILDING_MEMORY
// Declare variables in an anonymous namespace
namespace
{
    // A flag indicating whether the initialize() function has been called
    bool _initialized(false);

    // Output prefix and path
    QString _outputPath("");
    QString _outputPrefix("");

    // Strings identifying the process
    QString _procNameShort("");
    QString _procNameLong("");

    // Objects used for writing to file
    QFile _file;
    QTextStream _out;

    // The lowest amount of memory (in GB) to be reported
    double _limit;
}
#endif

//////////////////////////////////////////////////////////////////////

#ifdef BUILDING_MEMORY
void ArrayMemory::initialize(QString prefix, QString path, double limit)
{
    // Set the output prefix
    _outputPrefix = prefix;

    // Set the lower memory limit
    _limit = limit;

    // Set the output path
    if (path.isEmpty()) _outputPath = "";
    else
    {
        QFileInfo test(path);
        if (!test.isDir()) throw std::runtime_error("Output path does not exist or is not a directory: " + path.toStdString());
        _outputPath = test.canonicalFilePath() + "/";
    }

    // Set the process name
    if (ProcessManager::isMultiProc())
    {
        _procNameShort = QString("P%1").arg(ProcessManager::rank(), 3, 10, QChar('0'));
        _procNameLong = "[" + _procNameShort + "] ";
    }

    // Determine the path of the log file
    QString filepath;
    if (ProcessManager::isRoot())
    {
        filepath = outFilePath("memory.txt");
    }
    else
    {
        filepath = outFilePath( "memory" + _procNameShort + ".txt");
    }

    _file.setFileName(filepath);
    if (!_file.open(QIODevice::WriteOnly | QIODevice::Text))
        throw std::runtime_error("Could not open the log file " + filepath.toStdString());

    _out.setDevice(&_file);
    _out.setCodec("UTF-8");

    // If everything went well, set the _initialized flag
    _initialized = true;
}
#endif

//////////////////////////////////////////////////////////////////////

#ifdef BUILDING_MEMORY
void ArrayMemory::finalize()
{
    if (_file.isOpen())
    {
        _out.flush();
        _file.close();
    }
}
#endif

//////////////////////////////////////////////////////////////////////

#ifdef BUILDING_MEMORY
QString ArrayMemory::outFilePath(QString name)
{
    return _outputPath + _outputPrefix + "_" + name;
}
#endif

//////////////////////////////////////////////////////////////////////

#ifdef BUILDING_MEMORY
void ArrayMemory::log_resize(size_t oldsize, size_t newsize, void* ptr)
{
    // Return immediately if the initialize() function was not called
    if (!_initialized) return;

    // Calculate the change in memory (in GB)
    double delta;
    if (oldsize < newsize) delta = (newsize - oldsize) * 8 * 1e-9;
    else if (oldsize > newsize) delta = (oldsize - newsize) * 8 * 1e-9;
    else delta = 0;

    // Convert the pointer to a unique string
    QString address = QString("0x%1").arg((quintptr)ptr, QT_POINTER_SIZE * 2, 16, QChar('0'));

    // Log the amount of gained or released memory to the console, if larger than a certain threshold
    if (delta > _limit)
    {
        if (oldsize < newsize) log("+" + QString::number(delta) + " GB at " + address);
        else if (oldsize > newsize) log("-" + QString::number(delta) + " GB at " + address);
    }
}
#endif

//////////////////////////////////////////////////////////////////////

#ifdef BUILDING_MEMORY
void ArrayMemory::log(QString message)
{
    output(timestamp() + "   " + _procNameLong + message);
}
#endif

//////////////////////////////////////////////////////////////////////

#ifdef BUILDING_MEMORY
void ArrayMemory::output(QString message)
{
    if (_file.isOpen())
    {
        //std::unique_lock<std::mutex> lock(_mutex);
        _out << message << endl;
    }
}
#endif

//////////////////////////////////////////////////////////////////////

#ifdef BUILDING_MEMORY
QString ArrayMemory::timestamp()
{
    return QDateTime::currentDateTime().toString("dd/MM/yyyy hh:mm:ss.zzz");
}
#endif

//////////////////////////////////////////////////////////////////////

