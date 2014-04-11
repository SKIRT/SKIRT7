/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef NESTEDLOGWAVELENGTHGRID_HPP
#define NESTEDLOGWAVELENGTHGRID_HPP

#include "PanWavelengthGrid.hpp"

////////////////////////////////////////////////////////////////////

/** NestedLogWavelengthGrid is a subclass of the PanWavelengthGrid class representing hybrid grids
    consisting of a logarithmically distributed wavelength grid in which another, more compact
    logarithmic grid is embedded. It can be very useful to get higher-resolution spectra in a
    particular wavelength grid while still covering a broad wavelength range. */
class NestedLogWavelengthGrid : public PanWavelengthGrid
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a nested logarithmic wavelength grid")

    Q_CLASSINFO("Property", "minWavelength")
    Q_CLASSINFO("Title", "the shortest wavelength of the low-resolution grid")
    Q_CLASSINFO("Quantity", "wavelength")
    Q_CLASSINFO("MinValue", "1 A")
    Q_CLASSINFO("MaxValue", "1 m")

    Q_CLASSINFO("Property", "maxWavelength")
    Q_CLASSINFO("Title", "the longest wavelength of the low-resolution grid")
    Q_CLASSINFO("Quantity", "wavelength")
    Q_CLASSINFO("MinValue", "1 A")
    Q_CLASSINFO("MaxValue", "1 m")

    Q_CLASSINFO("Property", "points")
    Q_CLASSINFO("Title", "the number of wavelength grid points in the low-resolution grid")
    Q_CLASSINFO("MinValue", "3")
    Q_CLASSINFO("Default", "25")

    Q_CLASSINFO("Property", "minWavelengthSubGrid")
    Q_CLASSINFO("Title", "the shortest wavelength of the high-resolution subgrid")
    Q_CLASSINFO("Quantity", "wavelength")
    Q_CLASSINFO("MinValue", "1 A")
    Q_CLASSINFO("MaxValue", "1 m")

    Q_CLASSINFO("Property", "maxWavelengthSubGrid")
    Q_CLASSINFO("Title", "the longest wavelength of the high-resolution subgrid")
    Q_CLASSINFO("Quantity", "wavelength")
    Q_CLASSINFO("MinValue", "1 A")
    Q_CLASSINFO("MaxValue", "1 m")

    Q_CLASSINFO("Property", "pointsSubGrid")
    Q_CLASSINFO("Title", "the number of wavelength grid points in the high-resolution subgrid")
    Q_CLASSINFO("MinValue", "3")
    Q_CLASSINFO("Default", "25")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE NestedLogWavelengthGrid();

protected:
    /** This function constructs the vector with wavelengths. First, \f$N\f$ wavelength grid points
        are distributed logarithmically between \f$\lambda_{\text{min}}\f$ and
        \f$\lambda_{\text{max}}\f$. Next, \f$N_{\text{zoom}}\f$ wavelength grid points are
        distributed logarithmically between \f$\lambda_{\text{zoom,min}}\f$ and
        \f$\lambda_{\text{zoom,max}}\f$. Both sets of wavelength grids are subsequently merged and
        overlapping grid points are erased. */
    void setupSelfBefore();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the shortest wavelength of the low-resolution grid. */
    Q_INVOKABLE void setMinWavelength(double value);

    /** Returns the shortest wavelength of the low-resolution grid. */
    Q_INVOKABLE double minWavelength() const;

    /** Sets the longest wavelength of the low-resolution grid. */
    Q_INVOKABLE void setMaxWavelength(double value);

    /** Returns the longest wavelength of the low-resolution grid. */
    Q_INVOKABLE double maxWavelength() const;

    /** Sets the number of wavelength grid points in the low-resolution grid. */
    Q_INVOKABLE void setPoints(int value);

    /** Returns the number of wavelength grid points in the low-resolution grid. */
    Q_INVOKABLE int points() const;

    /** Sets the shortest wavelength of the high-resolution subgrid. */
    Q_INVOKABLE void setMinWavelengthSubGrid(double value);

    /** Returns the shortest wavelength of the high-resolution subgrid. */
    Q_INVOKABLE double minWavelengthSubGrid() const;

    /** Sets the longest wavelength of the high-resolution subgrid. */
    Q_INVOKABLE void setMaxWavelengthSubGrid(double value);

    /** Returns the longest wavelength of the high-resolution subgrid. */
    Q_INVOKABLE double maxWavelengthSubGrid() const;

    /** Sets the number of wavelength grid points in the high-resolution subgrid. */
    Q_INVOKABLE void setPointsSubGrid(int value);

    /** Returns the number of wavelength grid points in the high-resolution subgrid. */
    Q_INVOKABLE int pointsSubGrid() const;

    //======================== Data Members ========================

private:
    double _lambdamin;
    double _lambdamax;
    double _lambdazoommin;
    double _lambdazoommax;
    int _Nlambdazoom;
};

////////////////////////////////////////////////////////////////////

#endif // NESTEDLOGWAVELENGTHGRID_HPP
