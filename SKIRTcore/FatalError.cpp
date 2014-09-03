/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include <cstdlib>
#include <execinfo.h>    // UNIX only
#include "FatalError.hpp"

////////////////////////////////////////////////////////////////////

FatalError::FatalError(QString message, const char* file, int line, const char* function)
{
    // split the message in lines if needed
    _message = message.split('\n', QString::SkipEmptyParts);

    // ensure that the first line is nonempty
    if (_message.isEmpty()) _message << "Unknown error";
    else if (_message[0].isEmpty()) _message[0] = "Unknown error";

    // add information on the source code location
    _message << QString("On line %1 in file %2").arg(line).arg(file);
    _message << QString("In function %1").arg(function);

    // add a simple stack trace (UNIX only)
    _message << "Call stack:";
    const int max_depth = 100;
    int stack_depth;
    void *stack_addrs[max_depth];
    char **stack_strings;
    stack_depth = backtrace(stack_addrs, max_depth);
    stack_strings = backtrace_symbols(stack_addrs, stack_depth);
    for (int i = 1; i < stack_depth; i++)
        _message << QString(stack_strings[i]).simplified();
    free(stack_strings); // malloc()ed by backtrace_symbols
}

QStringList FatalError::message() const
{
    return _message;
}

////////////////////////////////////////////////////////////////////
