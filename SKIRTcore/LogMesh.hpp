/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef LOGMESH_HPP
#define LOGMESH_HPP

#include "AnchoredMesh.hpp"

//////////////////////////////////////////////////////////////////////

/** The LogMesh class represents origin-anchored, logarithmic meshes. Specifically, the first bin
    covers the interval \f$[0,t_\text{c}]\f$ and the widths of the remaining bins are distributed
    logarithmically over the interval \f$[t_\text{c},1]\f$, where \f$t_\text{c}\f$ is a
    user-configurable parameter. If the mesh has only one bin, the value of \f$t_\text{c}\f$ is
    ignored and the single bin spans the complete interval \f$[0,1]\f$. */
class LogMesh : public AnchoredMesh
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a logarithmic mesh")
    Q_CLASSINFO("AllowedIf", "!TwoPhaseDustGrid")

    Q_CLASSINFO("Property", "centralBinFraction")
    Q_CLASSINFO("Title", "the central bin width fraction")
    Q_CLASSINFO("MinValue", "0")
    Q_CLASSINFO("MaxValue", "1")
    Q_CLASSINFO("Default", "1e-3")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE LogMesh();

protected:
    /** This function verifies the property values. */
    void setupSelfBefore();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the central bin width fraction \f$t_\text{c}\f$. */
    Q_INVOKABLE void setCentralBinFraction(double value);

    /** Returns the central bin width fraction \f$t_\text{c}\f$. */
    Q_INVOKABLE double centralBinFraction() const;

    //======================== Other Functions =======================

public:
    /** This function returns an array containing the mesh points. */
    Array mesh() const;

    //======================== Data Members ========================

private:
    double _tc;
};

//////////////////////////////////////////////////////////////////////

#endif // LOGMESH_HPP
