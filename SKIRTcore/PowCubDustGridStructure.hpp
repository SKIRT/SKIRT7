/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef POWCUBDUSTGRIDSTRUCTURE_HPP
#define POWCUBDUSTGRIDSTRUCTURE_HPP

#include "CubDustGridStructure.hpp"

////////////////////////////////////////////////////////////////////

/** The PowCubDustGridStructure class is a subclass of the CubDustGridStructure class, and
    represents three-dimensional, cartesian dust grid structures with a power-law distribution of
    the grid points in all directions. */
class PowCubDustGridStructure : public CubDustGridStructure
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a 3D cartesian grid structure with a power-law distribution")

    Q_CLASSINFO("Property", "extentX")
    Q_CLASSINFO("Title", "the outer radius in the x direction")
    Q_CLASSINFO("Quantity", "length")
    Q_CLASSINFO("MinValue", "0")

    Q_CLASSINFO("Property", "ratioX")
    Q_CLASSINFO("Title", "the ratio of the inner- and outermost bin widths in the x direction")
    Q_CLASSINFO("MinValue", "0")
    Q_CLASSINFO("MaxValue", "10000")
    Q_CLASSINFO("Default", "50")

    Q_CLASSINFO("Property", "pointsX")
    Q_CLASSINFO("Title", "the number of grid points in the x direction")
    Q_CLASSINFO("MinValue", "5")
    Q_CLASSINFO("MaxValue", "100000")
    Q_CLASSINFO("Default", "250")

    Q_CLASSINFO("Property", "extentY")
    Q_CLASSINFO("Title", "the outer radius in the y direction")
    Q_CLASSINFO("Quantity", "length")
    Q_CLASSINFO("MinValue", "0")

    Q_CLASSINFO("Property", "ratioY")
    Q_CLASSINFO("Title", "the ratio of the inner- and outermost bin widths in the y direction")
    Q_CLASSINFO("MinValue", "0")
    Q_CLASSINFO("MaxValue", "10000")
    Q_CLASSINFO("Default", "50")

    Q_CLASSINFO("Property", "pointsY")
    Q_CLASSINFO("Title", "the number of grid points in the y direction")
    Q_CLASSINFO("MinValue", "5")
    Q_CLASSINFO("MaxValue", "100000")
    Q_CLASSINFO("Default", "250")

    Q_CLASSINFO("Property", "extentZ")
    Q_CLASSINFO("Title", "the outer radius in the z direction")
    Q_CLASSINFO("Quantity", "length")
    Q_CLASSINFO("MinValue", "0")

    Q_CLASSINFO("Property", "ratioZ")
    Q_CLASSINFO("Title", "the ratio of the inner- and outermost bin widths in the z direction")
    Q_CLASSINFO("MinValue", "0")
    Q_CLASSINFO("MaxValue", "10000")
    Q_CLASSINFO("Default", "50")

    Q_CLASSINFO("Property", "pointsZ")
    Q_CLASSINFO("Title", "the number of grid points in the z direction")
    Q_CLASSINFO("MinValue", "5")
    Q_CLASSINFO("MaxValue", "100000")
    Q_CLASSINFO("Default", "250")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE PowCubDustGridStructure();

protected:
    /** This function verifies the validity of the input parameters, and then calculates the
        distribution of grid points in each direction. */
    void setupSelfBefore();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the outer radius in the x direction. */
    Q_INVOKABLE void setExtentX(double value);

    /** Returns the outer radius in the x direction. */
    Q_INVOKABLE double extentX() const;

    /** Sets the ratio of the inner- and outermost bin widths in the x direction. */
    Q_INVOKABLE void setRatioX(double value);

    /** Returns the ratio of the inner- and outermost bin widths in the x direction. */
    Q_INVOKABLE double ratioX() const;

    /** Sets the number of grid points in the x direction. */
    Q_INVOKABLE void setPointsX(int value);

    /** Returns the number of grid points in the x direction. */
    Q_INVOKABLE int pointsX() const;

    /** Sets the outer radius in the y direction. */
    Q_INVOKABLE void setExtentY(double value);

    /** Returns the outer radius in the y direction. */
    Q_INVOKABLE double extentY() const;

    /** Sets the ratio of the inner- and outermost bin widths in the y direction. */
    Q_INVOKABLE void setRatioY(double value);

    /** Returns the ratio of the inner- and outermost bin widths in the y direction. */
    Q_INVOKABLE double ratioY() const;

    /** Sets the number of grid points in the y direction. */
    Q_INVOKABLE void setPointsY(int value);

    /** Returns the number of grid points in the y direction. */
    Q_INVOKABLE int pointsY() const;

    /** Sets the outer radius in the z direction. */
    Q_INVOKABLE void setExtentZ(double value);

    /** Returns the outer radius in the z direction. */
    Q_INVOKABLE double extentZ() const;

    /** Sets the ratio of the inner- and outermost bin widths in the z direction. */
    Q_INVOKABLE void setRatioZ(double value);

    /** Returns the ratio of the inner- and outermost bin widths in the z direction. */
    Q_INVOKABLE double ratioZ() const;

    /** Sets the number of grid points in the z direction. */
    Q_INVOKABLE void setPointsZ(int value);

    /** Returns the number of grid points in the z direction. */
    Q_INVOKABLE int pointsZ() const;

    //======================== Data Members ========================

private:
    double _xratio;
    double _yratio;
    double _zratio;
};

////////////////////////////////////////////////////////////////////

#endif // POWCUBDUSTGRIDSTRUCTURE_HPP
