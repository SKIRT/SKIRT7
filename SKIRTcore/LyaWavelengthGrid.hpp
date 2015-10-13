/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef LYAWAVELENGTHGRID_HPP
#define LYAWAVELENGTHGRID_HPP

#include "WavelengthGrid.hpp"

//////////////////////////////////////////////////////////////////////

/** LyaWavelengthGrid is a subclass of the general WavelengthGrid class that is to be used
    for Lyα line radiative transfer simulations. The grid is defined as a uniform grid in velocity
    space. */
class LyaWavelengthGrid : public WavelengthGrid
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a uniformly spaced velocity grid around the Lyα line")

    Q_CLASSINFO("Property", "minVelocity")
    Q_CLASSINFO("Title", "the minimum velocity")
    Q_CLASSINFO("Quantity", "velocity")

    Q_CLASSINFO("Property", "maxVelocity")
    Q_CLASSINFO("Title", "the maximum velocity")
    Q_CLASSINFO("Quantity", "velocity")

    Q_CLASSINFO("Property", "points")
    Q_CLASSINFO("Title", "the number of velocity grid points")
    Q_CLASSINFO("MinValue", "3")
    Q_CLASSINFO("Default", "40")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE LyaWavelengthGrid();

    /** This function initializes the wavelength grid. */
    void setupSelfBefore();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the minimum velocity. */
    Q_INVOKABLE void setMinVelocity(double value);

    /** Returns the minimum velocity. */
    Q_INVOKABLE double minVelocity() const;

    /** Sets the maximum velocity. */
    Q_INVOKABLE void setMaxVelocity(double value);

    /** Returns the maximum velocity. */
    Q_INVOKABLE double maxVelocity() const;

    /** Sets the number of velocity points. */
    Q_INVOKABLE void setPoints(int value);

    /** Returns the number of velocity points. */
    Q_INVOKABLE int points() const;

    //======================== Other Functions =======================

public:
    /** This function's implementation for this class always returns false, since an
        LyaWavelengthGrid contains individual distinct wavelengths for use by oligochromatic
        simulations. */
    bool issampledrange() const;

    //======================== Data Members ========================

private:
    double _vmin;
    double _vmax;
};

//////////////////////////////////////////////////////////////////////

#endif // LYAWAVELENGTHGRID_HPP
