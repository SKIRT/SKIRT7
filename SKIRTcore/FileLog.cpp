/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include "FatalError.hpp"
#include "FileLog.hpp"
#include "FilePaths.hpp"

////////////////////////////////////////////////////////////////////

FileLog::FileLog()
{
}

////////////////////////////////////////////////////////////////////

FileLog::~FileLog()
{
    if (_file.isOpen())
    {
        _out.flush();
        _file.close();
    }
}

////////////////////////////////////////////////////////////////////

void FileLog::setupSelfBefore()
{
    QString filepath = find<FilePaths>()->output("log.txt");
    _file.setFileName(filepath);
    if (!_file.open(QIODevice::WriteOnly | QIODevice::Text))
        throw FATALERROR("Could not open the log file " + filepath);

    _out.setDevice(&_file);
    _out.setCodec("UTF-8");
}

////////////////////////////////////////////////////////////////////

void FileLog::output(QString message, Log::Level /*level*/)
{
    QMutexLocker lock(&_mutex);
    _out << message << endl;
}

////////////////////////////////////////////////////////////////////
