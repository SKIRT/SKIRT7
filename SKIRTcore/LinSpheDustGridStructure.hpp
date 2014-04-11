/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef LINSPHEDUSTGRIDSTRUCTURE_HPP
#define LINSPHEDUSTGRIDSTRUCTURE_HPP

#include "SpheDustGridStructure.hpp"

////////////////////////////////////////////////////////////////////

/** The LinSpheDustGridStructure class is a subclass of the SpheDustGridStructure class, and
    represents one-dimensional, spherically symmetric dust grid structures with a linear
    distribution of the radial grid points. */
class LinSpheDustGridStructure : public SpheDustGridStructure
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a spherical grid structure with a linear distribution")

    Q_CLASSINFO("Property", "extent")
    Q_CLASSINFO("Title", "the outer radius")
    Q_CLASSINFO("Quantity", "length")
    Q_CLASSINFO("MinValue", "0")

    Q_CLASSINFO("Property", "points")
    Q_CLASSINFO("Title", "the number of radial grid points")
    Q_CLASSINFO("MinValue", "5")
    Q_CLASSINFO("MaxValue", "100000")
    Q_CLASSINFO("Default", "250")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE LinSpheDustGridStructure();

protected:
    /** This function verifies the validity of the number of radial bins \f$N_r\f$ and the maximum
        radius \f$r_{\text{max}}\f$ and then calculates the \f$N_r+1\f$ radial grid points \f[ r_i
        = \frac{i\,r_{\text{max}}}{N_r} \qquad i=0,\ldots,N_r. \f] */
    void setupSelfBefore();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the outer radius. */
    Q_INVOKABLE void setExtent(double value);

    /** Returns the outer radius. */
    Q_INVOKABLE double extent() const;

    /** Sets the number of radial grid points. */
    Q_INVOKABLE void setPoints(int value);

    /** Returns the number of radial grid points. */
    Q_INVOKABLE int points() const;
};

////////////////////////////////////////////////////////////////////

#endif // LINSPHEDUSTGRIDSTRUCTURE_HPP
