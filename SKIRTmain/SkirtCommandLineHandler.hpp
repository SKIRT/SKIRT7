/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef SKIRTCOMMANDLINEHANDLER_HPP
#define SKIRTCOMMANDLINEHANDLER_HPP

#include <QStringList>
#include "CommandLineArguments.hpp"
#include "Console.hpp"
class QDir;

////////////////////////////////////////////////////////////////////

/**
This class processes the command line arguments for SKIRT and invokes the appropriate
high-level functions to perform the actions requested by the user. When invoked with invalid
command line arguments, it prints a brief help message. When invoked without any arguments, it
enters interactive mode, constructing a simulation from the user's responses and saving the
result in a ski file, without actually performing the simulation. Otherwise, it runs the
simulations in the ski files specified on the command line according to the following syntax:

\verbatim
    skirt [-b] [-s <simulations>] [-t <threads>]
          [-k] [-i <dirpath>] [-o <dirpath>]
          [-r] {<filepath>}*
\endverbatim

The -b option forces brief console logging (only success and error messages are shown). The
complete log output for a simulation run is always written to a file in the output directory.
If there are multiple parallel simulations (see the -s option), the console only shows success
and error messages. If there is only one simulation at a time, the console shows all messages
unless -b is present. The -s option specifies the number of simulations to be executed in
parallel; the default value is one. The -t option specifies the number of parallel threads for
each simulation; the default value is the number of logical cores on the computer running
SKIRT. The -k option causes the simulation input/output paths to be relative to the ski file
being processed, rather than to the current directory. The -i option specifies the absolute or
relative path for simulation input files. The -o option specifies the absolute or relative path
for simulation output files. The -r option causes recursive directory descent for all specified
\<filepath\> arguments, in other words all directories inside the specified base paths are
searched for the specified filename (or filename pattern).

In the simplest case, a \<filepath\> argument specifies the relative or absolute file path for a
single ski file, with or without the .ski extension. However the filename (NOT the base path)
may also contain ? and * wildcards forming a pattern to match multiple files. If the -r option
is present, all directories recursively nested within the base path are searched as well, using
the same filename pattern. If the filename contains wildcards or the -r option is present (in
other words, if the filepath may match multiple files) the .ski extension is not automatically
added. Furthermore, filepaths containing wildcards should be enclosed in quotes on the command
line to avoid expansion of the wildcard pattern by the shell.

For example, to process all "test" ski files inside the "geometry" directory hierarchy, one
could specify:

\verbatim
    skirt -s 4 -t 1 -r "/root-test-file-path/geometry/test*.ski"
\endverbatim
*/
class SkirtCommandLineHandler
{
public:
    /** This constructor parses the specified command line arguments and internally stores the
        result. */
    SkirtCommandLineHandler(QStringList cmdlineargs);

    /** This function processes the command line arguments and invokes the appropriate high-level
        functions to perform the actions requested by the user. The function returns an appropriate
        application exit value. */
    int perform();

private:
    /** This function conducts an interactive session to construct a simulation and save the result
        in a ski file. The function returns an appropriate application exit value. */
    int doInteractive();

    /** This function scans the filepaths specified on the command line for ski files and performs
        the corresponding simulations according to the specified command line options. The function
        returns an appropriate application exit value. */
    int doBatch();

    /** This function exports a smile schema. This is an undocumented option. */
    int doSmileSchema();

    /** This function returns a list of ski filenames corresponding to the specified filepath,
        after processing any wildcards and performing recursive descent if so requested by the -r
        option. If the returned list is empty, the function logs an appropriate error message and
        sets the internal error flag to true. */
    QStringList skifilesFor(QString filepath);

    /** This function returns a list of ski filenames corresponding to the specified name pattern
        inside the specified directory. If so requested by the -r option, this function implements
        recursive descent by calling itself recursively for each subdirectory. */
    QStringList skifilesFor(const QDir& dir, QString name);

    /** This function actually performs a single simulation constructed from the ski file at the
        specified index. */
    void doSimulation(size_t index);

    /** This function prints a brief help message to the console. */
    void printHelp();

private:
    // data members
    CommandLineArguments _args;
    Console _console;
    QStringList _skifiles;
    bool _hasError;
    int _parallelSims;
};

////////////////////////////////////////////////////////////////////

#endif // SKIRTCOMMANDLINEHANDLER_HPP
