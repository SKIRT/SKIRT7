/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef TTAURIDISKGEOMETRY_HPP
#define TTAURIDISKGEOMETRY_HPP

#include "AxGeometry.hpp"

////////////////////////////////////////////////////////////////////

/** The TTauriDiskGeometry class is a subclass of the AxGeometry class and describes the
    geometry of a typical passive disk around a T Tauri star. The disks are axisymmetric and
    have a central cavity and are characterized by the density distribution \f[ \rho(R,z) = \rho_0
    \left(\frac{R}{R_d}\right)^{-1} \exp\left\{ -\frac{\pi}{4} \left[ \frac{z}{z_d
    \left(\frac{R}{R_d}\right)^{9/8}} \right]^2 \right\} \qquad\qquad R_{\text{inn}} < R <
    R_{\text{out}}. \f] There are four parameters in this class: the inner and outer radii
    \f$R_{\text{inn}}\f$ and \f$R_{\text{out}}\f$, the radial scale length \f$R_d\f$ and the
    vertical scale height \f$z_d\f$. This geometry is used by Pascucci et al. (2004, A&A, 417, 793)
    as a benchmark problem for 2D LTE radiative transfer problems. */
class TTauriDiskGeometry : public AxGeometry
{
    Q_OBJECT
    Q_CLASSINFO("Title", "a T Tauri disk geometry")

    Q_CLASSINFO("Property", "minRadius")
    Q_CLASSINFO("Title", "the inner radius of the disk")
    Q_CLASSINFO("Quantity", "length")
    Q_CLASSINFO("MinValue", "0")

    Q_CLASSINFO("Property", "maxRadius")
    Q_CLASSINFO("Title", "the outer radius of the disk")
    Q_CLASSINFO("Quantity", "length")
    Q_CLASSINFO("MinValue", "0")

    Q_CLASSINFO("Property", "radialScale")
    Q_CLASSINFO("Title", "the radial scale length")
    Q_CLASSINFO("Quantity", "length")
    Q_CLASSINFO("MinValue", "0")

    Q_CLASSINFO("Property", "axialScale")
    Q_CLASSINFO("Title", "the axial scale height")
    Q_CLASSINFO("Quantity", "length")
    Q_CLASSINFO("MinValue", "0")

    //============= Construction - Setup - Destruction =============

public:
    /** The default constructor. */
    Q_INVOKABLE TTauriDiskGeometry();

protected:
    /** This function accepts the inner and outer radii \f$R_{\text{inn}}\f$ and
        \f$R_{\text{out}}\f$, the radial scale length \f$R_d\f$ and the vertical scale height
        \f$z_d\f$. It is the responsibility of the user that the adopted parameters are valid and
        logical, the program does not test for this. The normalization constant \f$\rho_0\f$ is
        determined by requiring that total mass equals one. Since \f[ \int_0^\infty
        \exp\left[-\frac{\pi}{4} \left(\frac{z}{h}\right)^2 \right] {\text{d}}z = h, \f] we find
        that \f[ \int_{-\infty}^\infty \rho(R,z)\,{\text{d}}z = 2\rho_0 z_d
        \left(\frac{R}{R_d}\right)^{1/8}, \f] and consequently \f[ M \equiv 2\pi \int_0^\infty
        R\,{\text{d}}R \int_{-\infty}^\infty \rho(R,z)\,{\text{d}}z = \frac{32\pi}{17}\,\rho_0
        R_d^2 z_d \left[ \left(\frac{R_{\text{out}}}{R_d}\right)^{17/8} -
        \left(\frac{R_{\text{inn}}}{R_d}\right)^{17/8} \right]. \f] The normalization constant
        \f$\rho_0\f$ hence is \f[ \rho_0 = \frac{17}{32\pi}\,\frac{1}{R_d^2 z_d} \left[
        \left(\frac{R_{\text{out}}}{R_d}\right)^{17/8} -
        \left(\frac{R_{\text{inn}}}{R_d}\right)^{17/8} \right]^{-1}. \f] */
    void setupSelfBefore();

    //======== Setters & Getters for Discoverable Attributes =======

public:
    /** Sets the inner radius of the disk. */
    Q_INVOKABLE void setMinRadius(double value);

    /** Returns the inner radius of the disk. */
    Q_INVOKABLE double minRadius() const;

