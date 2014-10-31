/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QSharedPointer>
#include "CommandLineArguments.hpp"
#include "Console.hpp"
#include "ConsoleHierarchyCreator.hpp"
#include "FatalError.hpp"
#include "FileLog.hpp"
#include "FilePaths.hpp"
#include "LatexHierarchyWriter.hpp"
#include "MemoryStatistics.hpp"
#include "Parallel.hpp"
#include "ParallelFactory.hpp"
#include "PeerToPeerCommunicator.hpp"
#include "ProcessManager.hpp"
#include "Simulation.hpp"
#include "SmileSchemaWriter.hpp"
#include "SkirtCommandLineHandler.hpp"
#include "StopWatch.hpp"
#include "TimeLogger.hpp"
#include "XmlHierarchyCreator.hpp"
#include "XmlHierarchyWriter.hpp"

////////////////////////////////////////////////////////////////////

namespace
{
    // the allowed options list, in the format consumed by the CommandLineArguments constructor
    static const char* allowedOptions = "-t* -s* -b -i* -o* -k -r -x";
}

////////////////////////////////////////////////////////////////////

SkirtCommandLineHandler::SkirtCommandLineHandler(QStringList cmdlineargs)
    : _args(cmdlineargs, allowedOptions), _hasError(false), _parallelSims(0)
{
    _console.info("Welcome to " + QCoreApplication::applicationName() + " " + QCoreApplication::applicationVersion());
}

////////////////////////////////////////////////////////////////////

