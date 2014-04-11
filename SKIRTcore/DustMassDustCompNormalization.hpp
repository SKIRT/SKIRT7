/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef DUSTMASSDUSTCOMPNORMALIZATION_HPP
#define DUSTMASSDUSTCOMPNORMALIZATION_HPP

#include "DustCompNormalization.hpp"

//////////////////////////////////////////////////////////////////////

/** DustMassDustCompNormalization is a class that sets the normalization of a dust component by
    defining the total dust mass. For the conversion between dust mass and opacity, the absolute
    value of the total (i.e. absorption and scattering) dust opacity \f$\kappa_{\text{V}}\f$ at the
    V-band is necessary, which is given by the DustMix::absolutekappaV() member function. */
class DustMassDustCompNormalization : public DustCompNormalization
{
    Q_OBJECT
    Q_CLASSINFO("Title", "normalization by defining the total dust mass")

    Q_CLASSINFO("Property", "dustMass")
    Q_CLASSINFO("Title", "the total dust mass of the dust component")
    Q_CLASSINFO("Quantity", "mass")
    Q_CLASSINFO("MinValue", "0")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE DustMassDustCompNormalization();

protected:
    /** This function verifies that the dust mass has been appropriately set. */
    void setupSelfBefore();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** This function sets the total dust mass of the dust component to be normalized. */
    Q_INVOKABLE void setDustMass(double value);

    /** This function returns the total dust mass of the dust component to be normalized. */
    Q_INVOKABLE double dustMass() const;

    //======================== Other Functions =======================

public:
    /** This function returns the appropriate normalization factor for the specified
        geometry and dust mixture. */
    double normalizationFactor(const Geometry* geom, const DustMix* mix) const;

    //======================== Data Members ========================

private:
    double _Mdust;
};

//////////////////////////////////////////////////////////////////////

#endif // DUSTMASSDUSTCOMPNORMALIZATION_HPP
