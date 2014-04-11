/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef YDUSTCOMPNORMALIZATION_HPP
#define YDUSTCOMPNORMALIZATION_HPP

#include "DustCompNormalization.hpp"

//////////////////////////////////////////////////////////////////////

/** YDustCompNormalization is a class that sets the normalization of a general 3D dust component by
    defining the Y-axis optical depth at an arbitrary wavelength. The Y-axis optical depth is
    defined as the integral of the opacity along the entire Y-axis, \f[
    \tau_\lambda^{\text{Y}} = \int_{-\infty}^\infty k_\lambda(0,y,0)\, {\text{d}}y. \f] */
class YDustCompNormalization : public DustCompNormalization
{
    Q_OBJECT
    Q_CLASSINFO("Title", "normalization by defining the Y-axis optical depth at some wavelength")

    Q_CLASSINFO("Property", "wavelength")
    Q_CLASSINFO("Title", "the wavelength at which to specify the optical depth")
    Q_CLASSINFO("Quantity", "wavelength")
    Q_CLASSINFO("MinValue", "1 A")
    Q_CLASSINFO("MaxValue", "1 m")

    Q_CLASSINFO("Property", "opticalDepth")
    Q_CLASSINFO("Title", "the Y-axis optical depth at this wavelength")
    Q_CLASSINFO("MinValue", "0")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE YDustCompNormalization();

protected:
    /** This function verifies that the dust mass has been appropriately set. */
    void setupSelfBefore();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** This function sets the wavelength at which the optical depth is defined for normalizing the
        dust component. */
    Q_INVOKABLE void setWavelength(double value);

    /** This function returns the wavelength at which the optical depth is defined for normalizing
        the dust component. */
    Q_INVOKABLE double wavelength() const;

    /** This function sets the optical depth used for normalizing the dust component. */
    Q_INVOKABLE void setOpticalDepth(double value);

    /** This function returns the optical depth used for normalizing the dust component. */
    Q_INVOKABLE double opticalDepth() const;

    //======================== Other Functions =======================

public:
    /** This function returns the appropriate normalization factor for the specified
        geometry and dust mixture. */
    double normalizationFactor(const Geometry* geom, const DustMix* mix) const;

    //======================== Data Members ========================

private:
    double _wavelength;
    double _tau;
};

//////////////////////////////////////////////////////////////////////

#endif // YDUSTCOMPNORMALIZATION_HPP
