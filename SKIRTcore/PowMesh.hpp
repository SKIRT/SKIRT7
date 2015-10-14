/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef POWMESH_HPP
#define POWMESH_HPP

#include "Mesh.hpp"

//////////////////////////////////////////////////////////////////////

/** The PowMesh class represents meshes on the unit interval \f$[0,1]\f$ with a power-law
    distribution of the mesh points. This distribution is such that the bin sizes form a geometric
    series, i.e. each bin is a constant factor larger than the previous one. The actual
    distribution is characterized by a single parameter: the bin width ratio between the last and
    the first bin, \f[ {\cal{R}} = \frac{t_N-t_{N-1}}{t_1-t_0}.\f] This ratio can be larger than
    one (in which case the first bin is the smallest) or smaller than one (in which case the last
    bin is the smallest). */
class PowMesh : public Mesh
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a power-law mesh")
    Q_CLASSINFO("AllowedIf", "!TwoPhaseDustGrid")

    Q_CLASSINFO("Property", "ratio")
    Q_CLASSINFO("Title", "the bin width ratio between the last and the first bin")
    Q_CLASSINFO("MinValue", "0")
    Q_CLASSINFO("Default", "1")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE PowMesh();

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

#endif // POWMESH_HPP
