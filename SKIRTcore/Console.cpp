/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include <mutex>
#include <QProcessEnvironment>
#include <QTextStream>
#include "Console.hpp"

////////////////////////////////////////////////////////////////////

// the underlying device is shared by all instances of this class
// so we can use static data made private by an unnamed namespace
namespace
{
    std::mutex _mutex;         // mutex to guard the input/output operations
    bool _initialized = false; // flag becomes true if static data have been initialized

    QTextStream _out(stdout);  // Unicode aware output
    QTextStream _in(stdin);    // Unicode aware input
    bool _colored = false;     // set to true if the console supports ANSI escape sequences for coloring

    // ANSI escape sequences for coloring, indexed by level (Info, Warning, Success, Error, Prompt=Error+1)
    // NOTE: this depends on the order in the Level enum --> sligthly dirty
    const char* _colorBegin[] = { "", "\033[35m", "\033[32m", "\033[31m", "\033[34m" };
    const char* _colorEnd[]   = { "", "\033[0m",  "\033[0m",  "\033[0m",  "\033[0m"  };
}

////////////////////////////////////////////////////////////////////

Console::Console()
{
    std::unique_lock<std::mutex> lock(_mutex);

    if (!_initialized)
    {
        // Set console encoding on systems where the default QTextCodec::codecForLocale() is incorrect,
        // such as when running on MacOS X in the X11 terminal launched by Qt creator
        // !! this assumes that the environment variable "CONSOLE_ENCODING" has been set
        // !! to the appropriate encoding name on those systems where it is needed
        QString codecname = QProcessEnvironment::systemEnvironment().value("CONSOLE_ENCODING");
        if (!codecname.isEmpty())
        {
            _out.setCodec(codecname.toLatin1().constData());
            _in.setCodec(codecname.toLatin1().constData());
        }

        // we assume that coloring is supported if the TERM environment variable is defined
        _colored = !QProcessEnvironment::systemEnvironment().value("TERM").isEmpty();
    }
}

////////////////////////////////////////////////////////////////////

void Console::output(QString message, Log::Level level)
{
    std::unique_lock<std::mutex> lock(_mutex);
    _out << (_colored ? _colorBegin[level] : "") << message << (_colored ? _colorEnd[level] : "") << endl;
}

////////////////////////////////////////////////////////////////////

QString Console::promptForInput(QString message)
{
    std::unique_lock<std::mutex> lock(_mutex);
    _out << (_colored ? _colorBegin[Error+1] : "")
         << timestamp() << " ? " << message
         << (_colored ? _colorEnd[Error+1] : "") << ": " << flush;
    return _in.readLine().simplified();
}

////////////////////////////////////////////////////////////////////
