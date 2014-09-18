/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QSharedPointer>
#include "Console.hpp"
#include "ConsoleHierarchyCreator.hpp"
#include "FatalError.hpp"
#include "FileLog.hpp"
#include "FilePaths.hpp"
#include "LatexHierarchyWriter.hpp"
#include "FitScheme.hpp"
#include "FitSkirtCommandLineHandler.hpp"
#include "StopWatch.hpp"
#include "TimeLogger.hpp"
#include "XmlHierarchyCreator.hpp"
#include "XmlHierarchyWriter.hpp"

////////////////////////////////////////////////////////////////////

namespace
{
    // the allowed options list, in the format consumed by the CommandLineArguments constructor
    static const char* allowedOptions = "-t* -s* -i* -o* -k";
}

////////////////////////////////////////////////////////////////////

FitSkirtCommandLineHandler::FitSkirtCommandLineHandler(QStringList cmdlineargs)
    : _args(cmdlineargs, allowedOptions)
{
    _console.info("Welcome to " + QCoreApplication::applicationName() + " " + QCoreApplication::applicationVersion());
}

////////////////////////////////////////////////////////////////////

int FitSkirtCommandLineHandler::perform()
{
    // catch and properly report any exceptions
    try
    {
        // if there are no arguments at all --> interactive mode
        // if there is at least one file path argument --> batch mode
        // otherwise --> error
        if (_args.isValid() && !_args.hasOptions() && !_args.hasFilepaths()) return doInteractive();
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

int FitSkirtCommandLineHandler::doInteractive()
{
    _console.info("Interactively constructing a fit scheme...");

    // ask for the name of the fski file in which to save the result
    QString filename;
    while (true)
    {
        filename = _console.promptForInput("Enter the name of the fski file to be created");

        // reject empty strings
        if (!filename.isEmpty())
        {
            // add .fski extension if needed
            if (!filename.toLower().endsWith(".fski")) filename += ".fski";

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

    // interactively construct the fit scheme; use shared pointer for automatic clean-up
    ConsoleHierarchyCreator creator;
    QSharedPointer<FitScheme> fitscheme( creator.createHierarchy<FitScheme>() );

    // create the fski file reflecting this fit scheme
    XmlHierarchyWriter writer;
    writer.writeHierarchy(fitscheme.data(), filename);
    _console.info("Successfully created fski file '" + filename + "'.");
    _console.info("To run the fit use the command: fitskirt " + filename.left(filename.length()-5));

    return EXIT_SUCCESS;
}

////////////////////////////////////////////////////////////////////

int FitSkirtCommandLineHandler::doBatch()
{
    // build a list of paths to existing ski files
    QStringList filepaths;
    foreach (QString filepath, _args.filepaths())
    {
        // if the file does not exist as specified, try adding the .fski extension
        if (!QFile::exists(filepath) && !filepath.toLower().endsWith(".fski")) filepath += ".fski";
        if (QFile::exists(filepath)) filepaths << filepath;
        else
        {
            _console.error("This ski file does not exist: " + filepath);
            return EXIT_FAILURE;
        }
    }

    // a single filepath -> just run it
    if (filepaths.size() == 1)
    {
        doFitScheme(filepaths[0]);
    }

    // multiple filepaths -> add a time logger around the complete set
    else
    {
        TimeLogger logger(&_console, "a set of " + QString::number(filepaths.size()) + " fit schemes");
        for (int index = 0; index < filepaths.size(); index++)
        {
            _console.warning("Performing fit scheme #" + QString::number(index+1) +
                             " of " + QString::number(filepaths.size()));
            doFitScheme(filepaths[index]);
        }
    }

    // report stopwatch results, if any
    foreach (QString line, StopWatch::report()) _console.warning(line);
    return EXIT_SUCCESS;
}

////////////////////////////////////////////////////////////////////

void FitSkirtCommandLineHandler::doFitScheme(QString filepath)
{
    _console.info("Constructing a fit scheme from fski file '" + filepath + "'...");

    // construct the fit scheme from the fski file; use shared pointer for automatic clean-up
    XmlHierarchyCreator creator;
    QSharedPointer<FitScheme> fitscheme( creator.createHierarchy<FitScheme>(filepath) );

    // setup any properties that are not loaded from the fski file
    // file paths
    QFileInfo skiinfo(filepath);
    fitscheme->filePaths()->setOutputPrefix(skiinfo.completeBaseName());
    QString base = _args.isPresent("-k") ? skiinfo.absolutePath() : QDir::currentPath();
    fitscheme->filePaths()->setInputPath((_args.value("-i").startsWith('/') ? "" : base + "/") + _args.value("-i"));
    fitscheme->filePaths()->setOutputPath((_args.value("-o").startsWith('/') ? "" : base + "/") + _args.value("-o"));
    // parallel simulations and threads
    if (_args.intValue("-s") > 0) fitscheme->setParallelSimulationCount(_args.intValue("-s"));
    if (_args.intValue("-t") > 0) fitscheme->setParallelThreadCount(_args.intValue("-t"));
    // console
    //FileLog* log = new FileLog();
    //fitscheme->log()->setLinkedLog(log);

    // output an fski file and a latex file reflecting this fit scheme for later reference
    {
        //XmlHierarchyWriter writer1;
        //writer1.writeHierarchy(fitscheme.data(), fitscheme->filePaths()->output("fitscheme.xml"));
        //LatexHierarchyWriter writer2;
        //writer2.writeHierarchy(fitscheme.data(), fitscheme->filePaths()->output("fitscheme.tex"));
    }

    // run the fit scheme; catch and rethrow exceptions so they are also logged to file

    Log* log = fitscheme->find<Log>();
    try
    {
        log->setup();
        log->info(QCoreApplication::applicationName() + " " + QCoreApplication::applicationVersion());
        fitscheme->setupAndRun();
    }
    catch (FatalError& error)
    {
        foreach (QString line, error.message()) log->error(line);
        throw error;
    }
}

////////////////////////////////////////////////////////////////////

void FitSkirtCommandLineHandler::printHelp()
{
    _console.warning("");
    _console.warning("To create a new fski file interactively:   fitskirt");
    _console.warning("To run a fit scheme with default options:  fitskirt <fski-filename>");
    _console.warning("");
    _console.warning(" fitskirt [-k] [-i <dirpath>] [-o <dirpath>]");
    _console.warning("          [-s <simulations>] [-t <threads>] ");
    _console.warning("          {<filepath>}*");
    _console.warning("");
    _console.warning("  -k : makes the input/output paths relative to the fski file being processed");
    _console.warning("  -i <dirpath> : the relative or absolute path for input files");
    _console.warning("  -o <dirpath> : the relative or absolute path for output files");
    _console.warning("  -s <simulations> : the number of parallel SKIRT simulations");
    _console.warning("  -t <threads> : the number of parallel threads for each SKIRT simulation");
    _console.warning("  <filepath> : the relative or absolute file path for an fski file");
    _console.warning("");
}

//////////////////////////////////////////////////////////////////////
