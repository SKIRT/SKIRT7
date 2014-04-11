/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef TRUST6GEOMETRY_HPP
#define TRUST6GEOMETRY_HPP

#include "AxGeometry.hpp"

////////////////////////////////////////////////////////////////////

/** The Trust6Geometry class is a subclass of the AxGeometry class... */
class Trust6Geometry : public AxGeometry
{
    Q_OBJECT
    Q_CLASSINFO("Title", "the geometry from the TRUST6 benchmark model")

    Q_CLASSINFO("Property", "openingAngle")
    Q_CLASSINFO("Title", "the opening angle of the cavity")
    Q_CLASSINFO("Quantity", "posangle")
    Q_CLASSINFO("MinValue", "0 deg")
    Q_CLASSINFO("MaxValue", "180 deg")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE Trust6Geometry();

protected:
    /**  */
    void setupSelfBefore();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the opening angle of the cavity. */
    Q_INVOKABLE void setOpeningAngle(double value);

    /** Returns the opening angle of the cavity. */
    Q_INVOKABLE double openingAngle() const;

    //======================== Other Functions =======================

public:
    /** This function returns the density \f$\rho(R,z)\f$ at the cylindrical radius
        \f$R\f$ and height \f$z\f$. It just implements the analytical formula. */
    double density(double R, double z) const;

    /** This function generates a random position from the TRUST6 geometry. Not implemented (yet) for
        this geometry... */
    Position generatePosition() const;

    /** This function returns the radial surface density, i.e. the integration of
        the density along a line in the equatorial plane starting at the centre of the coordinate
        system, \f[ \Sigma_R = \int_0^\infty \rho(R,0)\,{\text{d}}R. \f] */
    double SigmaR() const;

    /** This function returns the Z-axis surface density, i.e. the integration of
        the density along the entire Z-axis, \f[ \Sigma_Z = \int_{-\infty}^\infty \rho(0,0,z)\,
        {\text{d}}z. \f] */
    double SigmaZ() const;

    //======================== Data Members ========================

private:
    // data members for which there are setters and getters
    double _thetaop;

    // data members initialized during setup
    double _rhoambient;
    double _rhoshell;
    double _rmin;
    double _rmax;
    double _rshell;
    double _sigmashell;
};

////////////////////////////////////////////////////////////////////

#endif // TRUST6GEOMETRY_HPP
