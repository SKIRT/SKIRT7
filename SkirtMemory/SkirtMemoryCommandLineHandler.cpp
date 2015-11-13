/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QSharedPointer>
#include <QHostInfo>
#include "ArrayMemory.hpp"
#include "CommandLineArguments.hpp"
#include "Console.hpp"
#include "ConsoleHierarchyCreator.hpp"
#include "FatalError.hpp"
#include "FileLog.hpp"
#include "FilePaths.hpp"
#include "LatexHierarchyWriter.hpp"
#include "MemoryStatistics.hpp"
#include "PanDustSystem.hpp"
#include "Parallel.hpp"
#include "ParallelFactory.hpp"
#include "PeerToPeerCommunicator.hpp"
#include "ProcessManager.hpp"
#include "RootAssigner.hpp"
#include "MonteCarloSimulation.hpp"
#include "SkirtMemoryCommandLineHandler.hpp"
#include "StopWatch.hpp"
#include "TimeLogger.hpp"
#include "XmlHierarchyCreator.hpp"
#include "XmlHierarchyWriter.hpp"

////////////////////////////////////////////////////////////////////

namespace
{
    // the allowed options list, in the format consumed by the CommandLineArguments constructor
    static const char* allowedOptions = "-i* -o* -k -r";
}

////////////////////////////////////////////////////////////////////

SkirtMemoryCommandLineHandler::SkirtMemoryCommandLineHandler(QStringList cmdlineargs)
    : _args(cmdlineargs, allowedOptions), _hasError(false)
{
    // get the host name
    _hostname = QHostInfo::localHostName();
    if (_hostname.isEmpty()) _username = "unknown host";

    // get the user name
    _username = qgetenv("USER");
    if (_username.isEmpty()) _username = qgetenv("USERNAME");
    if (_username.isEmpty()) _username = "unknown user";

    // issue welcome message
    _console.info("Welcome to " + QCoreApplication::applicationName() + " " + QCoreApplication::applicationVersion());
    _console.info("Running on " + _hostname + " for " + _username);
}

////////////////////////////////////////////////////////////////////

int SkirtMemoryCommandLineHandler::perform()
{
    // catch and properly report any exceptions
    try
    {
        // if there is at least one file path argument --> batch mode
        // otherwise --> error
        if (_args.hasFilepaths()) return doBatch();
        _console.error("Invalid command line arguments");
        printHelp();
    }
    catch (FatalError& error)
    {
        foreach (QString line, error.message()) _console.error(line);
    }
    return EXIT_FAILURE;
}

////////////////////////////////////////////////////////////////////

int SkirtMemoryCommandLineHandler::doBatch()
{
    QStringList skifiles;

    // build a list of filenames for existing ski files
    _hasError = false;
    foreach (QString filepath, _args.filepaths()) skifiles << skifilesFor(filepath);

    // exit if there were any problems with the file paths
    if (_hasError || skifiles.isEmpty())
    {
        printHelp();
        return EXIT_FAILURE;
    }

    // if there is only one ski file, simply perform the single simulation
    if (skifiles.size() == 1)
    {
        _skifilename = skifiles[0];
        doSimulation(); // memory statistics are reported in doSimulation()
    }
    // The SKIRT memory application should only be used for one ski file at a time
    else throw FATALERROR("You cannot run different simulations in parallel with the SKIRT memory application.");

    // report memory statistics for the complete run
    _console.info(MemoryStatistics::reportAvailable(true) + " -- " + MemoryStatistics::reportPeak(true));

    // report stopwatch results, if any
    foreach (QString line, StopWatch::report()) _console.warning(line);
    return EXIT_SUCCESS;
}

////////////////////////////////////////////////////////////////////

QStringList SkirtMemoryCommandLineHandler::skifilesFor(QString filepath)
{
    QStringList result;
    QFileInfo info(filepath);
    QString name = info.fileName();

    // no recursion and no wildcards -> expect a single result
    if (!_args.isPresent("-r") && !name.contains('?') && !name.contains('*'))
    {
        // if the file does not exist as specified, try adding the .ski extension
        if (!QFile::exists(filepath) && !filepath.toLower().endsWith(".ski")) filepath += ".ski";
        if (!QFile::exists(filepath))
        {
            _hasError = true;
            _console.error("This ski file does not exist: " + filepath);
        }
        else result << filepath;
    }

    // recursion and/or wildcards -> multiple results possible
    else
    {
        // find matching files in this directory, possibly recursively (depending on -r option);
        // in this case do not automatically add the .ski extension; it leads to trouble with patterns
        // like "test*" which are automatically expanded by the shell before invoking the application
        result << skifilesFor(info.absoluteDir(), name);

        if (result.isEmpty())
        {
            _hasError = true;
            _console.error("No ski file matches the pattern: " + filepath);
        }
    }

    return result;
}

////////////////////////////////////////////////////////////////////

