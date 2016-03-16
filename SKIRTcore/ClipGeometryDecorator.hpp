/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef CLIPGEOMETRYDECORATOR_HPP
#define CLIPGEOMETRYDECORATOR_HPP

#include "Geometry.hpp"

////////////////////////////////////////////////////////////////////

/** The abstract ClipGeometryDecorator class implements a decorator that adjusts another geometry
    by setting the density equal to zero inside or outside a region defined in a subclass. Each
    ClipGeometryDecorator subclass must implement the virtual functions dimension() and inside().
    The decorator increases the density in the remaining region with a constant factor to ensure
    that the total mass remains equal to one. The current implementation does not properly adjust
    the surface densities along the coordinate axes for the mass taken away by the cavity. */
class ClipGeometryDecorator : public Geometry
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a decorator that clips another geometry")

    Q_CLASSINFO("Property", "geometry")
    Q_CLASSINFO("Title", "the geometry to be clipped")

    Q_CLASSINFO("Property", "remove")
    Q_CLASSINFO("Title", "the region to be removed")
    Q_CLASSINFO("Inside", "the inner region (creating a cavity)")
    Q_CLASSINFO("Outside", "the outer region (cropping)")

    //============= Construction - Setup - Destruction =============

protected:
    /** The default constructor. */
    ClipGeometryDecorator();

    /** This function estimates the fraction \f$\chi\f$ of the mass from the original model taken
        away by the clipping. It samples the density of the geometry being decorated, and counts
        the number of generated positions that fall in the removed region. This value is used to
        renormalize the decorated density distribution to unity: the factor by which the original
        density has to be multiplied is simply \f$1/(1-\chi)\f$. */
    void setupSelfAfter();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the geometry to be clipped (i.e. the geometry being decorated). */
    Q_INVOKABLE void setGeometry(Geometry* value);

    /** Returns the geometry to be clipped (i.e. the geometry being decorated). */
    Q_INVOKABLE Geometry* geometry() const;

    /** The enumeration type indicating which region to remove (Inside or Outside). */
    Q_ENUMS(Remove)
    enum Remove { Inside, Outside };

    /** Sets the enumeration value indicating which region to remove. */
    Q_INVOKABLE void setRemove(Remove value);

    /** Returns the enumeration value indicating which region to remove. */
    Q_INVOKABLE Remove remove() const;

    //======================== Other Functions =======================

public:
    /** This function returns the density \f$\rho({\bf{r}})\f$ at the position \f${\bf{r}}\f$. It
        is zero in the removed region, and equal to the density of the geometry being decorated
        elsewhere, after an adjustment is made to account for the clipping. */
    double density(Position bfr) const;

    /** This function generates a random position from the geometry, by drawing a random point from
        the three-dimensional probability density \f$p({\bf{r}})\, {\text{d}}{\bf{r}} =
        \rho({\bf{r}})\, {\text{d}}{\bf{r}}\f$. It repeatedly calls the density() function for the
        geometry being decorated until a position is returned that does not lie in the removed
        region. */
    Position generatePosition() const;

    /** This function returns the X-axis surface density, i.e. the integration of the density along
        the entire X-axis, \f[ \Sigma_X = \int_{-\infty}^\infty \rho(x,0,0)\,{\text{d}}x. \f] It
        simply calls the corresponding function of the geometry being decorated. */
    double SigmaX() const;

    /** This function returns the Y-axis surface density, i.e. the integration of the density along
        the entire Y-axis, \f[ \Sigma_Y = \int_{-\infty}^\infty \rho(0,y,0)\,{\text{d}}y. \f] It
        simply calls the corresponding function of the geometry being decorated. */
    double SigmaY() const;

    /** This function returns the Z-axis surface density, i.e. the integration of the density along
        the entire Z-axis, \f[ \Sigma_Z = \int_{-\infty}^\infty \rho(0,0,z)\,{\text{d}}z. \f] It
        simply calls the corresponding function of the geometry being decorated. */
    double SigmaZ() const;

    /** This function implements part of the AngularDistribution interface. It returns the
        probability \f$P(\Omega)\f$ for a given direction \f$(\theta,\phi)\f$ at the specified
        position. For this decorator, the function simply calls the corresponding function for the
        geometry being decorated. */
    double probabilityForDirection(int ell, Position bfr, Direction bfk) const;

    /** This function implements part of the AngularDistribution interface. It generates a random
        direction \f$(\theta,\phi)\f$ drawn from the probability distribution \f$P(\Omega)
        \,{\mathrm{d}}\Omega\f$ at the specified position. For this decorator, the function simply
        calls the corresponding function for the geometry being decorated. */
    Direction generateDirection(int ell, Position bfr) const;

protected:
    /** This pure virtual function, to be implemented by a subclass, returns true if the specified
        position is inside the boundary defined by the subclass, i.e. the point is in the region
        that would be carved away when creating a cavity, or in the region that would be retained
        when cropping. */
    virtual bool inside(Position bfr) const = 0;

    //======================== Data Members ========================

private:
    // data members for discoverable attributes
    Geometry* _geometry;
    Remove _remove;

protected:
    // values initialized during setup
    double _norm;
};

////////////////////////////////////////////////////////////////////

#endif // CLIPGEOMETRYDECORATOR_HPP
