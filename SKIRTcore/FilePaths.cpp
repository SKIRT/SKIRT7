/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include <QCoreApplication>
#include <QFileInfo>
#include <QMutex>
#include "FatalError.hpp"
#include "FilePaths.hpp"

////////////////////////////////////////////////////////////////////

namespace
{
    // mutex to guard initialization of the static application and resource paths
    QMutex _mutex;

    // flag becomes true if the static paths have been initialized
    bool _initialized = false;

    // the static application and resource paths
    QString _applicationPath;
    QString _resourcePath;

    // relative paths to check for presence of dat folder
    const char* _datpaths[] = { "dat", "../dat", "../../git/dat", "../svn/dat" };
    const int _Ndatpaths = sizeof(_datpaths) / sizeof(const char*);

    // sets the static application and resource paths, or throws an error if there is a problem
    void setStaticPaths()
    {
        // initialization must be locked to protect against race conditions when used in multiple threads
        QMutexLocker lock(&_mutex);

        if (!_initialized)
        {
            // get the location of the executable
            QString appPath = QCoreApplication::applicationDirPath();

            // verify that its a directory and store the canonical path
            QFileInfo test(appPath);
            if (test.isDir()) _applicationPath = test.canonicalFilePath() + "/";
            else throw FATALERROR("Could not locate SKIRT directory '" + appPath + "'");

            // iterate over the relative paths
            for (int i=0; i<_Ndatpaths; i++)
            {
                QFileInfo test(appPath + "/" + _datpaths[i]);
                if (test.isDir())
                {
                    _resourcePath = test.canonicalFilePath() + "/";
                    _initialized = true;
                    return;
                }
            }

            // if we reach here, the "dat" folder wasn't found
            throw FATALERROR("Could not locate 'dat' directory relative to '" + appPath + "'");
        }
    }
}

////////////////////////////////////////////////////////////////////

FilePaths::FilePaths()
{
    // verify existence of the dat directory sooner rather than later
    if (!_initialized) setStaticPaths();
}

////////////////////////////////////////////////////////////////////

void FilePaths::setInputPath(QString value)
{
    if (value.isEmpty()) _inputPath = "";
    else
    {
        QFileInfo test(value);
        if (!test.isDir()) throw FATALERROR("Input path does not exist or is not a directory: " + value);
        _inputPath = test.canonicalFilePath() + "/";
    }
}

////////////////////////////////////////////////////////////////////

QString FilePaths::inputPath() const
{
    return _inputPath;
}

////////////////////////////////////////////////////////////////////

void FilePaths::setOutputPath(QString value)
{
    if (value.isEmpty()) _outputPath = "";
    else
    {
        QFileInfo test(value);
        if (!test.isDir()) throw FATALERROR("Output path does not exist or is not a directory: " + value);
        _outputPath = test.canonicalFilePath() + "/";
    }
}

////////////////////////////////////////////////////////////////////

QString FilePaths::outputPath() const
{
    return _outputPath;
}

////////////////////////////////////////////////////////////////////

void FilePaths::setOutputPrefix(QString value)
{
    _outputPrefix = value;
}

////////////////////////////////////////////////////////////////////

QString FilePaths::outputPrefix() const
{
    return _outputPrefix;
}

////////////////////////////////////////////////////////////////////

QString FilePaths::input(QString name) const
{
    return _inputPath + name;
}

////////////////////////////////////////////////////////////////////

QString FilePaths::output(QString name) const
{
    return _outputPath + _outputPrefix + "_" + name;
}

////////////////////////////////////////////////////////////////////

QString FilePaths::application(QString name)
{
    // set the static paths if needed
    if (!_initialized) setStaticPaths();
    return _applicationPath + name;
}

////////////////////////////////////////////////////////////////////

QString FilePaths::resource(QString name)
{
    // set the static paths if needed
    if (!_initialized) setStaticPaths();
    return _resourcePath + name;
}

////////////////////////////////////////////////////////////////////
