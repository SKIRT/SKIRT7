/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef SPHERICALCLIPGEOMETRYDECORATOR_HPP
#define SPHERICALCLIPGEOMETRYDECORATOR_HPP

#include "ClipGeometryDecorator.hpp"

////////////////////////////////////////////////////////////////////

/** The SphericalClipGeometryDecorator class is a decorator that adjusts another geometry by
    setting the density equal to zero inside or outside a sphere with given position and radius.
    The dimension of the geometry implemented by a SphericalClipGeometryDecorator object depends on
    the symmetries of the geometry being decorated and on the position of the clipping sphere. */
class SphericalClipGeometryDecorator : public ClipGeometryDecorator
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a decorator that clips another geometry using a sphere")

    Q_CLASSINFO("Property", "radius")
    Q_CLASSINFO("Title", "the radius of the sphere")
    Q_CLASSINFO("Quantity", "length")
    Q_CLASSINFO("MinValue", "0")

    Q_CLASSINFO("Property", "centerX")
    Q_CLASSINFO("Title", "the x coordinate of the sphere's center")
    Q_CLASSINFO("Quantity", "length")
    Q_CLASSINFO("Default", "0")

    Q_CLASSINFO("Property", "centerY")
    Q_CLASSINFO("Title", "the y coordinate of the sphere's center")
    Q_CLASSINFO("Quantity", "length")
    Q_CLASSINFO("Default", "0")

    Q_CLASSINFO("Property", "centerZ")
    Q_CLASSINFO("Title", "the z coordinate of the sphere's center")
    Q_CLASSINFO("Quantity", "length")
    Q_CLASSINFO("Default", "0")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE SphericalClipGeometryDecorator();

protected:
    /** This function calculates some frequently used values. */
    void setupSelfBefore();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the radius of the sphere. */
    Q_INVOKABLE void setRadius(double value);

    /** Returns the radius of the sphere. */
    Q_INVOKABLE double radius() const;

    /** Sets the x coordinate of the sphere's center. */
    Q_INVOKABLE void setCenterX(double value);

    /** Returns the x coordinate of the sphere's center. */
    Q_INVOKABLE double centerX() const;

    /** Sets the y coordinate of the sphere's center. */
    Q_INVOKABLE void setCenterY(double value);

    /** Returns the y coordinate of the sphere's center. */
    Q_INVOKABLE double centerY() const;

    /** Sets the z coordinate of the sphere's center. */
    Q_INVOKABLE void setCenterZ(double value);

    /** Returns the z coordinate of the sphere's center. */
    Q_INVOKABLE double centerZ() const;

    //======================== Other Functions =======================

public:
    /** This function returns the dimension of the geometry, which is the larger of two dimensions:
        the dimension of the geometry being decorated and the dimension of the hole. The dimension
        of the hole is 1 if its center is at the origin, 2 if the center is on the Z-axis, and 3 if
        the center is elsewhere. */
    int dimension() const;

    /** This function returns true if the specified position is inside the sphere defined by the
        properties of this class. */
    bool inside(Position bfr) const;

    //======================== Data Members ========================

private:
    // data members for discoverable attributes
    double _radius, _centerX, _centerY, _centerZ;

    // values initialized during setup
    Position _center;
    double _radius2;
};

////////////////////////////////////////////////////////////////////

#endif // SPHERICALCLIPGEOMETRYDECORATOR_HPP
