/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef POWSPHEDUSTGRIDSTRUCTURE_HPP
#define POWSPHEDUSTGRIDSTRUCTURE_HPP

#include "SpheDustGridStructure.hpp"

////////////////////////////////////////////////////////////////////

/** The PowSpheDustGridStructure class is a subclass of the SpheDustGridStructure class, and
    represents one-dimensional, spherically symmetric dust grid structures with a power-law
    distribution of the radial grid points. */
class PowSpheDustGridStructure : public SpheDustGridStructure
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a spherical grid structure with a power-law distribution")

    Q_CLASSINFO("Property", "extent")
    Q_CLASSINFO("Title", "the outer radius")
    Q_CLASSINFO("Quantity", "length")
    Q_CLASSINFO("MinValue", "0")

    Q_CLASSINFO("Property", "ratio")
    Q_CLASSINFO("Title", "the ratio of the inner- and outermost bin widths")
    Q_CLASSINFO("MinValue", "0")
    Q_CLASSINFO("MaxValue", "10000")
    Q_CLASSINFO("Default", "50")

    Q_CLASSINFO("Property", "points")
    Q_CLASSINFO("Title", "the number of radial grid points")
    Q_CLASSINFO("MinValue", "5")
    Q_CLASSINFO("MaxValue", "100000")
    Q_CLASSINFO("Default", "250")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE PowSpheDustGridStructure();

protected:
    /** This function verifies the validity of the number of radial bins \f$N_r\f$, the maximum
        radius \f$r_{\text{max}}\f$ and the ratio \f${\cal{R}}\f$ of the widths of the outermost
        and innermost dust cells. It then calculates the \f$N_r+1\f$ radial grid points. */
    void setupSelfBefore();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the outer radius. */
    Q_INVOKABLE void setExtent(double value);

    /** Returns the outer radius. */
    Q_INVOKABLE double extent() const;

    /** Sets the ratio of the inner- and outermost bin widths. */
    Q_INVOKABLE void setRatio(double value);

    /** Returns the ratio of the inner- and outermost bin widths. */
    Q_INVOKABLE double ratio() const;

    /** Sets the number of radial grid points. */
    Q_INVOKABLE void setPoints(int value);

    /** Returns the number of radial grid points. */
    Q_INVOKABLE int points() const;

    //======================== Data Members ========================

private:
    double _ratio;
};

////////////////////////////////////////////////////////////////////

#endif // POWSPHEDUSTGRIDSTRUCTURE_HPP
