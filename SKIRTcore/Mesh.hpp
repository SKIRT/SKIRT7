/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef MESH_HPP
#define MESH_HPP

#include "Array.hpp"
#include "SimulationItem.hpp"

//////////////////////////////////////////////////////////////////////

/** The Mesh class is an abstract base class that characterizes different types of one-dimensional
    meshes over the unit interval \f$[0,1]\f$. A mesh is essentially a partition of this interval
    into a number of \f$N\f$ finite bins. Internally, a mesh consists of an ordered array of
    \f$N+1\f$ mesh points \f$\{t_i\}\f$, with \f$t_0=0\f$ and \f$t_N=1\f$. The different subclasses
    of the Mesh class indicate different mesh point distributions, such as linear distributions,
    etc. */
class Mesh : public SimulationItem
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a mesh")

    Q_CLASSINFO("Property", "numBins")
    Q_CLASSINFO("Title", "the number of bins in the mesh")
    Q_CLASSINFO("MinValue", "1")
    Q_CLASSINFO("MaxValue", "100000")
    Q_CLASSINFO("Default", "100")

    //============= Construction - Setup - Destruction =============

protected:
    /** The default constructor; it is protected since this is an abstract class. */
    Q_INVOKABLE Mesh();

    /** This function just checks whether the number of bins is positive. */
    void setupSelfBefore();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the number of bins in the mesh. */
    Q_INVOKABLE void setNumBins(int value);

    /** Returns the number of bins in the mesh. */
    Q_INVOKABLE int numBins() const;

    //======================== Other Functions =======================

public:

    /** Returns the mesh as an Array. */
    Array mesh() const;

    /** Returns the \f$i\f$'th mesh point. If \f$i<0\f$ or \f$i\geq N\f$ a fatal error is returned.
        */
    double meshpoint(int i) const;

    //======================== Data Members ========================

protected:
    int _N;
    Array _tv;

};

//////////////////////////////////////////////////////////////////////

#endif // MESH_HPP
