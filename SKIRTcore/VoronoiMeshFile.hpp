/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef VORONOIMESHFILE_HPP
#define VORONOIMESHFILE_HPP

#include "SimulationItem.hpp"
#include "Vec.hpp"

////////////////////////////////////////////////////////////////////

/** VoronoiMeshFile is an abstract class used to read the relevant information on a cartesian
    three-dimensional Voronoi mesh from a data file. The VoronoiMeshFile subclasses implement
    specific file formats, including a generic ASCII format.

    For the purposes of this class, a Voronoi mesh represents any number of scalar fields over a
    given three-dimensional spatial domain. The Voronoi mesh partitions the domain in cells
    according to a Voronoi tesselation of the domain. Consider a given set of points in the domain,
    called particles. For each particle there will be a corresponding region consisting of all
    points in the domain closer to that particle than to any other. These regions are called
    Voronoi cells, and together they form the Voronoi tesselation of the domain. Each Voronoi cell
    holds a single value per field (i.e. the fields are considered to be constant over each cell).

    A Voronoi tesselation is fully defined by the locations of the generating particles. Thus
    an instance of this class (or rather of one of its subclasses) supplies the mesh data to the
    caller as a sequence of \em particle \em records, in arbitary order. Each record provides the
    coordinates of a particle plus the values of the fields in the cell surrounding this particle.
    All records in the file must contain the same number of field values \f$N_{fields}\f$.

    Note that the data provided by this class does not specify the domain geometry or size, the
    meaning of the represented fields, nor the units in which the field values are expressed.
    However the particle coordinates returned by this class are always given in SI units, as is
    usual for SKIRT internals.
*/
class VoronoiMeshFile : public SimulationItem
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a Voronoi mesh data file")

    Q_CLASSINFO("Property", "filename")
    Q_CLASSINFO("Title", "the name of the Voronoi mesh data file")

    //================= Construction - Destruction =================

protected:
    /** The default constructor. It is declared protected since this is an abstract class. */
    VoronoiMeshFile();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the name of the file containing the Voronoi mesh data.  */
    Q_INVOKABLE void setFilename(QString value);

    /** Returns the name of the file containing the Voronoi mesh data. */
    Q_INVOKABLE QString filename() const;

    //======================== Other Functions =======================

public:
    /** This function opens the Voronoi mesh data file, or throws a fatal error if the file can't
        be opened. It does not yet read any records. */
    virtual void open() = 0;

    /** This function closes the Voronoi mesh data file. */
    virtual void close() = 0;

    /** This function reads the next record from the file, and holds its information ready for
        inspection through the other functions of this class. The function returns true if a record
        was successfully read, or false if the end of the file was reached or another error
        occurred. */
    virtual bool read() = 0;

    /** This function returns the coordinates of the particle (in SI units) for the current record.
        If there is no current record, or if the current record does not properly provide three
        coordinates, a fatal error is thrown. */
    virtual Vec particle() const = 0;

    /** This function returns the value \f$F_g\f$ of the field (in data file units) with given
        zero-based index \f$0\le g \le N_{fields}-1\f$ for the current record. If there is no
        current record, or if the index is out of range, a fatal error is thrown. */
    virtual double value(int g) const = 0;

    //========================= Data members =======================

protected:
    // discoverable attributes
    QString _filename;
};

////////////////////////////////////////////////////////////////////

#endif // VORONOIMESHFILE_HPP
