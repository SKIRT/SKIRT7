/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef LOGSPHEDUSTGRIDSTRUCTURE_HPP
#define LOGSPHEDUSTGRIDSTRUCTURE_HPP

#include "SpheDustGridStructure.hpp"

////////////////////////////////////////////////////////////////////

/** The LogSpheDustGridStructure class is a subclass of the SpheDustGridStructure class, and
    represents one-dimensional, spherically symmetric dust grid structures with a logarithmic
    distribution of the radial grid points. */
class LogSpheDustGridStructure : public SpheDustGridStructure
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a spherical grid structure with a logarithmic distribution")

    Q_CLASSINFO("Property", "innerExtent")
    Q_CLASSINFO("Title", "the inner radius")
    Q_CLASSINFO("Quantity", "length")
    Q_CLASSINFO("MinValue", "0")

    Q_CLASSINFO("Property", "outerExtent")
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
    Q_INVOKABLE LogSpheDustGridStructure();

protected:
    /** This function verifies the validity of the number of radial bins \f$N_r\f$, the minimum
        radius \f$r_{\text{min}}\f$ and the maximum radius \f$r_{\text{max}}\f$, and then
        calculates the \f$N_r+1\f$ radial grid points. */
    void setupSelfBefore();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the inner radius. */
    Q_INVOKABLE void setInnerExtent(double value);

    /** Returns the inner radius. */
    Q_INVOKABLE double innerExtent() const;

    /** Sets the outer radius. */
    Q_INVOKABLE void setOuterExtent(double value);

    /** Returns the outer radius. */
    Q_INVOKABLE double outerExtent() const;

    /** Sets the number of radial grid points. */
    Q_INVOKABLE void setPoints(int value);

    /** Returns the number of radial grid points. */
    Q_INVOKABLE int points() const;

    //======================== Data Members ========================

private:
    double _rmin;
};

////////////////////////////////////////////////////////////////////

#endif // LOGSPHEDUSTGRIDSTRUCTURE_HPP