QStringList SkirtMemoryCommandLineHandler::skifilesFor(const QDir& dir, QString name)
{
    QStringList result;

    // add matching files at the current directory level
    foreach (QString entry, dir.entryList(QStringList() << name,
                                          QDir::Files|QDir::Readable|QDir::NoDotAndDotDot,
                                          QDir::Name|QDir::IgnoreCase) )
    {
        result << dir.absoluteFilePath(entry);
    }

    // if recursion is requested, call ourselves for all directories at this level
    if (_args.isPresent("-r"))
    {
        foreach (QString entry, dir.entryList(QDir::AllDirs|QDir::Readable|QDir::NoDotAndDotDot,
                                              QDir::Name|QDir::IgnoreCase) )
        {
            QDir subdir(dir);
            subdir.cd(entry);
            result << skifilesFor(subdir, name);
        }
    }

    return result;
}

////////////////////////////////////////////////////////////////////

void SkirtMemoryCommandLineHandler::doSimulation()
{
    _console.info("Constructing a simulation from ski file '" + _skifilename + "'...");

    QFileInfo skiinfo(_skifilename);
    QString base = _args.isPresent("-k") ? skiinfo.absolutePath() : QDir::currentPath();

    // Setup the ArrayMemory class
    ArrayMemory::initialize(skiinfo.completeBaseName(), (_args.value("-o").startsWith('/') ? "" : base + "/") + _args.value("-o"));

    // Construct the simulation from the ski file; use shared pointer for automatic clean-up
    XmlHierarchyCreator creator;
    QSharedPointer<MonteCarloSimulation> simulation(creator.createHierarchy<MonteCarloSimulation>(_skifilename));

    // Change the number of photon packages to 1 (we don't care about actually performing it)
    simulation->setPackages(1);

    // Disable dust self-absorption (does not lead to additional memory usage, would not converge anyway)
    try
    {
        simulation->find<PanDustSystem>(false)->setSelfAbsorption(false);
    }
    catch (FatalError&) {}

    // Limit the number of self-absorption cycles to avoid the convergence loop
    //try
    //{
    //simulation->find<PanDustSystem>(false)->setCycles(1);
    //}
    //catch (FatalError&) {}

    // Set up any simulation attributes that are not loaded from the ski file:
    //  - the paths for input and output files
    simulation->filePaths()->setOutputPrefix(skiinfo.completeBaseName());
    simulation->filePaths()->setInputPath((_args.value("-i").startsWith('/') ? "" : base + "/") + _args.value("-i"));
    simulation->filePaths()->setOutputPath((_args.value("-o").startsWith('/') ? "" : base + "/") + _args.value("-o"));

    //  - the number of parallel threads
    simulation->parallelFactory()->setMaxThreadCount(1);

    //  - the multiprocessing environment
    PeerToPeerCommunicator* comm = simulation->communicator();
    comm->setup();

    //  - the console and the file log
    FileLog* log = new FileLog();
    simulation->log()->setLinkedLog(log);
    simulation->log()->setVerbose(true);              // enable verbose logging
    simulation->log()->setMemoryLogging(true);        // enable memory logging
    simulation->log()->setLowestLevel(Log::Error);    // only log error messags to the console

    // Output a ski file and a latex file reflecting this simulation for later reference
    if (comm->isRoot())
    {
        XmlHierarchyWriter writer1;
        writer1.writeHierarchy(simulation.data(), simulation->filePaths()->output("parameters.xml"));
        LatexHierarchyWriter writer2;
        writer2.writeHierarchy(simulation.data(), simulation->filePaths()->output("parameters.tex"));
    }

    // Run the simulation; catch and rethrow exceptions so they are also logged to file
    try
    {
        log->setup();
        log->info(QCoreApplication::applicationName() + " " + QCoreApplication::applicationVersion());
        log->info("Running on " + _hostname + " for " + _username);
        _console.info("Emulating the simulation steps and monitoring memory usage...");
        simulation->setupAndRun();

        // Report memory statistics in the simulation's log file
        log->info(MemoryStatistics::reportAvailable(true) + " -- " + MemoryStatistics::reportPeak(true));
    }
    catch (FatalError& error)
    {
        foreach (QString line, error.message()) log->error(line);
        throw error;
    }
}

////////////////////////////////////////////////////////////////////

void SkirtMemoryCommandLineHandler::printHelp()
{
    _console.warning("");
    _console.warning("To emulate a simulation run while logging its memory requirements: memskirt <ski-filename>");
    _console.warning("");
    _console.warning("  memskirt [-k] [-i <dirpath>] [-o <dirpath>]");
    _console.warning("           [-r] {<filepath>}*");
    _console.warning("");
    _console.warning("  -k : makes the input/output paths relative to the ski file being processed");
    _console.warning("  -i <dirpath> : the relative or absolute path for simulation input files");
    _console.warning("  -o <dirpath> : the relative or absolute path for simulation output files");
    _console.warning("  -r : causes recursive directory descent for all specified ski file paths");
    _console.warning("  <filepath> : the relative or absolute file path for a ski file");
    _console.warning("               (the filename may contain ? and * wildcards)");
    _console.warning("");
}

//////////////////////////////////////////////////////////////////////
