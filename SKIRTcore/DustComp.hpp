/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef DUSTCOMP_HPP
#define DUSTCOMP_HPP

#include <vector>
#include "Position.hpp"
#include "SimulationItem.hpp"

class DustCompNormalization;
class Geometry;
class DustMix;

//////////////////////////////////////////////////////////////////////

/** DustComp is a class that can be used to simulate a dust component. A dust component is
    characterized by a geometrical distribution of the dust, the optical properties of the dust,
    and the total amount of dust. A DustComp class object contains as data members a pointer to a
    Geometry object, a pointer to a dust mixture object of the DustMix class, and a
    normalization factor. */
class DustComp : public SimulationItem
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a dust component")

    Q_CLASSINFO("Property", "geometry")
    Q_CLASSINFO("Title", "the geometry of the dust component")
    Q_CLASSINFO("Default", "ExpDiskGeometry")

    Q_CLASSINFO("Property", "mix")
    Q_CLASSINFO("Title", "the dust mixture of the dust component")
    Q_CLASSINFO("Default", "InterstellarDustMix")

    Q_CLASSINFO("Property", "normalization")
    Q_CLASSINFO("Title", "the type of normalization for the dust component")
    Q_CLASSINFO("Default", "DustMassDustCompNormalization")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor */
    Q_INVOKABLE DustComp();

protected:
    /** This function verifies that the properties have been appropriately set. */
    void setupSelfBefore();

    /** This function calculates the normalization factor based on the chosen normalization. */
    void setupSelfAfter();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** This function sets the dust geometry of the dust component. */
    Q_INVOKABLE void setGeometry(Geometry* value);

    /** This function returns the dust geometry of the dust component. */
    Q_INVOKABLE Geometry* geometry() const;

    /** This function sets the dust mixture of the dust component. */
    Q_INVOKABLE void setMix(DustMix* value);

    /** This function returns the dust mixture of the dust component. */
    Q_INVOKABLE DustMix* mix() const;

    /** This function sets the type of normalization for the dust component. */
    Q_INVOKABLE void setNormalization(DustCompNormalization* value);

    /** This function returns the type of normalization for the dust component. */
    Q_INVOKABLE DustCompNormalization* normalization() const;

    //======================== Other Functions =======================

public:
    /** This function returns the dimension of the dust component, which depends on the (lack of)
        symmetry of its geometry. A value of 1 means spherical symmetry, 2 means axial symmetry and
        3 means none of these symmetries. */
    int dimension();

    /** This function returns the mass density \f$\rho({\bf{r}})\f$ of the dust component at the
        position \f${\bf{r}}\f$. It is just the mass density of the Geometry object multiplied
        by the normalization factor. */
    double density(Position bfr) const;

    /** This function returns the total dust mass of the dust component at the position
        \f${\bf{r}}\f$. It is just the total dust mass of the Geometry object multiplied by the
        normalization factor. */
    double mass() const;

    /** This function returns the X-axis surface density of the dust component. It is just the
        X-axis surface density of the Geometry object multiplied by the normalization factor. */
    double SigmaX() const;

    /** This function returns the Y-axis surface density of the dust component. It is just the
        Y-axis surface density of the Geometry object multiplied by the normalization factor. */
    double SigmaY() const;

    /** This function returns the Z-axis surface density of the dust component. It is just the
        Z-axis surface density of the Geometry object multiplied by the normalization factor. */
    double SigmaZ() const;

    //======================== Data Members ========================

private:
    Geometry* _geom;
    DustMix* _mix;
    DustCompNormalization* _norm;
    double _nf;
};

//////////////////////////////////////////////////////////////////////

#endif // DUSTCOMP_HPP