int SkirtCommandLineHandler::perform()
{
    // catch and properly report any exceptions
    try
    {
        // if there are no arguments at all --> interactive mode
        // if there is at least one file path argument --> batch mode
        // if the -x option is present --> export smile schema (undocumented option)
        // otherwise --> error
        if (_args.isValid() && !_args.hasOptions() && !_args.hasFilepaths()) return doInteractive();
        if (_args.hasFilepaths()) return doBatch();
        if (_args.isPresent("-x")) return doSmileSchema();
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

int SkirtCommandLineHandler::doInteractive()
{
    if (ProcessManager::isMultiProc()) throw FATALERROR("Interactive mode cannot be run with multiple processes!");

    _console.info("Interactively constructing a simulation...");

    // ask for the name of the ski file in which to save the result
    QString filename;
    while (true)
    {
        filename = _console.promptForInput("Enter the name of the ski file to be created");

        // reject empty strings
        if (!filename.isEmpty())
        {
            // add .ski extension if needed
            if (!filename.toLower().endsWith(".ski")) filename += ".ski";

            // reject name if file already exists
            if (!QFile::exists(filename))
            {
                // accept the filename
                break;
            }
            else _console.error("This file already exists; enter another name");
        }
        else _console.error("Enter a nonempty string");
    }

    // interactively construct the simulation; use shared pointer for automatic clean-up
    ConsoleHierarchyCreator creator;
    QSharedPointer<Simulation> simulation( creator.createHierarchy<Simulation>() );

    // create the ski file reflecting this simulation
    XmlHierarchyWriter writer;
    writer.writeHierarchy(simulation.data(), filename);
    _console.info("Successfully created ski file '" + filename + "'.");
    _console.info("To run the simulation use the command: skirt " + filename.left(filename.length()-4));

    return EXIT_SUCCESS;
}

////////////////////////////////////////////////////////////////////

int SkirtCommandLineHandler::doBatch()
{
    // build a list of filenames for existing ski files
    _hasError = false;
    foreach (QString filepath, _args.filepaths()) _skifiles << skifilesFor(filepath);

    // exit if there were any problems with the file paths
    if (_hasError || _skifiles.isEmpty())
    {
        if (!_args.isPresent("-b")) printHelp();
        return EXIT_FAILURE;
    }

    // if there is only one ski file, simply perform the single simulation
    if (_skifiles.size() == 1)
    {
        doSimulation(0); // memory statistics are reported in doSimulation()
    }
    else
    {
        // determine the number of parallel simulations
        _parallelSims = std::max(_args.intValue("-s"), 1);

        // perform a simulation for each ski file
        TimeLogger logger(&_console, "a set of " + QString::number(_skifiles.size()) + " simulations"
                          + (_parallelSims > 1 ? ", " + QString::number(_parallelSims) + " in parallel" : ""));
        ParallelFactory factory;
        factory.setMaxThreadCount(_parallelSims);
        factory.parallel()->call(this, &SkirtCommandLineHandler::doSimulation, _skifiles.size());
    }

    // report memory statistics for the complete run
    _console.info(MemoryStatistics::reportPeak());

    // report stopwatch results, if any
    foreach (QString line, StopWatch::report()) _console.warning(line);
    return EXIT_SUCCESS;
}

////////////////////////////////////////////////////////////////////

int SkirtCommandLineHandler::doSmileSchema()
{
    SmileSchemaWriter writer;
    writer.writeSmileSchema();
    _console.info("Successfully created smile schema file 'skirt.smile'.");
    return EXIT_SUCCESS;
}

////////////////////////////////////////////////////////////////////

QStringList SkirtCommandLineHandler::skifilesFor(QString filepath)
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

QStringList SkirtCommandLineHandler::skifilesFor(const QDir& dir, QString name)
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

void SkirtCommandLineHandler::doSimulation(size_t index)
{
    QString filename = _skifiles[index];
    if (_skifiles.size() > 1) _console.warning("Performing simulation #" + QString::number(index+1) +
                                               " of " + QString::number(_skifiles.size()));
    _console.info("Constructing a simulation from ski file '" + filename + "'...");

    // construct the simulation from the ski file; use shared pointer for automatic clean-up
    XmlHierarchyCreator creator;
    QSharedPointer<Simulation> simulation( creator.createHierarchy<Simulation>(filename) );

    // setup any simulation attributes that are not loaded from the ski file
    // file paths
    QFileInfo skiinfo(filename);
    simulation->filePaths()->setOutputPrefix(skiinfo.completeBaseName());
    QString base = _args.isPresent("-k") ? skiinfo.absolutePath() : QDir::currentPath();
    simulation->filePaths()->setInputPath((_args.value("-i").startsWith('/') ? "" : base + "/") + _args.value("-i"));
    simulation->filePaths()->setOutputPath((_args.value("-o").startsWith('/') ? "" : base + "/") + _args.value("-o"));
    // threads
    if (_args.intValue("-t") > 0) simulation->parallelFactory()->setMaxThreadCount(_args.intValue("-t"));
    // console
    FileLog* log = new FileLog();
    simulation->log()->setLinkedLog(log);
    if (_parallelSims > 1 || _args.isPresent("-b")) simulation->log()->setLowestLevel(Log::Success);

    // output a ski file and a latex file reflecting this simulation for later reference
    PeerToPeerCommunicator* communicator = simulation->getCommunicator();
    if (communicator->isRoot())
    {
        XmlHierarchyWriter writer1;
        writer1.writeHierarchy(simulation.data(), simulation->filePaths()->output("parameters.xml"));
        LatexHierarchyWriter writer2;
        writer2.writeHierarchy(simulation.data(), simulation->filePaths()->output("parameters.tex"));
    }

    // run the simulation; catch and rethrow exceptions so they are also logged to file
    try
    {
        log->setup();
        log->info(QCoreApplication::applicationName() + " " + QCoreApplication::applicationVersion());
        simulation->setupAndRun();

        // if this is the only or first simulation in the run, report memory statistics in the simulation's log file
        if (_skifiles.size() == 1 || (_parallelSims==1 && index==0))
            log->info(MemoryStatistics::reportPeak());
    }
    catch (FatalError& error)
    {
        foreach (QString line, error.message()) log->error(line);
        throw error;
    }
}

////////////////////////////////////////////////////////////////////

void SkirtCommandLineHandler::printHelp()
{
    _console.warning("");
    _console.warning("To create a new ski file interactively:    skirt");
    _console.warning("To run a simulation with default options:  skirt <ski-filename>");
    _console.warning("");
    _console.warning("  skirt [-b] [-s <simulations>] [-t <threads>]");
    _console.warning("        [-k] [-i <dirpath>] [-o <dirpath>]");
    _console.warning("        [-r] {<filepath>}*");
    _console.warning("");
    _console.warning("  -b : forces brief console logging");
    _console.warning("  -s <simulations> : the number of parallel simulations per process");
    _console.warning("  -t <threads> : the number of parallel threads for each simulation");
    _console.warning("  -k : makes the input/output paths relative to the ski file being processed");
    _console.warning("  -i <dirpath> : the relative or absolute path for simulation input files");
    _console.warning("  -o <dirpath> : the relative or absolute path for simulation output files");
    _console.warning("  -r : causes recursive directory descent for all specified ski file paths");
    _console.warning("  <filepath> : the relative or absolute file path for a ski file");
    _console.warning("               (the filename may contain ? and * wildcards)");
    _console.warning("");
}

//////////////////////////////////////////////////////////////////////
