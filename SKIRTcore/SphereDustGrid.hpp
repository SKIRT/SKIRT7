/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef SPHEREDUSTGRID_HPP
#define SPHEREDUSTGRID_HPP

#include "Array.hpp"
#include "DustGrid.hpp"

////////////////////////////////////////////////////////////////////

/** The SphereDustGrid class is an abstract subclass of the general DustGrid class, and represents
    any dust grid defined within a spherical configuration space, centered on the origin of the
    system. */
class SphereDustGrid : public DustGrid
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a dust grid bounded by a sphere")

    Q_CLASSINFO("Property", "maxR")
    Q_CLASSINFO("Title", "the outer radius of the grid")
    Q_CLASSINFO("Quantity", "length")
    Q_CLASSINFO("MinValue", "0")

    //============= Construction - Setup - Destruction =============

protected:
    /** The default constructor; it is protected since this is an abstract class. */
    SphereDustGrid();

    /** This function verifies that the positivity of the outer radius, and sets the correct
        dimensions of the bounding box. */
    void setupSelfBefore();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the outer radius \f$r_{\text{max}}\f$ of the dust grid. */
    Q_INVOKABLE void setMaxR(double value);

    /** Returns the outer radius \f$r_{\text{max}}\f$ of the dust grid. */
    Q_INVOKABLE double maxR() const;

    //======================== Other Functions =======================

public:
    /** This function returns the bounding box that encloses the dust grid. */
    Box boundingbox() const;

    //======================== Data Members ========================

private:
    double _rmax;
};

////////////////////////////////////////////////////////////////////

#endif // SPHEREDUSTGRID_HPP
