/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "FatalError.hpp"
#include "FilePaths.hpp"
#include "Log.hpp"
#include "TextOutFile.hpp"
#include "ProcessManager.hpp"

using namespace std;

////////////////////////////////////////////////////////////////////

TextOutFile::TextOutFile(const SimulationItem* item, QString filename, QString description, bool overwrite)
    : _ncolumns(0)
{
    _log = item->find<Log>();
    _filepath = item->find<FilePaths>()->output(filename + ".dat");

    // Only open the output file if this is the root process
    if (ProcessManager::isRoot())
    {
        _log->info("Writing " + description + " to " + _filepath + "...");
        _out.open(_filepath.toLocal8Bit().constData(), overwrite ? ios_base::out : ios_base::app);
    }
}

////////////////////////////////////////////////////////////////////

TextOutFile::~TextOutFile()
{
    // Close the output file, if it was opened by this process
    if (_out.is_open())
    {
        _out.close();
        _log->info("File " + _filepath + " created.");
    }
}

////////////////////////////////////////////////////////////////////

void TextOutFile::addColumn(QString description, char format, int precision)
{
    _formats.append(format);
    _precisions.append(precision);

    writeLine("# column " + QString::number(++_ncolumns) + ": " + description);
}

////////////////////////////////////////////////////////////////////

void TextOutFile::writeLine(QString line)
{
    if (_out.is_open())
    {
        _out << line.toStdString() << endl;
    }
}

////////////////////////////////////////////////////////////////////

void TextOutFile::writeRow(QList<double> values)
{
    if (values.size() != _ncolumns) throw FATALERROR("Number of values in row does not match the number of columns");

    QString line;
    for (int i=0; i<_ncolumns; i++)
    {
        if (_formats[i] == 'd')
        {
            line += (i ? " " : "") + QString::number(values[i], 'f', 0);
        }
        else
        {
            line += (i ? " " : "") + QString::number(values[i], _formats[i], _precisions[i]);
        }
    }
    writeLine(line);
}

////////////////////////////////////////////////////////////////////
