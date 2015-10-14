/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef SYMPOWMESH_HPP
#define SYMPOWMESH_HPP

#include "MoveableMesh.hpp"

//////////////////////////////////////////////////////////////////////

/** The SymPowMesh class represents meshes on the unit interval \f$[0,1]\f$ with a symmetric
    power-law distribution of the mesh points. This distribution is such that the bin sizes form a
    geometric series, starting from the innermost bin and moving outwards symmetrically. If the
    number of bins is odd, there is one innermost bin; if it is even, there are two equal-size
    innermost bins. The actual distribution is characterized by a single parameter: the bin width
    ratio between the outermost and the innermost bins. This ratio can be larger than one (in which
    case the bin size increases when moving outwards) or smaller than one (in which case the bin
    size decrease from the centre to the edge of the interval). If the mesh has only one bin, this
    bin spans the complete interval \f$[0,1]\f$. If the mesh has two bins, each bin spans exactly
    half of the interval \f$[0,1]\f$. */
class SymPowMesh : public MoveableMesh
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a symmetric power-law mesh")
    Q_CLASSINFO("AllowedIf", "!TwoPhaseDustGrid")

    Q_CLASSINFO("Property", "ratio")
    Q_CLASSINFO("Title", "the bin width ratio between the outermost and the innermost bins")
    Q_CLASSINFO("MinValue", "0")
    Q_CLASSINFO("Default", "1")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE SymPowMesh();

protected:
    /** This function verifies the property values. */
    void setupSelfBefore();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the bin width ratio of the mesh. */
    Q_INVOKABLE void setRatio(double value);

    /** Returns the bin width ratio of the mesh. */
    Q_INVOKABLE double ratio() const;

    //======================== Other Functions =======================

public:
    /** This function returns an array containing the mesh points. */
    Array mesh() const;

    //======================== Data Members ========================

private:
    double _ratio;
};

//////////////////////////////////////////////////////////////////////

#endif // SYMPOWMESH_HPP
