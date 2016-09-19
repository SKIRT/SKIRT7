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
#include "AllCellsDustLib.hpp"
#include "Array.hpp"
#include "CommandLineArguments.hpp"
#include "Console.hpp"
#include "ConsoleHierarchyCreator.hpp"
#include "FatalError.hpp"
#include "FileLog.hpp"
#include "FilePaths.hpp"
#include "LatexHierarchyWriter.hpp"
#include "MemoryStatistics.hpp"
#include "MonteCarloSimulation.hpp"
#include "PanDustSystem.hpp"
#include "Parallel.hpp"
#include "ParallelFactory.hpp"
#include "PeerToPeerCommunicator.hpp"
#include "ProcessManager.hpp"
#include "Simulation.hpp"
#include "SmileSchemaWriter.hpp"
#include "SkirtCommandLineHandler.hpp"
#include "StopWatch.hpp"
#include "TimeLogger.hpp"
#include "WavelengthGrid.hpp"
#include "XmlHierarchyCreator.hpp"
#include "XmlHierarchyWriter.hpp"

////////////////////////////////////////////////////////////////////

namespace
{
    // the allowed options list, in the format consumed by the CommandLineArguments constructor
    static const char* allowedOptions = "-t* -s* -d -b -v -m -l* -e -k -i* -o* -r -x";
}

////////////////////////////////////////////////////////////////////

SkirtCommandLineHandler::SkirtCommandLineHandler(QStringList cmdlineargs)
    : _args(cmdlineargs, allowedOptions), _hasError(false), _parallelSims(0)
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
    if (ProcessManager::isMultiProc()) throw FATALERROR("Interactive mode cannot be run with multiple processes");

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

        // prevent different simulations to be launched at once while MPI parallelization is used
        if (ProcessManager::isMultiProc() && _parallelSims > 1)
            throw FATALERROR("You cannot run different simulations in parallel whilst parallelizing them with MPI. "
                             "Retry with -s set to 1 or consider launching different SKIRT instances.");

        // perform a simulation for each ski file
        TimeLogger logger(&_console, "a set of " + QString::number(_skifiles.size()) + " simulations"
                          + (_parallelSims > 1 ? ", " + QString::number(_parallelSims) + " in parallel" : ""));
        ParallelFactory factory;
        factory.setMaxThreadCount(_parallelSims);
        factory.parallel()->call(this, &SkirtCommandLineHandler::doSimulation, _skifiles.size());
    }

    // report memory statistics for the complete run
    _console.info(MemoryStatistics::reportAvailable(true) + " -- " + MemoryStatistics::reportPeak(true));

    // report stopwatch results, if any
    foreach (QString line, StopWatch::report()) _console.warning(line);
    return EXIT_SUCCESS;
}

////////////////////////////////////////////////////////////////////

