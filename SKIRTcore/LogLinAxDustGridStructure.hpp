/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef LOGLINAXDUSTGRIDSTRUCTURE_HPP
#define LOGLINAXDUSTGRIDSTRUCTURE_HPP

#include "AxDustGridStructure.hpp"

////////////////////////////////////////////////////////////////////

/** The LogLinAxDustGridStructure class is a subclass of the AxDustGridStructure class and
    represents two-dimensional, axisymmetric dust grid structures with a logarithmic distribution
    of grid points in the radial direction and a linear distribution in the vertical direction. */
class LogLinAxDustGridStructure : public AxDustGridStructure
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a cylindrical grid structure with a radial logarithmic and axial linear distribution")

    Q_CLASSINFO("Property", "radialInnerExtent")
    Q_CLASSINFO("Title", "the inner radius in the radial direction")
    Q_CLASSINFO("Quantity", "length")
    Q_CLASSINFO("MinValue", "0")

    Q_CLASSINFO("Property", "radialOuterExtent")
    Q_CLASSINFO("Title", "the outer radius in the radial direction")
    Q_CLASSINFO("Quantity", "length")
    Q_CLASSINFO("MinValue", "0")

    Q_CLASSINFO("Property", "radialPoints")
    Q_CLASSINFO("Title", "the number of radial grid points")
    Q_CLASSINFO("MinValue", "5")
    Q_CLASSINFO("MaxValue", "100000")
    Q_CLASSINFO("Default", "250")

    Q_CLASSINFO("Property", "axialExtent")
    Q_CLASSINFO("Title", "the outer radius in the axial direction")
    Q_CLASSINFO("Quantity", "length")
    Q_CLASSINFO("MinValue", "0")

    Q_CLASSINFO("Property", "axialPoints")
    Q_CLASSINFO("Title", "the number of axial grid points")
    Q_CLASSINFO("MinValue", "5")
    Q_CLASSINFO("MaxValue", "100000")
    Q_CLASSINFO("Default", "250")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE LogLinAxDustGridStructure();

protected:
    /** This function verifies the validity of the number of bins and the maximum extent of the
        grid in the radial and vertical directions, and then calculates the \f$N_R+1\f$ radial grid
        points and the \f$N_z+1\f$ vertical grid points. */
    void setupSelfBefore();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the inner radius in the radial direction. */
    Q_INVOKABLE void setRadialInnerExtent(double value);

    /** Returns the inner radius in the radial direction. */
    Q_INVOKABLE double radialInnerExtent() const;

    /** Sets the outer radius in the radial direction. */
    Q_INVOKABLE void setRadialOuterExtent(double value);

    /** Returns the outer radius in the radial direction. */
    Q_INVOKABLE double radialOuterExtent() const;

    /** Sets the number of radial grid points. */
    Q_INVOKABLE void setRadialPoints(int value);

    /** Returns the number of radial grid points. */
    Q_INVOKABLE int radialPoints() const;

    /** Sets the outer radius in the axial direction. */
    Q_INVOKABLE void setAxialExtent(double value);

    /** Returns the outer radius in the axial direction. */
    Q_INVOKABLE double axialExtent() const;

    /** Sets the number of axial grid points. */
    Q_INVOKABLE void setAxialPoints(int value);

    /** Returns the number of axial grid points. */
    Q_INVOKABLE int axialPoints() const;

    //======================== Data Members ========================

private:
    double _Rmin;
};

////////////////////////////////////////////////////////////////////

#endif // LOGLINAXDUSTGRIDSTRUCTURE_HPP
