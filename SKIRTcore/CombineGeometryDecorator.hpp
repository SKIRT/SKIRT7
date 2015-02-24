/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef COMBINEGEOMETRYDECORATOR_HPP
#define COMBINEGEOMETRYDECORATOR_HPP

#include "Geometry.hpp"

//////////////////////////////////////////////////////////////////////

/**  */
class CombineGeometryDecorator : public Geometry
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a decorator that combines two different geometries")

    Q_CLASSINFO("Property", "firstGeometry")
    Q_CLASSINFO("Title", "the first geometry")

    Q_CLASSINFO("Property", "firstWeight")
    Q_CLASSINFO("Title", "the weight of the first geometry")
    Q_CLASSINFO("MinValue", "0")

    Q_CLASSINFO("Property", "secondGeometry")
    Q_CLASSINFO("Title", "the second geometry")

    Q_CLASSINFO("Property", "secondWeight")
    Q_CLASSINFO("Title", "the weight of the second geometry")
    Q_CLASSINFO("MinValue", "0")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE CombineGeometryDecorator();

protected:
    /** This function verifies that at least one geometry has been added. */
    void setupSelfBefore();

    /** This function normalizes the weight of the different geometries. */
    void setupSelfAfter();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the first geometry. */
    Q_INVOKABLE void setFirstGeometry(Geometry* value);

    /** Returns the first geometry. */
    Q_INVOKABLE Geometry* firstGeometry() const;

    /** Sets the weight of the first geometry. */
    Q_INVOKABLE void setFirstWeight(double value);

    /** Returns the weight of the first geometry. */
    Q_INVOKABLE double firstWeight() const;

    /** Sets the second geometry. */
    Q_INVOKABLE void setSecondGeometry(Geometry* value);

    /** Returns the second geometry. */
    Q_INVOKABLE Geometry* secondGeometry() const;

    /** Sets the weight of the second geometry. */
    Q_INVOKABLE void setSecondWeight(double value);

    /** Returns the weight of the second geometry. */
    Q_INVOKABLE double secondWeight() const;

    //======================== Other Functions =======================

public:
    /** This function returns the dimension of the combined geometry, which depends on the (lack of)
        symmetry in the geometries of its stellar components. A value of 1 means spherical
        symmetry, 2 means axial symmetry and 3 means none of these symmetries. The geometry
        with the least symmetry (i.e. the highest dimension) determines the result for
        the whole system. */
    int dimension() const;

    /** This function returns the density \f$\rho({\bf{r}})\f$ at the position
        \f${\bf{r}}\f$. It adds the contribution of the two components, weighted by their
        weights. */
    double density(Position bfr) const;

    /** This function generates a random position from the geometry, by drawing a random
        point from the three-dimensional probability density \f$p({\bf{r}})\, {\text{d}}{\bf{r}} =
        \rho({\bf{r}})\, {\text{d}}{\bf{r}}\f$. It first generates a random component, and
        subsequently generates a random position from this component by calling the corresponding
        generatePosition() function. */
    Position generatePosition() const;

    /** This function returns the X-axis surface density, i.e. the integration of the density
        along the entire X-axis, \f[ \Sigma_X = \int_{-\infty}^\infty \rho(x,0,0)\,{\text{d}}x. \f]
        It adds the contribution of the two components, weighted by their weights. */
    double SigmaX() const;

    /** This function returns the Y-axis surface density, i.e. the integration of the density
        along the entire Y-axis, \f[ \Sigma_Y = \int_{-\infty}^\infty \rho(0,y,0)\,{\text{d}}y. \f]
        It adds the contribution of the two components, weighted by their weights. */
    double SigmaY() const;

    /** This function returns the Z-axis surface density, i.e. the integration of the density
        along the entire Z-axis, \f[ \Sigma_Z = \int_{-\infty}^\infty \rho(0,0,z)\,{\text{d}}z. \f]
        It adds the contribution of the two components, weighted by their weights. */
    double SigmaZ() const;

    /** This function implements part of the AngularDistribution interface. It returns the
        probability \f$P(\Omega)\f$ for a given direction \f$(\theta,\phi)\f$ at the specified
        position. It adds the contribution of the two components, weighted by their weights. */
    double probabilityForDirection(Position bfr, Direction bfk) const;

    /** This function implements part of the AngularDistribution interface. It generates a random
        direction \f$(\theta,\phi)\f$ drawn from the probability distribution \f$P(\Omega)
        \,{\mathrm{d}}\Omega\f$ at the specified position. It first generates a random component,
        and subsequently generates a random position from this component by calling the
        corresponding generateDirection(bfr) function. */
    Direction generateDirection(Position bfr) const;

    //======================== Data Members ========================

private:
    Geometry* _geometry1;
    Geometry* _geometry2;
    double _w1, _w2;
    Random* _random;
};

////////////////////////////////////////////////////////////////

#endif // COMBINEGEOMETRYDECORATOR_HPP
