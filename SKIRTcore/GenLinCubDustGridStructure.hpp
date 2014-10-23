/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef GENLINCUBDUSTGRIDSTRUCTURE_HPP
#define GENLINCUBDUSTGRIDSTRUCTURE_HPP

#include "CubDustGridStructure.hpp"

////////////////////////////////////////////////////////////////////

/** The GenLinCubDustGridStructure class is a subclass of the CubDustGridStructure class, and
    represents three-dimensional, cartesian dust grid structures with a linear distribution of the
    grid points in all directions. In each direction, the minimum radius, maximum radius and the
    number of grid points can be chosen. */
class GenLinCubDustGridStructure : public CubDustGridStructure
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a general 3D cartesian grid structure with a linear distribution")

    Q_CLASSINFO("Property", "minX")
    Q_CLASSINFO("Title", "the minimum radius in the x direction")
    Q_CLASSINFO("Quantity", "length")

    Q_CLASSINFO("Property", "maxX")
    Q_CLASSINFO("Title", "the maximum radius in the x direction")
    Q_CLASSINFO("Quantity", "length")

    Q_CLASSINFO("Property", "pointsX")
    Q_CLASSINFO("Title", "the number of grid points in the x direction")
    Q_CLASSINFO("MinValue", "5")
    Q_CLASSINFO("MaxValue", "100000")
    Q_CLASSINFO("Default", "250")

    Q_CLASSINFO("Property", "minY")
    Q_CLASSINFO("Title", "the minimum radius in the y direction")
    Q_CLASSINFO("Quantity", "length")

    Q_CLASSINFO("Property", "maxY")
    Q_CLASSINFO("Title", "the maximum radius in the y direction")
    Q_CLASSINFO("Quantity", "length")

    Q_CLASSINFO("Property", "pointsY")
    Q_CLASSINFO("Title", "the number of grid points in the y direction")
    Q_CLASSINFO("MinValue", "5")
    Q_CLASSINFO("MaxValue", "100000")
    Q_CLASSINFO("Default", "250")

    Q_CLASSINFO("Property", "minZ")
    Q_CLASSINFO("Title", "the minimum radius in the z direction")
    Q_CLASSINFO("Quantity", "length")

    Q_CLASSINFO("Property", "maxZ")
    Q_CLASSINFO("Title", "the maximum radius in the z direction")
    Q_CLASSINFO("Quantity", "length")

    Q_CLASSINFO("Property", "pointsZ")
    Q_CLASSINFO("Title", "the number of grid points in the z direction")
    Q_CLASSINFO("MinValue", "5")
    Q_CLASSINFO("MaxValue", "100000")
    Q_CLASSINFO("Default", "250")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE GenLinCubDustGridStructure();

protected:
    /** This function verifies the number of bins and the maximum extent of the grid in the X, Y
        and Z directions, and then calculates the grid points along each of these axes. */
    void setupSelfBefore();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the minimum radius in the x direction. */
    Q_INVOKABLE void setMinX(double value);

    /** Returns the minimum radius in the x direction. */
    Q_INVOKABLE double minX() const;

    /** Sets the maximum radius in the x direction. */
    Q_INVOKABLE void setMaxX(double value);

    /** Returns the maximum radius in the x direction. */
    Q_INVOKABLE double maxX() const;

    /** Sets the number of grid points in the x direction. */
    Q_INVOKABLE void setPointsX(int value);

    /** Returns the number of grid points in the x direction. */
    Q_INVOKABLE int pointsX() const;

    /** Sets the minimum radius in the y direction. */
    Q_INVOKABLE void setMinY(double value);

    /** Returns the minimum radius in the y direction. */
    Q_INVOKABLE double minY() const;

    /** Sets the maximum radius in the y direction. */
    Q_INVOKABLE void setMaxY(double value);

    /** Returns the maximum radius in the y direction. */
    Q_INVOKABLE double maxY() const;

    /** Sets the number of grid points in the y direction. */
    Q_INVOKABLE void setPointsY(int value);

    /** Returns the number of grid points in the y direction. */
    Q_INVOKABLE int pointsY() const;

    /** Sets the minimum radius in the z direction. */
    Q_INVOKABLE void setMinZ(double value);

    /** Returns the minimum radius in the z direction. */
    Q_INVOKABLE double minZ() const;

    /** Sets the maximum radius in the z direction. */
    Q_INVOKABLE void setMaxZ(double value);

    /** Returns the maximum radius in the z direction. */
    Q_INVOKABLE double maxZ() const;

    /** Sets the number of grid points in the z direction. */
    Q_INVOKABLE void setPointsZ(int value);

    /** Returns the number of grid points in the z direction. */
    Q_INVOKABLE int pointsZ() const;
};

////////////////////////////////////////////////////////////////////

#endif // GENLINCUBDUSTGRIDSTRUCTURE_HPP
