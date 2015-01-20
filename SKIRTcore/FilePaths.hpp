/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef FILEPATHS_HPP
#define FILEPATHS_HPP

#include "SimulationItem.hpp"

////////////////////////////////////////////////////////////////////

/** The FilePaths class manages the paths for the input and output files of a simulation, and for
    the resources included with the code. */
class FilePaths : public SimulationItem
{
    Q_OBJECT

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor */
    FilePaths();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the (absolute or relative) path for input files. An empty string (the default value)
        means the current directory. */
    void setInputPath(QString value);

    /** Returns the (absolute or relative) path for input files. */
    QString inputPath() const;

    /** Sets the (absolute or relative) path for output files. An empty string (the default value)
        means the current directory. */
    void setOutputPath(QString value);

    /** Returns the (absolute or relative) path for output files. */
    QString outputPath() const;

    /** Sets the prefix for output file names; the default is empty (i.e. no prefix). */
    void setOutputPrefix(QString value);

    /** Returns the prefix for output file names. */
    QString outputPrefix() const;

    //======================== Other Functions =======================

public:
    /** This function returns the complete path for an input file with the specified name, relative
        to the input path returned by inputPath(). */
    QString input(QString name) const;

    /** This function returns the complete path for an output file with the specified name, relative
        to the output path returned by outputPath(). The prefix returned by outputPrefix() is
        inserted in front of the filename specified here. The prefix and the filename are separated
        by an underscore. */
    QString output(QString name) const;

    /** This function returns the complete path for an executable with the specified name residing
        in the same directory as the SKIRT executable. */
    static QString application(QString name);

    /** This function returns the complete path for a built-in resource with the specified relative
        path and filename. All built-in resources are expected to reside in one of the standard
        places, e.g. the \c dat directory next to the SKIRT executable (a typical deployment
        situation) or in the \c dat directory of the SKIRT build tree (a typical development
        situation). The function does not verify that the specified resource actually exists, so it
        is permissible to pass a partial filename and append the remaining part to the returned
        result later on. */
    static QString resource(QString name);

    /** This function returns the complete path for an external resource with the specified
        filename (without file path). The function looks for the specified file in the \c extdat
        directory (see below) and in all its subdirectories, recursively. If the file can't be
        located, the function throws a fatal error.

        Overly large resource files are not provided as part of the
        SKIRT build tree to keep the source code repository down to a reasonable size. Instead,
        these resources can be downloaded seperately from the SKIRT web site using the
        "downloadextdat.sh" shell script, which places the files in the \c extdat directory next
        to the \c git directory (for development situations). Alternatively, the extdat directory
        can be placed next to the SKIRT executable (for deployment situations). */
    static QString externalResource(QString name);

    //======================== Data Members ========================

private:
    QString _inputPath;
    QString _outputPath;
    QString _outputPrefix;
};

////////////////////////////////////////////////////////////////////

#endif // FILEPATHS_HPP
