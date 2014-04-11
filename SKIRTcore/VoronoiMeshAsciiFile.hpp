/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef VORONOIMESHASCIIFILE_HPP
#define VORONOIMESHASCIIFILE_HPP

#include <QFile>
#include "VoronoiMeshFile.hpp"

////////////////////////////////////////////////////////////////////

/** The VoronoiMeshAsciiFile class can read the relevant information on a cartesian
    three-dimensional Voronoi mesh from a file in a simple ASCII text column format. Comment lines
    are ignored, i.e. lines with a crosshatch (#) as the first non-whitespace character, lines
    containing only whitespace, and empty lines. Any other line represents a particle record and
    must contain three or more whitespace-separated floating point numbers. The first three numbers
    provide the x,y,z coordinates of the particle for this record. Subsequent numbers provide the
    \f$N_{fields}\f$ values of the fields for this record, i.e. the fourth number provides the
    value for field \f$F_0\f$, the fifth for \f$F_1\f$, and so on. All record lines in the file
    must contain the same number of field values. */
class VoronoiMeshAsciiFile : public VoronoiMeshFile
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a Voronoi mesh data file in ASCII format")

    Q_CLASSINFO("Property", "coordinateUnits")
    Q_CLASSINFO("Title", "the units in which the file specifies particle coordinates")
    Q_CLASSINFO("Quantity", "length")
    Q_CLASSINFO("MinValue", "0")
    Q_CLASSINFO("Default", "1 pc")

    //================= Construction - Destruction =================

public:
    /** The default constructor. */
    Q_INVOKABLE VoronoiMeshAsciiFile();

protected:
    /** This function verifies the property values. */
    virtual void setupSelfBefore();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the units in which the file specifies particle coordinates. */
    Q_INVOKABLE void setCoordinateUnits(double value);

    /** Returns the units in which the file specifies particle coordinates. */
    Q_INVOKABLE double coordinateUnits() const;

    //======================== Other Functions =======================

public:
    /** This function opens the Voronoi mesh data file, or throws a fatal error if the file can't
        be opened. It does not yet read any records. */
    void open();

    /** This function closes the Voronoi mesh data file. */
    void close();

    /** This function reads the next record from the file, and holds its information ready for
        inspection through the other functions of this class. The function returns true if a record
        was successfully read, or false if the end of the file was reached or another error
        occurred. */
    bool read();

    /** This function returns the coordinates of the particle (in SI units) for the current record.
        If there is no current record, or if the current record does not properly provide three
        coordinates, a fatal error is thrown. */
    Vec particle() const;

    /** This function returns the value \f$F_g\f$ of the field (in data file units) with given
        zero-based index \f$0\le g \le N_{fields}-1\f$ for the current record. If there is no
        current record, or if the index is out of range, a fatal error is thrown. */
    double value(int g) const;

    //========================= Data members =======================

private:
    double _coordinateUnits;     // the units in which the file specifies particle coordinates
    QFile _infile;               // the input file
    QList<QByteArray> _columns;  // the columns of the current record, or empty if there is no current record
};

////////////////////////////////////////////////////////////////////

#endif // VORONOIMESHASCIIFILE_HPP
