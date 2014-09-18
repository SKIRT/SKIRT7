/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef POWAXDUSTGRIDSTRUCTURE_HPP
#define POWAXDUSTGRIDSTRUCTURE_HPP

#include "AxDustGridStructure.hpp"

//////////////////////////////////////////////////////////////////////

/** The PowAxDustGridStructure class is a subclass of the AxDustGridStructure class and represents
    two-dimensional, axisymmetric dust grid structures with a power-law distribution of both the
    radial and vertical grid points. */
class PowAxDustGridStructure : public AxDustGridStructure
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a cylindrical grid structure with a power-law distribution")

    Q_CLASSINFO("Property", "radialExtent")
    Q_CLASSINFO("Title", "the outer radius in the radial direction")
    Q_CLASSINFO("Quantity", "length")
    Q_CLASSINFO("MinValue", "0")

    Q_CLASSINFO("Property", "radialRatio")
    Q_CLASSINFO("Title", "the ratio of the inner- and outermost bin widths in the radial direction")
    Q_CLASSINFO("MinValue", "0")
    Q_CLASSINFO("MaxValue", "10000")
    Q_CLASSINFO("Default", "50")

    Q_CLASSINFO("Property", "radialPoints")
    Q_CLASSINFO("Title", "the number of radial grid points")
    Q_CLASSINFO("MinValue", "5")
    Q_CLASSINFO("MaxValue", "100000")
    Q_CLASSINFO("Default", "250")

    Q_CLASSINFO("Property", "axialExtent")
    Q_CLASSINFO("Title", "the outer radius in the axial direction")
    Q_CLASSINFO("Quantity", "length")
    Q_CLASSINFO("MinValue", "0")

    Q_CLASSINFO("Property", "axialRatio")
    Q_CLASSINFO("Title", "the ratio of the inner- and outermost bin widths in the axial direction")
    Q_CLASSINFO("MinValue", "0")
    Q_CLASSINFO("MaxValue", "10000")
    Q_CLASSINFO("Default", "50")

    Q_CLASSINFO("Property", "axialPoints")
    Q_CLASSINFO("Title", "the number of axial grid points")
    Q_CLASSINFO("MinValue", "5")
    Q_CLASSINFO("MaxValue", "100000")
    Q_CLASSINFO("Default", "250")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE PowAxDustGridStructure();

protected:
    /** This function verifies the validity of the input parameters, and then calculates the
        \f$N_R+1\f$ radial grid points and the \f$N_z+1\f$ vertical grid points. */
    void setupSelfBefore();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the outer radius in the radial direction. */
    Q_INVOKABLE void setRadialExtent(double value);

    /** Returns the outer radius in the radial direction. */
    Q_INVOKABLE double radialExtent() const;

    /** Sets the ratio of the widths of the inner- and outermost bin in the radial direction. */
    Q_INVOKABLE void setRadialRatio(double value);

    /** Returns the ratio of the widths of the inner- and outermost bin in the radial direction. */
    Q_INVOKABLE double radialRatio() const;

    /** Sets the number of radial grid points. */
    Q_INVOKABLE void setRadialPoints(int value);

    /** Returns the number of radial grid points. */
    Q_INVOKABLE int radialPoints() const;

    /** Sets the outer radius in the axial direction. */
    Q_INVOKABLE void setAxialExtent(double value);

    /** Returns the outer radius in the axial direction. */
    Q_INVOKABLE double axialExtent() const;

    /** Sets the ratio of the widths of the inner- and outermost bin in the axial direction. */
    Q_INVOKABLE void setAxialRatio(double value);

    /** Returns the ratio of the widths of the inner- and outermost bin in the axial direction. */
    Q_INVOKABLE double axialRatio() const;

    /** Sets the number of axial grid points. */
    Q_INVOKABLE void setAxialPoints(int value);

    /** Returns the number of axial grid points. */
    Q_INVOKABLE int axialPoints() const;

    //======================== Data Members ========================

private:
    // discoverable attributes
    //  - stored in base class: _NR, _Rmax, _Nz, _zmax, _zmin == -_zmax
    //  - in addition to data members in base class:
    double _Rratio;
    double _zratio;
};

//////////////////////////////////////////////////////////////////////

#endif // POWAXDUSTGRIDSTRUCTURE_HPP
