/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef EXPDISKGEOMETRY_HPP
#define EXPDISKGEOMETRY_HPP

#include "SepAxGeometry.hpp"

////////////////////////////////////////////////////////////////////

/** The ExpDiskGeometry class is a subclass of the SepAxGeometry class, and describes
    axisymmetric geometries characterized by a double-exponential profile, in which the
    density decreases exponentially in the radial and the vertical directions. A truncation
    can be applied in both the radial and vertical direction.  In formula form \f[ \rho(R,z) =
    \rho_0\, {\text{e}}^{-\frac{R}{h_R}-\frac{|z|}{h_z}}, \f] for
    \f$R\leq R_{\text{max}}\f$ and \f$|z|\leq z_{\text{max}}\f$. The model contains four free
    parameters: the radial scale length \f$h_R\f$, the vertical scale height \f$h_z\f$, the radial
    truncation radius \f$R_{\text{max}}\f$, and the vertical truncation radius
    \f$z_{\text{max}}\f$. */
class ExpDiskGeometry : public SepAxGeometry
{
    Q_OBJECT
    Q_CLASSINFO("Title", "an exponential disk geometry")

    Q_CLASSINFO("Property", "radialScale")
    Q_CLASSINFO("Title", "the radial scale length")
    Q_CLASSINFO("Quantity", "length")
    Q_CLASSINFO("MinValue", "0")

    Q_CLASSINFO("Property", "axialScale")
    Q_CLASSINFO("Title", "the axial scale height")
    Q_CLASSINFO("Quantity", "length")
    Q_CLASSINFO("MinValue", "0")

    Q_CLASSINFO("Property", "radialTrunc")
    Q_CLASSINFO("Title", "the radial truncation length (zero means no truncation)")
    Q_CLASSINFO("Quantity", "length")
    Q_CLASSINFO("MinValue", "0")
    Q_CLASSINFO("Default", "0")

    Q_CLASSINFO("Property", "axialTrunc")
    Q_CLASSINFO("Title", "the axial truncation height (zero means no truncation)")
    Q_CLASSINFO("Quantity", "length")
    Q_CLASSINFO("MinValue", "0")
    Q_CLASSINFO("Default", "0")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE ExpDiskGeometry();

protected:
    /** This function verifies the validity of the parameters. The central density \f$\rho_0\f$ is
        set by the normalization condition that the total mass equals one. One finds easily \f[
        \rho_0 = \left[ 4\pi\, h_R^2\, h_z \left( 1 - {\text{e}}^{-z_{\text{max}}/h_z} \right)
        \left[ 1 - \left( 1+\frac{R_{\text{max}}}{h_R} \right) \left(
        {\text{e}}^{-R_{\text{max}}/h_R} \right) \right] \right]^{-1}. \f] In case there is no
        truncation in either radial or vertical directions, this reduces to \f[ \rho_0 = \frac{1}{
        4\pi\, h_R^2\, h_z }. \f] */
    void setupSelfBefore();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the disk scale length \f$h_R\f$. */
    Q_INVOKABLE void setRadialScale(double value);

    /** Returns the disk scale length \f$h_R\f$. */
    Q_INVOKABLE double radialScale() const;

    /** Sets the disk scale height \f$h_z\f$. */
    Q_INVOKABLE void setAxialScale(double value);

    /** Returns the disk scale height \f$h_z\f$. */
    Q_INVOKABLE double axialScale() const;

    /** Sets the radial truncation length \f$R_{\text{max}}\f$ (zero means no truncation). */
    Q_INVOKABLE void setRadialTrunc(double value);

    /** Returns the radial truncation length \f$R_{\text{max}}\f$ (zero means no truncation). */
    Q_INVOKABLE double radialTrunc() const;

    /** Sets the axial truncation height \f$z_{\text{max}}\f$ (zero means no truncation). */
    Q_INVOKABLE void setAxialTrunc(double value);

    /** Returns the axial truncation height \f$z_{\text{max}}\f$ (zero means no truncation). */
    Q_INVOKABLE double axialTrunc() const;

    //======================== Other Functions =======================

    /** This function returns the density \f$\rho(R,z)\f$ at the cylindrical radius \f$R\f$ and
        height \f$z\f$. It just implements the analytical formula. */
    double density(double R, double z) const;

    /** This function returns the cylindrical radius \f$R\f$ of a random position drawn from the
        geometry, by picking a uniform deviate \f${\cal{X}}\f$
        and solving the equation \f[ {\cal{X}} = 2\pi \int_0^R \rho_R(R')\, R'\, {\text{d}}R' \f]
        for \f$R\f$. Substituting the exponential radial profile (without truncation) into this
        equation, we obtain \f[ {\cal{X}} = 1 - \left( 1+\frac{R}{h_R}
        \right) \exp \left( -\frac{R}{h_R} \right). \f] This equation can be solved by means of
        the Lambert function of order \f$-1\f$, yielding \f[ R = h_R \left[ -1-W_{-1} \left(
        \frac{ {\cal{X}}-1}{\text{e}} \right) \right]. \f] The Lambert function \f$W_{-1}(z)\f$
        is implemented in the function SpecialFunctions::LambertW1. The truncation is taken into
        account by rejecting values larger than \f$R_{\text{max}}\f$. */
    double randomR() const;

    /** This function returns the height \f$z\f$ of a random position drawn from the
        geometry, by picking a uniform deviate \f${\cal{X}}\f$
        and solving the equation \f[ {\cal{X}} = \int_{-\infty}^z \rho_z(z')\, {\text{d}}z' \f]
        for \f$z\f$. For the exponential disk geometry, this
        integration is simple, and the inversion results in \f[ z = \begin{cases} \;
        h_z\,\ln(2{\cal{X}}) & \text{if $0<{\cal{X}}<\tfrac{1}{2}$,} \\ \;-h_z\,\ln[2(1-{\cal{X}})]
        & \text{if $\tfrac{1}{2}<{\cal{X}}<1$.} \end{cases} \f] The truncation is taken into
        account by rejecting values \f$|z|\f$ larger than \f$z_{\text{max}}\f$.*/
    double randomz() const;

    /** This function returns the surface density along a line in the equatorial plane
        starting at the centre of the coordinate system, i.e. \f[ \Sigma_R =
        \int_0\infty \rho(R,0)\,{\text{d}}R. \f] For the exponential disc geometry we find \f[
        \Sigma_R = \rho_0 h_R \left( 1 - {\text{e}}^{-R_{\text{max}}/h_R} \right), \f] which
        reduces to \f$ \Sigma_R = \rho_0 h_R \f$ if there is no radial truncation. */
    double SigmaR() const;

    /** This function returns the surface density along the Z-axis, i.e.
        the integration of the density along the entire Z-axis, \f[
        \Sigma_Z = \int_{-\infty}^\infty \rho(0,0,z)\, {\text{d}}z.\f] For the exponential
        disc geometry we find \f[ \Sigma_Z = 2\,\rho_0 h_Z \left( 1 - {\text{e}}^{-z_{\text{max}}/h_z}
        \right), \f] which reduces to \f$ \Sigma_Z = 2\,\rho_0 h_z \f$ if there is no vertical
        truncation. */
    double SigmaZ() const;

    //======================== Data Members ========================

private:
    // data members for which there are setters and getters
    double _hR;
    double _hz;
    double _Rmax;
    double _zmax;

    // data members initialized during setup
    double _rho0;
};

////////////////////////////////////////////////////////////////////

#endif // EXPDISKGEOMETRY_HPP
