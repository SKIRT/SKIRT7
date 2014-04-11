/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef OLIGOWAVELENGTHGRID_HPP
#define OLIGOWAVELENGTHGRID_HPP

#include "WavelengthGrid.hpp"

//////////////////////////////////////////////////////////////////////

/** OligoWavelengthGrid is a subclass of the general WavelengthGrid class representing one or more
    distinct wavelengths rather than a discretized wavelength range. It is intended for use with
    oligochromatic simulations, which don't calculate the dust temperature by integrating over a
    wavelength range. */
class OligoWavelengthGrid : public WavelengthGrid
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a list of one or more distinct wavelengths")

    Q_CLASSINFO("Property", "wavelengths")
    Q_CLASSINFO("Title", "the wavelengths (λ)")
    Q_CLASSINFO("Quantity", "wavelength")
    Q_CLASSINFO("MinValue", "1 A")
    Q_CLASSINFO("MaxValue", "1 m")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE OligoWavelengthGrid();

    /** This function initializes the wavelength bin widths maintained by the WavelengthGrid
        base class. Since we're doing multiple monochromatic simulations for distinct wavelengths,
        the bins are taken to be very small and independent from the other wavelengths. */
    void setupSelfAfter();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the list of one or more distinct wavelengths in this "grid". The specified list must
        not be empty. */
    Q_INVOKABLE void setWavelengths(QList<double> value);

    /** Returns the list of distinct wavelengths in this "grid". */
    Q_INVOKABLE QList<double> wavelengths() const;

    //======================== Other Functions =======================

public:
    /** This function's implementation for this class always returns false, since an
        OligoWavelengthGrid contains individual distinct wavelengths for use by oligochromatic
        simulations. */
    bool issampledrange() const;
};

//////////////////////////////////////////////////////////////////////

#endif // OLIGOWAVELENGTHGRID_HPP
