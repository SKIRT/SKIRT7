/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef CYLINDRICALCLIPGEOMETRYDECORATOR_HPP
#define CYLINDRICALCLIPGEOMETRYDECORATOR_HPP

#include "ClipGeometryDecorator.hpp"

////////////////////////////////////////////////////////////////////

/** The CylindricalClipGeometryDecorator class is a decorator that adjusts another geometry by
    setting the density equal to zero inside or outside an infinitely long cylinder centered at the
    origin and oriented along the Z-axis. The radius of the cylinder can be chosen. */
class CylindricalClipGeometryDecorator : public ClipGeometryDecorator
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a decorator that clips another geometry using a cylinder")

    Q_CLASSINFO("Property", "radius")
    Q_CLASSINFO("Title", "the radius of the cylinder")
    Q_CLASSINFO("Quantity", "length")
    Q_CLASSINFO("MinValue", "0")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE CylindricalClipGeometryDecorator();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the radius of the cylinder. */
    Q_INVOKABLE void setRadius(double value);

    /** Returns the radius of the cylinder. */
    Q_INVOKABLE double radius() const;

    //======================== Other Functions =======================

public:
    /** This function returns the dimension of the geometry. If the original geometry has a dimension 3,
        so will the new geometry. Otherwise, i.e. if the original geometry is spherically or axisymmetric,
        the dimension is 2. */
    int dimension() const;

    /** This function returns true if the specified position is inside the cylinder defined by the
        properties of this class. */
    bool inside(Position bfr) const;

    /** This function returns the X-axis surface density, i.e. the integration of the density along
        the entire X-axis, \f[ \Sigma_X = \int_{-\infty}^\infty \rho(x,0,0)\,{\text{d}}x. \f] It
        returns the corresponding value of the geometry being decorated after normalization. */
    double SigmaX() const;

    /** This function returns the Y-axis surface density, i.e. the integration of the density along
        the entire Y-axis, \f[ \Sigma_Y = \int_{-\infty}^\infty \rho(0,y,0)\,{\text{d}}y. \f] It
        returns the corresponding value of the geometry being decorated after normalization. */
    double SigmaY() const;

    /** This function returns the Z-axis surface density, i.e. the integration of the density along
        the entire Z-axis, \f[ \Sigma_Z = \int_{-\infty}^\infty \rho(0,0,z)\,{\text{d}}z. \f] If
        the inside region is being removed, this function returns zero; otherwise it returns the
        corresponding value of the geometry being decorated. */
    double SigmaZ() const;

    //======================== Data Members ========================

private:
    // data members for discoverable attributes
    double _radius;
};

////////////////////////////////////////////////////////////////////

#endif // CYLINDRICALCLIPGEOMETRYDECORATOR_HPP
