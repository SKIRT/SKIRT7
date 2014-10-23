/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef LOGLINAXSPHEDUSTGRIDSTRUCTURE_HPP
#define LOGLINAXSPHEDUSTGRIDSTRUCTURE_HPP

#include "AxSpheDustGridStructure.hpp"

////////////////////////////////////////////////////////////////////

/** The LogLinAxSpheDustGridStructure class is a subclass of the AxSpheDustGridStructure class and
    represents two-dimensional, axisymmetric dust grid structures using spherical coordinates with
    a logarithmic distribution of grid points in the radial direction and a linear distribution in
    the angular direction. */
class LogLinAxSpheDustGridStructure : public AxSpheDustGridStructure
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a spherical axisymmetric grid structure with a radial logarithmic and angular linear distribution")

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
    Q_CLASSINFO("MinValue", "4")
    Q_CLASSINFO("MaxValue", "100000")
    Q_CLASSINFO("Default", "250")

    Q_CLASSINFO("Property", "angularPoints")
    Q_CLASSINFO("Title", "the number of angular grid points")
    Q_CLASSINFO("MinValue", "4")
    Q_CLASSINFO("MaxValue", "100000")
    Q_CLASSINFO("Default", "250")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE LogLinAxSpheDustGridStructure();

protected:
    /** This function verifies the validity of the number of bins and the maximum extent of the
        grid in the radial and vertical directions, and then calculates the \f$N_r+1\f$ radial grid
        points and the \f$N_\theta+1\f$ angular grid points. */
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

    /** Sets the number of angular grid points. */
    Q_INVOKABLE void setAngularPoints(int value);

    /** Returns the number of angular grid points. */
    Q_INVOKABLE int angularPoints() const;

    //======================== Data Members ========================

private:
    double _rmin;
};

////////////////////////////////////////////////////////////////////

#endif // LOGLINAXSPHEDUSTGRIDSTRUCTURE_HPP
