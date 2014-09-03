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

    /** This function returns the complete path for a resource with the specified name (or relative
        path) residing in the "dat" directory. The function looks for the "dat" directory next to the
        SKIRT executable (a typical deployment situation), in the executable's parent folder and in
        the svn subfolder of the executable's parent folder (a typical development situation). */
    static QString resource(QString name);

    //======================== Data Members ========================

private:
    QString _inputPath;
    QString _outputPath;
    QString _outputPrefix;
};

////////////////////////////////////////////////////////////////////

#endif // FILEPATHS_HPP
