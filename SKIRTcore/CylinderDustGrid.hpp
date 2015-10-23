/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef CYLINDERDUSTGRID_HPP
#define CYLINDERDUSTGRID_HPP

#include "Array.hpp"
#include "DustGrid.hpp"

////////////////////////////////////////////////////////////////////

/** The CylinderDustGrid class is an abstract subclass of the general DustGrid class, and represents
    any dust grid defined within a cylindrical configuration space, with the symmetry axis the Z-axis of the system. */
class CylinderDustGrid : public DustGrid
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a dust grid bounded by a cylinder")

    Q_CLASSINFO("Property", "maxR")
    Q_CLASSINFO("Title", "the cylindrical radius of the grid")
    Q_CLASSINFO("Quantity", "length")
    Q_CLASSINFO("MinValue", "0")

    Q_CLASSINFO("Property", "minZ")
    Q_CLASSINFO("Title", "the start point of the cylinder in the Z direction")
    Q_CLASSINFO("Quantity", "length")

    Q_CLASSINFO("Property", "maxZ")
    Q_CLASSINFO("Title", "the end point of the cylinder in the Z direction")
    Q_CLASSINFO("Quantity", "length")

    //============= Construction - Setup - Destruction =============

protected:
    /** The default constructor; it is protected since this is an abstract class. */
    CylinderDustGrid();

    /** This function verifies the characteristics of the grid. */
    void setupSelfBefore();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the cylindrical radius \f$R_{\text{max}}\f$ of the dust grid. */
    Q_INVOKABLE void setMaxR(double value);

    /** Returns the cylindrical radius \f$R_{\text{max}}\f$ of the dust grid. */
    Q_INVOKABLE double maxR() const;

    /** Sets the start point \f$z_{\text{min}}\f$ of the cylinder in the Z direction. */
    Q_INVOKABLE void setMinZ(double value);

    /** Returns the start point \f$z_{\text{min}}\f$ of the cylinder in the Z direction. */
    Q_INVOKABLE double minZ() const;

    /** Sets the end point \f$z_{\text{max}}\f$ of the cylinder in the Z direction. */
    Q_INVOKABLE void setMaxZ(double value);

    /** Returns the end point \f$z_{\text{max}}\f$ of the cylinder in the Z direction. */
    Q_INVOKABLE double maxZ() const;

    //======================== Other Functions =======================

    /** This function returns the bounding box that encloses the dust grid. */
    Box boundingbox() const;

    //======================== Data Members ========================

private:
    // discoverable properties
    double _Rmax;
    double _zmin;
    double _zmax;
};

////////////////////////////////////////////////////////////////////

#endif // CYLINDERDUSTGRID_HPP
