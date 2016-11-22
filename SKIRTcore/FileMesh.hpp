/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef FILEMESH_HPP
#define FILEMESH_HPP

#include "MoveableMesh.hpp"
#include "Array.hpp"

////////////////////////////////////////////////////////////////////

/** FileMesh is a subclass of the MoveableMesh class. It represents a one-dimensional mesh over the
    unit interval [0,1] with mesh points that are read from a file. The \em numBins property of the
    Mesh base class is overridden to match the number of bins defined by the file. */
class FileMesh : public MoveableMesh
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a mesh read from a file")

    Q_CLASSINFO("Property", "filename")
    Q_CLASSINFO("Title", "the name of the file with the mesh points")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE FileMesh();

protected:
    /** This function reads the mesh points from the specified file. */
    void setupSelfBefore();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the name of the file containing the mesh points, optionally including an absolute or
        relative path. The text file contains the mesh points (i.e. the border points separating
        the mesh bins) in arbitrary order. The points may be given in arbitary units. If the lowest
        point is not zero, an extra zero mesh point is inserted. The mesh is scaled so that the
        last point in the file is mapped to unity. */
    Q_INVOKABLE void setFilename(QString value);

    /** Returns the name of the file containing the mesh points. */
    Q_INVOKABLE QString filename() const;

    //======================== Other Functions =======================

public:
    /** This function returns an array containing the mesh points. */
    Array mesh() const;

    //======================== Data Members ========================

private:
    QString _filename;
    Array _mesh;
};

////////////////////////////////////////////////////////////////////

#endif // FILEMESH_HPP
