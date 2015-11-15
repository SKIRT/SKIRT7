/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef SKIRTMEMORYCOMMANDLINEHANDLER_HPP
#define SKIRTMEMORYCOMMANDLINEHANDLER_HPP

#include <QStringList>
#include "CommandLineArguments.hpp"
#include "Console.hpp"
class QDir;

////////////////////////////////////////////////////////////////////

/** This class process the command line arguments for the SkirtMemory console application. */
class SkirtMemoryCommandLineHandler
{
public:
    /** This constructor parses the specified command line arguments and internally stores the
        result. */
    SkirtMemoryCommandLineHandler(QStringList cmdlineargs);

    /** This function processes the command line arguments and invokes the appropriate high-level
        functions to perform the actions requested by the user. The function returns an appropriate
        application exit value. */
    int perform();

private:
    /** This function scans the filepaths specified on the command line for ski files and performs
        the corresponding simulations according to the specified command line options. The function
        returns an appropriate application exit value. */
    int doBatch();

    /** This function returns a list of ski filenames corresponding to the specified filepath,
        after processing any wildcards and performing recursive descent if so requested by the -r
        option. If the returned list is empty, the function logs an appropriate error message and
        sets the internal error flag to true. */
    QStringList skifilesFor(QString filepath);

    /** This function returns a list of ski filenames corresponding to the specified name pattern
        inside the specified directory. If so requested by the -r option, this function implements
        recursive descent by calling itself recursively for each subdirectory. */
    QStringList skifilesFor(const QDir& dir, QString name);

    /** This function actually performs the simulation constructed from the ski file. */
    void doSimulation();

    /** This function prints a brief help message to the console. */
    void printHelp();

private:
    // data members
    CommandLineArguments _args;
    Console _console;
    QString _skifilename;
    bool _hasError;
    QString _hostname;
    QString _username;
};

////////////////////////////////////////////////////////////////////

#endif // SKIRTMEMORYCOMMANDLINEHANDLER_HPP
