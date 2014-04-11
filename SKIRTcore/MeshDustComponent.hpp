/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef MESHDUSTCOMPONENT_HPP
#define MESHDUSTCOMPONENT_HPP

#include "Position.hpp"
#include "SimulationItem.hpp"

class DustMix;

//////////////////////////////////////////////////////////////////////

/** A MeshDustComponent instance represents a dust component in a dust distribution of class
    AdaptiveMeshDustDistribution or VoronoiDustDistribution. Its properties specify the column(s)
    in the mesh data file defining the dust density distribution for the component, and the dust
    mix to be used for the component.

    The property \em densityIndex specifies the index \f$g_d\;(0\le g_d \le N_{fields}-1)\f$ of the
    field that should be interpreted as a density distribution \f$D\f$ over the domain. If the
    property \em multiplierIndex is nonnegative (the default value is -1), it specifies the index
    \f$g_m\;(0\le g_m \le N_{fields}-1)\f$ for the field that will serve as a multiplication factor
    for the density field. Finally, the density is always multiplied by the fraction \f$f\f$
    specified by the property \em densityFraction (with a default value of 1). In other words the
    density field value for each cell is determined by \f$D=F_{g_d}\times F_{g_m}\times f\f$. */
class MeshDustComponent : public SimulationItem
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a mesh dust component")

    Q_CLASSINFO("Property", "densityIndex")
    Q_CLASSINFO("Title", "the index of the column defining the density distribution for the dust component")
    Q_CLASSINFO("MinValue", "0")
    Q_CLASSINFO("MaxValue", "99")
    Q_CLASSINFO("Default", "0")

    Q_CLASSINFO("Property", "multiplierIndex")
    Q_CLASSINFO("Title", "the index of the column defining an extra multiplication factor, or -1")
    Q_CLASSINFO("MinValue", "-1")
    Q_CLASSINFO("MaxValue", "99")
    Q_CLASSINFO("Default", "-1")

    Q_CLASSINFO("Property", "densityFraction")
    Q_CLASSINFO("Title", "the fraction of the density actually locked up in dust grains")
    Q_CLASSINFO("MinValue", "0")
    Q_CLASSINFO("MaxValue", "1")
    Q_CLASSINFO("Default", "1")

    Q_CLASSINFO("Property", "mix")
    Q_CLASSINFO("Title", "the dust mixture for the dust component")
    Q_CLASSINFO("Default", "InterstellarDustMix")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor */
    Q_INVOKABLE MeshDustComponent();

protected:
    /** This function verifies that the properties have been appropriately set. */
    void setupSelfBefore();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the index of the column in the data file that defines the density distribution
        for this dust component. */
    Q_INVOKABLE void setDensityIndex(int value);

    /** Returns the index of the column in the data file that defines the density distribution
        for this dust component. */
    Q_INVOKABLE int densityIndex() const;

    /** Sets the index of the column in the data file that defines an extra multiplication factor
        for the dust density distribution, or -1 if there is no such multiplication factor. */
    Q_INVOKABLE void setMultiplierIndex(int value);

    /** Returns the index of the column in the data file that defines an extra multiplication
        factor for the dust density distribution, or -1 if there is no such multiplication factor.
        */
    Q_INVOKABLE int multiplierIndex() const;

    /** Sets the fraction of the density actually locked up in dust grains. */
    Q_INVOKABLE void setDensityFraction(double value);

    /** Returns the fraction of the density actually locked up in dust grains. */
    Q_INVOKABLE double densityFraction() const;

    /** This function sets the dust mixture of the dust component. */
    Q_INVOKABLE void setMix(DustMix* value);

    /** This function returns the dust mixture of the dust component. */
    Q_INVOKABLE DustMix* mix() const;

    //======================== Other Functions =======================

    //======================== Data Members ========================

private:
    int _densityIndex;
    int _multiplierIndex;
    double _densityFraction;
    DustMix* _mix;
};

//////////////////////////////////////////////////////////////////////

#endif // MESHDUSTCOMPONENT_HPP
