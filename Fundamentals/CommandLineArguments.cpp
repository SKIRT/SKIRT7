/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "CommandLineArguments.hpp"

////////////////////////////////////////////////////////////////////

CommandLineArguments::CommandLineArguments(QStringList cmdlineargs, QString options)
{
    // parse the option list into a dictionary with a value of true if the option takes a value
    QHash<QString,bool> takesValue;
    foreach(QString option, options.simplified().split(' '))
    {
        // ignore options that don't start with a dash
        if (option.startsWith('-'))
        {
            if (option.endsWith('*')) takesValue[option.left(option.length()-1)] = true;
            else takesValue[option] = false;
        }
    }

    // remove the application name from the command line arguments
    if (!cmdlineargs.isEmpty()) cmdlineargs.removeFirst();

    // parse the command line arguments, verifying allowed options and values
    _valid = false;
    for (int index = 0; index < cmdlineargs.size(); index++)
    {
        QString arg = cmdlineargs[index];

        // process an option and, if applicable, its value
        if (arg.startsWith('-'))
        {
            if (!takesValue.contains(arg)) return;
            if (takesValue[arg])
            {
                if (index+1 >= cmdlineargs.size()) return;
                QString value = cmdlineargs[++index];
                if (value.startsWith('-')) return;
                _options[arg] = value;
            }
            else _options[arg] = "";
        }

        // process a filepath
        else
        {
            _filepaths << arg;
        }
    }
    _valid = true;
}

////////////////////////////////////////////////////////////////////

bool CommandLineArguments::isValid() const
{
    return _valid;
}

////////////////////////////////////////////////////////////////////

bool CommandLineArguments::hasOptions() const
{
    return _valid && !_options.isEmpty();
}

////////////////////////////////////////////////////////////////////

bool CommandLineArguments::isPresent(QString option) const
{
    return _valid && _options.contains(option);
}

////////////////////////////////////////////////////////////////////

QString CommandLineArguments::value(QString option) const
{
    return _valid ? _options.value(option) : "";
}

////////////////////////////////////////////////////////////////////

int CommandLineArguments::intValue(QString option) const
{
    if (_valid && !_options.value(option).isEmpty())
    {
        bool ok;
        int result = _options.value(option).toInt(&ok);
        if (ok) return result;
    }
    return -1;
}

////////////////////////////////////////////////////////////////////

double CommandLineArguments::doubleValue(QString option) const
{
    if (_valid && !_options.value(option).isEmpty())
    {
        bool ok;
        double result = _options.value(option).toDouble(&ok);
        if (ok) return result;
    }
    return -1;
}

////////////////////////////////////////////////////////////////////

bool CommandLineArguments::hasFilepaths() const
{
    return _valid && !_filepaths.isEmpty();
}

////////////////////////////////////////////////////////////////////

QStringList CommandLineArguments::filepaths() const
{
    return _valid ? _filepaths : QStringList();
}

////////////////////////////////////////////////////////////////////