int SkirtCommandLineHandler::doSmileSchema()
{
    SmileSchemaWriter writer;
    writer.writeSmileSchema();
    _console.info("Successfully created SMILE schema file 'skirt.smile'.");
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

    // Construct the simulation from the ski file; use shared pointer for automatic clean-up
    XmlHierarchyCreator creator;
    QSharedPointer<Simulation> simulation( creator.createHierarchy<Simulation>(filename) );

    // Check whether emulation mode if enabled
    bool emulation = _args.isPresent("-e");
    if (emulation)
    {
        // Change the number of photon packages to 1 (we don't care about actually performing it)
        simulation->find<MonteCarloSimulation>(false)->setPackages(1);

        // Disable dust self-absorption (does not lead to additional memory usage, would not converge anyway)
        try {simulation->find<PanDustSystem>(false)->setSelfAbsorption(false);}
        catch (FatalError&) {}

        // Limit the number of self-absorption cycles to avoid the convergence loop (does not work?)
        //try {simulation->find<PanDustSystem>(false)->setCycles(1);}
        //catch (FatalError&) {}
    }

    // Check whether memory (de)allocation logging is enabled
    bool memoryalloc = _args.isPresent("-l");
    #ifndef BUILDING_MEMORY
    if (memoryalloc) throw FATALERROR("Enable BUILDING_MEMORY in the build options to use the -l option");
    #endif

    // Set up any simulation attributes that are not loaded from the ski file:
    //  - the paths for input and output files
    QFileInfo skiinfo(filename);
    simulation->filePaths()->setOutputPrefix(skiinfo.completeBaseName());
    QString base = _args.isPresent("-k") ? skiinfo.absolutePath() : QDir::currentPath();
    simulation->filePaths()->setInputPath((_args.value("-i").startsWith('/') ? "" : base + "/") + _args.value("-i"));
    simulation->filePaths()->setOutputPath((_args.value("-o").startsWith('/') ? "" : base + "/") + _args.value("-o"));

    //  - the number of parallel threads
    if (_args.intValue("-t") > 0) simulation->parallelFactory()->setMaxThreadCount(_args.intValue("-t"));
    if (memoryalloc)
    {
        if (_args.intValue("-t") > 0)
            simulation->log()->warning("You cannot use multiple threads when logging memory (de)allocation. Setting "
                                       "the number of threads to 1.");
        simulation->parallelFactory()->setMaxThreadCount(1); // memory (de)allocation logging requires singlethreading
    }

    //  - the multiprocessing environment
    PeerToPeerCommunicator* comm = simulation->communicator();
    comm->setup();

    //  - the activation of data parallelization
    if (_args.isPresent("-d") && comm->isMultiProc())
    {
        PanDustSystem* pds = nullptr;
        try {pds = simulation->find<PanDustSystem>(false);}
        catch (FatalError&) {}

        if (pds && pds->dustemission())
        {
            try {pds->find<AllCellsDustLib>();}
            catch(FatalError&)
            {
                throw FATALERROR("When using -d, the dust emission can only be calculated using an AllCellsDustLib");
            }
        }
        // no PanDustSystem -> true,
        // pds but no dust emission -> true,
        // pds with dust emission -> error when no AllCellsDustLib
        comm->setDataParallel(true);
    }

    //  - the console and the file log (and memory (de)allocation logging)
    FileLog* log = new FileLog();
    simulation->log()->setLinkedLog(log);
    simulation->log()->setVerbose(_args.isPresent("-v"));
    simulation->log()->setMemoryLogging(_args.isPresent("-m"));
    if (emulation) simulation->log()->setLowestLevel(Log::Error); // in emulation mode, only log errors to the console
    if (_parallelSims > 1 || _args.isPresent("-b")) simulation->log()->setLowestLevel(Log::Success);
    #ifdef BUILDING_MEMORY
    if (memoryalloc)
    {
        // enable memory logging for each log message when memory (de)allocation logging is enabled
        simulation->log()->setMemoryLogging(true);
        // set the lower limit for memory (de)allocation logging
        log->setLimit(_args.doubleValue("-l"));
        Array::setLogger(log);
    }
    #endif

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
        if (emulation) _console.info("Emulating the simulation steps and monitoring memory usage...");
        simulation->setupAndRun();

        // if this is the only or first simulation in the run, report memory statistics in the simulation's log file
        if (_skifiles.size() == 1 || (_parallelSims==1 && index==0))
            log->info(MemoryStatistics::reportAvailable(true) + " -- " + MemoryStatistics::reportPeak(true));

        #ifdef BUILDING_MEMORY
        // disable memory (de)allocation logging after the simulation finished
        if (memoryalloc) Array::setLogger(nullptr);
        #endif
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
    _console.warning("  skirt [-t <threads>] [-s <simulations>] [-d]");
    _console.warning("        [-b] [-v] [-m] [-l <limit>] [-e]");
    _console.warning("        [-k] [-i <dirpath>] [-o <dirpath>]");
    _console.warning("        [-r] {<filepath>}*");
    _console.warning("");
    _console.warning("  -t <threads> : the number of parallel threads for each simulation");
    _console.warning("  -s <simulations> : the number of parallel simulations per process");
    _console.warning("  -d : enable data parallelization mode for multiple processes");
    _console.warning("  -b : force brief console logging");
    _console.warning("  -v : force verbose logging for multiple processes");
    _console.warning("  -m : state the amount of used memory at the start of each log message");
    _console.warning("  -l <limit> : enable memory (de)allocation logging (lower limit in GB)");
    _console.warning("  -e : run the simulation in 'emulation' mode to get an estimate of the memory consumption");
    _console.warning("  -k : make the input/output paths relative to the ski file being processed");
    _console.warning("  -i <dirpath> : the relative or absolute path for simulation input files");
    _console.warning("  -o <dirpath> : the relative or absolute path for simulation output files");
    _console.warning("  -r : cause recursive directory descent for all specified ski file paths");
    _console.warning("  <filepath> : the relative or absolute file path for a ski file");
    _console.warning("               (the filename may contain ? and * wildcards)");
    _console.warning("");
}

//////////////////////////////////////////////////////////////////////
