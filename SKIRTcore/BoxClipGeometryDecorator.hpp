/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef BOXCLIPGEOMETRYDECORATOR_HPP
#define BOXCLIPGEOMETRYDECORATOR_HPP

#include "Box.hpp"
#include "ClipGeometryDecorator.hpp"

////////////////////////////////////////////////////////////////////

/** The BoxClipGeometryDecorator class is a decorator that adjusts another geometry by setting the
    density equal to zero inside or outside a given cuboidal bounding box. */
class BoxClipGeometryDecorator : public ClipGeometryDecorator, public Box
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a decorator that clips another geometry using a cubodial box")

    Q_CLASSINFO("Property", "minX")
    Q_CLASSINFO("Title", "the start point of the box in the X direction")
    Q_CLASSINFO("Quantity", "length")

    Q_CLASSINFO("Property", "maxX")
    Q_CLASSINFO("Title", "the end point of the box in the X direction")
    Q_CLASSINFO("Quantity", "length")

    Q_CLASSINFO("Property", "minY")
    Q_CLASSINFO("Title", "the start point of the box in the Y direction")
    Q_CLASSINFO("Quantity", "length")

    Q_CLASSINFO("Property", "maxY")
    Q_CLASSINFO("Title", "the end point of the box in the Y direction")
    Q_CLASSINFO("Quantity", "length")

    Q_CLASSINFO("Property", "minZ")
    Q_CLASSINFO("Title", "the start point of the box in the Z direction")
    Q_CLASSINFO("Quantity", "length")

    Q_CLASSINFO("Property", "maxZ")
    Q_CLASSINFO("Title", "the end point of the box in the Z direction")
    Q_CLASSINFO("Quantity", "length")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE BoxClipGeometryDecorator();

    /** This function verifies that the box has a positive volume. */
    void setupSelfBefore();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the start point \f$x_{\text{min}}\f$ of the box in the X direction. */
    Q_INVOKABLE void setMinX(double value);

    /** Returns the start point \f$x_{\text{min}}\f$ of the box in the X direction. */
    Q_INVOKABLE double minX() const;

    /** Sets the end point \f$x_{\text{max}}\f$ of the box in the X direction. */
    Q_INVOKABLE void setMaxX(double value);

    /** Returns the end point \f$x_{\text{max}}\f$ of the box in the X direction. */
    Q_INVOKABLE double maxX() const;

    /** Sets the start point \f$y_{\text{min}}\f$ of the box in the Y direction. */
    Q_INVOKABLE void setMinY(double value);

    /** Returns the start point \f$y_{\text{min}}\f$ of the box in the Y direction. */
    Q_INVOKABLE double minY() const;

    /** Sets the end point \f$y_{\text{max}}\f$ of the box in the Y direction. */
    Q_INVOKABLE void setMaxY(double value);

    /** Returns the end point \f$y_{\text{max}}\f$ of the box in the Y direction. */
    Q_INVOKABLE double maxY() const;

    /** Sets the start point \f$z_{\text{min}}\f$ of the box in the Z direction. */
    Q_INVOKABLE void setMinZ(double value);

    /** Returns the start point \f$z_{\text{min}}\f$ of the box in the Z direction. */
    Q_INVOKABLE double minZ() const;

    /** Sets the end point \f$z_{\text{max}}\f$ of the box in the Z direction. */
    Q_INVOKABLE void setMaxZ(double value);

    /** Returns the end point \f$z_{\text{max}}\f$ of the box in the Z direction. */
    Q_INVOKABLE double maxZ() const;

    //======================== Other Functions =======================

public:
    /** This function returns the dimension of the geometry, which is 3 for this
        class since a box has no specific symmetry. */
    int dimension() const;

    /** This function returns true if the specified position is inside the box defined by the
        properties of this class. */
    bool inside(Position bfr) const;

};

////////////////////////////////////////////////////////////////////

#endif // BOXCLIPGEOMETRYDECORATOR_HPP
