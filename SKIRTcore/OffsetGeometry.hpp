/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef OFFSETGEOMETRY_HPP
#define OFFSETGEOMETRY_HPP

#include "Geometry.hpp"

////////////////////////////////////////////////////////////////////

/** The OffsetGeometry class is a Geometry decorator that adds an arbitrary offset to
    any geometry. The properties of an OffsetGeometry object include (1) a reference
    to the Geometry object being decorated and (2) three offsets in the x, y, and z
    directions. The resulting geometry is identical to the geometry being decorated, except
    that the density distribution is shifted over the specified offset. The geometry
    implemented by an OffsetGeometry object is 2D (axial symmetry) or 3D (no symmetries)
    depending on the symmetries of the geometry being decorated and on the specified offset.
    Specifically, it is 2D if the geometry being decorated is 1D or 2D and the offsets in
    the x and y directions are both zero. It is 3D if the geometry being decorated is 3D, or
    if at least one of the offsets in the x and y directions is nonzero. */
class OffsetGeometry : public Geometry
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a geometry that adds an offset to any geometry")

    Q_CLASSINFO("Property", "geometry")
    Q_CLASSINFO("Title", "the geometry to be offset")

    Q_CLASSINFO("Property", "offsetX")
    Q_CLASSINFO("Title", "the offset in the x direction")
    Q_CLASSINFO("Quantity", "length")
    Q_CLASSINFO("Default", "0")

    Q_CLASSINFO("Property", "offsetY")
    Q_CLASSINFO("Title", "the offset in the y direction")
    Q_CLASSINFO("Quantity", "length")
    Q_CLASSINFO("Default", "0")

    Q_CLASSINFO("Property", "offsetZ")
    Q_CLASSINFO("Title", "the offset in the z direction")
    Q_CLASSINFO("Quantity", "length")
    Q_CLASSINFO("Default", "0")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE OffsetGeometry();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the geometry to be offset (i.e. the geometry being decorated). */
    Q_INVOKABLE void setGeometry(Geometry* value);

    /** Returns the geometry to be offset (i.e. the geometry being decorated). */
    Q_INVOKABLE Geometry* geometry() const;

    /** Sets the offset in the x direction. */
    Q_INVOKABLE void setOffsetX(double value);

    /** Returns the offset in the x direction. */
    Q_INVOKABLE double offsetX() const;

    /** Sets the offset in the y direction. */
    Q_INVOKABLE void setOffsetY(double value);

    /** Returns the offset in the y direction. */
    Q_INVOKABLE double offsetY() const;

    /** Sets the offset in the z direction. */
    Q_INVOKABLE void setOffsetZ(double value);

    /** Returns the offset in the z direction. */
    Q_INVOKABLE double offsetZ() const;

    //======================== Other Functions =======================

public:
    /** This function returns the dimension of the geometry. It is 2 if the dimension of the
        geometry being decorated is 1 or 2 and the offsets in the x and y directions are both zero.
        It is 3 if the dimension of the geometry being decorated is 3, or if at least one of the
        offsets in the x and y directions is nonzero. */
    int dimension() const;

    /** This function returns the density \f$\rho({\bf{r}})\f$ at the position
        \f${\bf{r}}\f$. It calls the density() function for the geometry being decorated with the
        translated position \f${\bf{r}}-{\bf{r}_\text{offset}}\f$. */
    double density(Position bfr) const;

    /** This function generates a random position from the geometry, by drawing a random
        point from the three-dimensional probability density \f$p({\bf{r}})\, {\text{d}}{\bf{r}} =
        \rho({\bf{r}})\, {\text{d}}{\bf{r}}\f$. It calls the density() function for the geometry
        being decorated and returns the translated position \f${\bf{r}}+{\bf{r}_\text{offset}}\f$.
        */
    Position generatePosition() const;

    /** This function returns the X-axis surface density, i.e. the integration of the density
        along the entire X-axis, \f[ \Sigma_X = \int_{-\infty}^\infty \rho(x,0,0)\,{\text{d}}x. \f]
        It is impossible to calculate this value for a random value of the offset. The best
        option we have is to return the X-axis surface density of the original geometry, which
        is the true value in case there is only an offset in the x direction. */
    double SigmaX() const;

    /** This function returns the Y-axis surface density, i.e. the integration of the density
        along the entire Y-axis, \f[ \Sigma_Y = \int_{-\infty}^\infty \rho(0,y,0)\,{\text{d}}y. \f]
        It is impossible to calculate this value for a random value of the offset. The best
        option we have is to return the Y-axis surface density of the original geometry, which
        is the true value in case there is only an offset in the y direction. */
    double SigmaY() const;

    /** This function returns the Z-axis surface density, i.e. the integration of the density
        along the entire Z-axis, \f[ \Sigma_Z = \int_{-\infty}^\infty \rho(0,0,z)\,{\text{d}}z. \f]
        It is impossible to calculate this value for a random value of the offset. The best
        option we have is to return the Z-axis surface density of the original geometry, which
        is the true value in case there is only an offset in the z direction. */
    double SigmaZ() const;

    //======================== Data Members ========================

private:
    // data members for discoverable attributes
    Geometry* _geometry;
    double _offsetX, _offsetY, _offsetZ;
};

////////////////////////////////////////////////////////////////////

#endif // OFFSETGEOMETRY_HPP
