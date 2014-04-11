/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef LINAXDUSTGRIDSTRUCTURE_HPP
#define LINAXDUSTGRIDSTRUCTURE_HPP

#include "AxDustGridStructure.hpp"

////////////////////////////////////////////////////////////////////

/** The LinAxDustGridStructure class is a subclass of the AxDustGridStructure class, and represents
    two-dimensional, axisymmetric dust grid structures with a linear distribution of both the
    radial bins and vertical grid points. */
class LinAxDustGridStructure : public AxDustGridStructure
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a cylindrical grid structure with a linear distribution")

    Q_CLASSINFO("Property", "radialExtent")
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
    Q_INVOKABLE LinAxDustGridStructure();

protected:
    /** This function verifies the validity of the number of bins and the maximum extent of the
        grid in the radial and vertical directions, and then calculates the \f$N_R+1\f$ radial grid
        points \f[ R_i = \frac{i\,R_{\text{max}}}{N_R} \qquad i=0,\ldots,N_R, \f] and the
        \f$N_z+1\f$ vertical grid points \f[ z_k = -z_{\text{max}} + \frac{2k\,z_{\text{max}}}{N_z}
        \qquad k=0,\ldots,N_z. \f] */
    void setupSelfBefore();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the outer radius in the radial direction. */
    Q_INVOKABLE void setRadialExtent(double value);

    /** Returns the outer radius in the radial direction. */
    Q_INVOKABLE double radialExtent() const;

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
};

////////////////////////////////////////////////////////////////////

#endif // LINAXDUSTGRIDSTRUCTURE_HPP