    /** Sets the outer radius of the disk. */
    Q_INVOKABLE void setMaxRadius(double value);

    /** Returns the outer radius of the disk. */
    Q_INVOKABLE double maxRadius() const;

    /** Sets the radial scale length. */
    Q_INVOKABLE void setRadialScale(double value);

    /** Returns the radial scale length. */
    Q_INVOKABLE double radialScale() const;

    /** Sets the vertical scale height. */
    Q_INVOKABLE void setAxialScale(double value);

    /** Returns the vertical scale height. */
    Q_INVOKABLE double axialScale() const;

    //======================== Other Functions =======================

public:
    /** This function returns the density \f$\rho(R,z)\f$ at the cylindrical radius
        \f$R\f$ and height \f$z\f$. It just implements the analytical formula. */
    double density(double R, double z) const;

    /** This function generates a random position from the geometry, by
        drawing a random point from the three-dimensional probability density \f$p({\bf{r}})\,
        {\text{d}}{\bf{r}} = \rho({\bf{r}})\, {\text{d}}{\bf{r}}\f$. In the present case, we accomplish
        this by picking a random cylindrical radius \f$R\f$, a random height
        \f$z\f$, and a random azimuth \f$\phi\f$ from the appropriate one-dimensional probability
        distribution functions (PDF). We start by simply generating the azimuth from a uniform
        distribution between 0 and \f$2\pi\f$. Subsequently we generate a random radius \f$R\f$ from the
        marginal distribution \f[ p(R)\,{\text{d}}R = \left[ 2\pi \int_{-\infty}^\infty \rho(R,z)\, R\,
        {\text{d}}z \right] {\text{d}}R = \frac{17}{8}\, \frac{R^{9/8}}{R_{\text{out}}^{17/8}-
        R_{\text{inn}}^{17/8}}. \f] The cumulative distribution is \f[ P(R) = \int_{R_{\text{inn}}}^R
        p(R')\,{\text{d}}R' = \frac{ R^{17/8}-R_{\text{inn}}^{17/8} }{ R_{\text{out}}^{17/8}-
        R_{\text{inn}}^{17/8} }, \f] so random \f$R\f$ is generated by picking a uniform deviate
        \f${\cal{X}}\f$ and setting \f[ R = \left[ R_{\text{inn}}^{17/8} + {\cal{X}}
        \left( R_{\text{out}}^{17/8}-R_{\text{inn}}^{17/8} \right) \right]^{8/17}. \f] Finally, we
        generate a random height from the conditional distribution function \f[ p(z)\,{\text{d}}z
        = \dfrac{\rho(R,z)\,{\text{d}}z}{\int_{-\infty}^\infty \rho(R,z')\, {\text{d}}z'}, \f] where
        \f$R\f$ is the random cylindrical radius generated before. One finds that this distribution is
        a Gaussian distribution with mean zero and dispersion \f[ \sigma(R) =
        \sqrt{\frac{2}{\pi}}\,z_d\left(\frac{R}{R_d}\right)^{9/8}. \f] Generating a random \f$z\f$
        from this distribution is easy as the Random class contains a gaussian random number generating
        function. */
    Position generatePosition() const;

    /** This function returns the radial surface density, i.e. the integration of
        the density along a line in the equatorial plane starting at the centre of the coordinate
        system, \f[ \Sigma_R = \int_0^\infty \rho(R,0)\,{\text{d}}R. \f] For the T Tauri geometry,
        \f[ \Sigma_R = \rho_0 R_d \ln \left( \frac{R_{\text{out}}}{R_{\text{inn}}}
        \right). \f] */
    double SigmaR() const;

    /** This function returns the Z-axis surface density, i.e. the integration of
        the density along the entire Z-axis, \f[ \Sigma_Z = \int_{-\infty}^\infty \rho(0,0,z)\,
        {\text{d}}z. \f] For the T Tauri geometry with its central cylindrical cavity, this integral
        is simply zero. */
    double SigmaZ() const;

    //======================== Data Members ========================

private:
    // data members for which there are setters and getters
    double _Rinn;
    double _Rout;
    double _Rd;
    double _zd;

    // data members initialized during setup
    double _rho0;
};

////////////////////////////////////////////////////////////////////

#endif // TTAURIDISKGEOMETRY_HPP
