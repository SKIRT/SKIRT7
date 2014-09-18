/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef LOGWAVELENGTHGRID_HPP
#define LOGWAVELENGTHGRID_HPP

#include "PanWavelengthGrid.hpp"

////////////////////////////////////////////////////////////////////

/** LogWavelengthGrid is a subclass of the PanWavelengthGrid class representing logarithmically
    distributed wavelength grids. */
class LogWavelengthGrid : public PanWavelengthGrid
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a logarithmic wavelength grid")

    Q_CLASSINFO("Property", "minWavelength")
    Q_CLASSINFO("Title", "the shortest wavelength")
    Q_CLASSINFO("Quantity", "wavelength")
    Q_CLASSINFO("MinValue", "1 A")
    Q_CLASSINFO("MaxValue", "1 m")

    Q_CLASSINFO("Property", "maxWavelength")
    Q_CLASSINFO("Title", "the longest wavelength")
    Q_CLASSINFO("Quantity", "wavelength")
    Q_CLASSINFO("MinValue", "1 A")
    Q_CLASSINFO("MaxValue", "1 m")

    Q_CLASSINFO("Property", "points")
    Q_CLASSINFO("Title", "the number of wavelength grid points")
    Q_CLASSINFO("MinValue", "3")
    Q_CLASSINFO("Default", "25")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE LogWavelengthGrid();

protected:
    /** This function constructs the vector with wavelengths. The \f$N\f$ wavelength grid points
        are distributed logarithmically between \f$\lambda_{\text{min}}\f$ and
        \f$\lambda_{\text{max}}\f$. */
    void setupSelfBefore();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the shortest wavelength. */
    Q_INVOKABLE void setMinWavelength(double value);

    /** Returns the shortest wavelength. */
    Q_INVOKABLE double minWavelength() const;

    /** Sets the longest wavelength. */
    Q_INVOKABLE void setMaxWavelength(double value);

    /** Returns the longest wavelength. */
    Q_INVOKABLE double maxWavelength() const;

    /** Sets the number of wavelength grid points. */
    Q_INVOKABLE void setPoints(int value);

    /** Returns the number of wavelength grid points. */
    Q_INVOKABLE int points() const;

    //======================== Data Members ========================

private:
    double _lambdamin;
    double _lambdamax;
};

////////////////////////////////////////////////////////////////////

#endif // LOGWAVELENGTHGRID_HPP
